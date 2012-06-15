#include "InteriorPoint.h"

//Returns a 0-1 ADD for which positions are valid
ADD Valid(ADDvector x, ADDvector lambda, ADDvector nu) {
	//(x, nu) > 0...
	ADD ret = mgr.addOne();
	for(int i = 0; i < x.count(); i++) {
		ret *= x[i].OneZeroMaximum(mgr.addZero());
		ret *= nu[i].OneZeroMaximum(mgr.addZero());
	}
}

void takeStep(ADDvector&  x, ADDvector&  lambda, ADDvector&  nu, 
		           ADDvector& Dx, ADDvector& Dlambda, ADDvector& Dnu) {

	ADD s = mgr.addOne();
	ADD alpha = mgr.constant(.9);
	ADDvector Nx(Dx.count());
	ADDvector Nlambda(Dlambda.count());
	ADDvector Nnu(Dnu.count());

	//Start with a full step
	for(int i = 0; i < x.count(); i++) {
		Nx[i] = Dx[i] * s;
	}
	for(int i = 0; i < lambda.count(); i++) {
		Nlambda[i] = Dlambda[i] * s;
	}
	for(int i = 0; i < nu.count(); i++) {
		Nnu[i] = Dnu[i] * s;
	}

	//Decay step size while instances are not feasible...
	ADD feasible = Valid(Nx, Nlambda, Nnu);
	while (feasible != mgr.addOne()) {
		s *= alpha;
		for(int i = 0; i < x.count(); i++) {
			Nx[i] = Nx[i] * feasible + (x[i] + Dx[i] * s) * feasible.Cmpl();
		}
		for(int i = 0; i < lambda.count(); i++) {
			Nlambda[i] = Nlambda[i] * feasible + (lambda[i] + Dlambda[i] * s) * feasible.Cmpl();
		}
		for(int i = 0; i < nu.count(); i++) {
			Nnu[i] = Nnu[i] * feasible + (nu[i] + Dnu[i] * s) * feasible.Cmpl();
		}
		feasible = Valid(Nx, Nlambda, Nnu);
	}
	x = Nx;
	lambda = Nlambda;
	nu = Nnu;
}

void addD2(SparceMatrix& lhs, ADDvector& x, Plan& P, bool phase1 = false) {
	if(phase1)
		return; //The second dirivative of s is 0. So leave it alone.

	int nSteps = P.getNumSteps();
	for(int i = 0; i < nSteps; i++) {
		ADD tmp = x[i] - x[i+nSteps];
		ADD val = mgr.addOne().Divide(tmp*tmp);
		lhs.set(i,i,-val);
		lhs.set(i,i+nSteps,-val);
		lhs.set(i+nSteps,i,val);
		lhs.set(i+nSteps,i+nSteps,val);
	}
}

void addAandAT(int offset, SparceMatrix& lhs, Plan& P) {
	P.A()->insertWithOffset(lhs, offset, 0);
	P.A()->Transpose().insertWithOffset(lhs, 0, offset);
}

ADDvector Plan::F(ADDvector x, bool phase1 = false) {
	ADDvector y(mf.size());
	for(int i = 0; i < mf.size(); i++) {
		if(mf[i].inverted) {
			y[i] = -x[mf[i].index];
		} else {
			y[i] = x[mf[i].index];
		}
		if(phase1)
			y[i] -= x[x.count()-1]; //... - s
	}
	return y;
}

ADDvector Plan::DeltaF_0(ADDvector x, bool phase1 = false) {
	if(phase1) {
		ADDvector y(x.count());
		y[x.count() - 1] = -mgr.addOne();
		return y;
	}
	ADDvector y(x.count());
	for(int i = 0; i < numOfUnknownDurations; i++) {
		y[i] = mgr.addOne().Divide(x[i] - x[i+numOfUnknownDurations]);
		y[i+numOfUnknownDurations] = -y[i];
	}
}

void addDiagFx(int offset, ADDvector x, SparceMatrix& lhs, Plan& P, bool phase1 = false) {
	ADDvector f_x = P.F(x, phase1);
	for(int i = 0; i < x.count(); i++) {
		lhs.set(offset, offset, -f_x[i]);
		offset++;
	}
}

void addDF_x(int offset, ADDvector& x, ADDvector& lambda, SparceMatrix& lhs, Plan& P, bool phase1 = false) {
	SparceMatrix DF_x = P.DF_x(phase1);
	DF_x.Transpose().insertWithOffset(lhs, 0, offset);
	(-diag(lambda)*DF_x).insertWithOffset(lhs, offset, 0);
}

void addR_Dual(ADDvector& r_t, Plan& P, ADDvector& x, ADDvector& lambda, ADDvector& mu, bool phase1 = false) {
	ADDvector R_Dual = P.DeltaF_0(x, phase1) + P.DF_x(phase1).Transpose() * lambda + P.A()->Transpose() * mu;
	for(int i = 0; i < R_Dual.count(); i++) {
		r_t[i] = R_Dual[i];
	}
}

void addR_Cent(ADDvector& r_t, Plan& P, ADDvector& x, ADDvector& lambda, bool phase1 = false) {
	int t = 1;
	ADDvector R_Cent = -diag(lambda)*P.F(x, phase1) - ConstVector(1/t, lambda.count());
	for(int i = 0; i < R_Cent.count(); i++) {
		r_t[x.count() + i] = R_Cent[i];
	}
}

void addR_Pri(ADDvector& r_t, Plan& P, ADDvector& x, ADDvector& lambda, bool phase1 = false) {
	//note phase 1 and 2 are the same here but the parameter is added for
	//symetry with the other calls
	int offset = lambda.count() + x.count();
	ADDvector R_Pri = *(P.A()) * x; // - b?
	for(int i = 0; i < lambda.count(); i++) {
		r_t[i+offset] = R_Pri[i];
	}
}

ADDvector R_T(Plan& P, ADDvector& x, ADDvector& lambda, ADDvector& nu, bool phase1 = false) {
	ADDvector r_t(x.count() + lambda.count() + nu.count());
	addR_Dual(r_t, P, x, lambda, nu, phase1);
	addR_Cent(r_t, P, x, lambda, phase1);
	addR_Pri(r_t, P, x, lambda, phase1);
	return r_t;
}

void calcStepDirection (ADDvector&  x, ADDvector&  lambda, ADDvector&  nu,
		                ADDvector& Dx, ADDvector& Dlambda, ADDvector& Dnu,
						Plan& P, bool phase1 = false) {
	SparceMatrix lhs(x.count() + lambda.count() + nu.count(), x.count() + lambda.count() + nu.count());
	addD2(lhs, x, P, phase1);
	addDF_x(x.count(), x, lambda, lhs, P, phase1);
	addAandAT(x.count()*2, lhs, P);
	addDiagFx(x.count(), x, lhs, P, phase1);
	ADDvector r_t = R_T(P, x, lambda, nu, phase1);
	ADDvector pi = lhs.LUPDecomposition();
	ADDvector dir = lhs.Solve(pi, r_t);
	int index = 0;
	for(int i = 0; i < x.count(); i++) {
		Dx[i] = dir[index];
		index++;
	}
	for(int i = 0; i < x.count(); i++) {
		Dlambda[i] = dir[index];
		index++;
	}
	for(int i = 0; i < x.count(); i++) {
		Dnu[i] = dir[index];
		index++;
	}
}

ADD CalcDist(ADDvector vector, int start = 0, int end = -1) {
	if(end == -1)
		end = vector.count();
	ADD total = mgr.addZero();
	for(int i = start; i < end; i++) {
		total += vector[i] * vector[i];
	}
	return total.Sqrt();
}

double maximum(ADD a) {
	return Cudd_V(a.FindMax().getNode());
}

bool checkTeminationConditions(Plan& P, ADDvector& x, ADDvector& lambda, ADDvector& nu, bool phase1) {
	double epsilon_feas = .01;
	double epsilon = .01;
	ADDvector r_t = R_T(P, x, lambda, nu, phase1);
	ADD dist_r_pri = CalcDist(r_t, 0, x.count());
	ADD dist_r_dual = CalcDist(r_t, x.count() + lambda.count(), r_t.count());
	ADD lambdaTest = dotProduct(-P.F(x, phase1), lambda);
	return maximum(dist_r_pri) < epsilon_feas && 
		   maximum(dist_r_dual) < epsilon_feas && 
		   maximum(lambdaTest) < epsilon;
}

double evaluate(Plan& P) {
	//Compute Phase One
	ADDvector xPhase1(P.getNumStepsWithUnknownDurration() + P.getNumSteps() + P.getNumSlackVars() + 1);
	for(int i = 0; i < xPhase1.count(); i++) {
		xPhase1[i] = mgr.addZero();
	}
	xPhase1[xPhase1.count()-1] = mgr.constant(10000); //inital value for s
	ADDvector lambda(P.getNumFunctions());
	for(int i = 0; i < lambda.count(); i++) {
		lambda[i] = mgr.addZero();
	}
	ADDvector nu(P.getNumFunctions());
	for(int i = 0; i < nu.count(); i++) {
		nu[i] = mgr.addZero();
	}
	while(checkTeminationConditions(P, xPhase1, lambda, nu, true)) {
		ADDvector DxPhase1(xPhase1.count());
		ADDvector Dlambda(lambda.count());
		ADDvector Dnu(nu.count());
		calcStepDirection( xPhase1,  lambda,  nu,
				          DxPhase1, Dlambda, Dnu,
						   P, true);
		takeStep( xPhase1,  lambda,  nu,
				 DxPhase1, Dlambda, Dnu);
	}
	//Compute Phase two
	ADDvector x(xPhase1.count() -1);
	for(int i = 0; i < x.count(); i++) {
		x[i] = xPhase1[i];
	}
	while(checkTeminationConditions(P, x, lambda, nu, false)) {
		ADDvector Dx(x.count());
		ADDvector Dlambda(lambda.count());
		ADDvector Dnu(nu.count());
		calcStepDirection( x,  lambda,  nu,
				          Dx, Dlambda, Dnu,
						   P, false);
		takeStep( x,  lambda,  nu,
				 Dx, Dlambda, Dnu);
	}

	//Really I need to do some max's and sum's here but that can be added later.
	//Right now I need to add something to test all of this code.
	return 0;
}
