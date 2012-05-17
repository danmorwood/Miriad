#include "matrix.h" 
#include <cstdio>

using namespace std;

Cudd mgr(0,0); 

ADD eq(ADD add, double value) {
	if(value)
		return add.Agreement(mgr.constant(value)).Cmpl().Cmpl();
	else
		return add.Cmpl();
}

ADD neq(ADD add, double value) {
	if(value)
		return add.Agreement(mgr.constant(value)).Cmpl();
	else
		return add.Cmpl().Cmpl();
}

SparceMatrix::SparceMatrix(unsigned int n, unsigned int m) {
	sizeN = n;
	mData.resize(n);
	sizeM = m;
}

void SparceMatrix::set(unsigned int x, unsigned int y, valueType value) {
	if(x >= sizeN || y >= sizeM) {
		printf("Error: array out of bounds... requested [%d, %d] in matrix of size [%d, %d]\n",
				x, y, sizeN, sizeM);
		return;
	}
	if(value == BACKGROUND)
		mData[x].erase(y);
	else
		mData[x][y] = value;
}

valueType SparceMatrix::get(unsigned int x, unsigned int y) {
	if(x >= sizeN || y >= sizeM) {
		printf("Error: array out of bounds... requested [%d, %d] in matrix of size [%d, %d]\n",
				x, y, sizeN, sizeM);
		return BACKGROUND;
	}
	map<unsigned int, valueType>::iterator it = mData[x].find(y);
	if(it == mData[x].end())
		return BACKGROUND;
	else
		return it->second;
}

void SparceMatrix::swapRow(unsigned int r1, unsigned int r2) {
	if(r1 >= sizeN || r2 >= sizeN) {
		printf("Error: Requested rows %d and %d swaped in matrix of size [%d, %d]\n",
				r1, r2, sizeN, sizeM);
		return;
	}
	mData[r1].swap(mData[r2]);
}

void SparceMatrix::swapEntryInColumn(unsigned int i, unsigned int k, ADD kPrime) {
	valueType tmp = get(k, i);
	valueType A_ki = BACKGROUND;
	for(int j = k; j < sizeM; j++) {
		A_ki += get(j, i) * eq(kPrime, j);
	}
	set(k,i, A_ki);
	for(int j = k+1; j < sizeM; j++) {
		ADD Different = neq(kPrime, j);
		set(j, i, get(j,i) * Different + tmp * Different.Cmpl());
	}
}

void swapInMatrix(ADDvector& vec, int k, ADD p) {
	valueType tmp = vec[k];
	valueType newAdd = BACKGROUND;
	for(int j = k; j < vec.count(); j++) {
		newAdd += vec[j] * eq(p, j);
	}
	vec[k] = newAdd;
	for(int j = k+1; j < vec.count(); j++) {
		ADD Different = neq(p, j);
		vec[j] = Different * vec[j] + tmp * Different.Cmpl();
	}
}

void SparceMatrix::swapColumn(unsigned int c1, unsigned int c2) {
	if(c1 >= sizeM || c2 >= sizeM) {
		printf("Error: Requested columns %d and %d swaped in matrix of size [%d, %d]\n",
				c1, c2, sizeN, sizeM);
		return;
	}
	for(int i = 0; i < sizeN; i++) {
		map<unsigned int, valueType>::iterator c1it = mData[i].find(c1);
		map<unsigned int, valueType>::iterator c2it = mData[i].find(c2);
		if(c1it != mData[i].end()) {
			valueType temp = c1it->second;
			if(c2it != mData[i].end()) {
				mData[i][c1] = c2it->second;
			} else {
				mData[i].erase(c1it);
			}
			mData[i][c2] = temp;
		} else {
			if(c2it != mData[i].end()) {
				mData[i][c1] = c2it->second;
				mData[i].erase(c2it);
			}
		}
	}
}

void SparceMatrix::Subtract(unsigned int r1, unsigned int r2) {
	map<unsigned int, valueType>::iterator r1it = mData[r1].begin();
	map<unsigned int, valueType>::iterator r2it = mData[r2].begin();
	while (r1it != mData[r1].end() && r2it != mData[r2].end()) {
		if(r1it->first < r2it->first) {
			r1it++;
		} else if( r2it->first < r1it->first) {
			mData[r1][r2it->first] = -r2it->second;
			r2it++;
		} else {
			mData[r1][r1it->first] = r1it->second - r2it->second;
			r1it++;
			r2it++;
		}
	}
	while(r2it != mData[r2].end()) {
		mData[r1][r2it->first] = -r2it->second;
		r2it++;
	}
}

void SparceMatrix::SubtractMultiple(unsigned int r1, valueType factor, unsigned int r2) {
	map<unsigned int, valueType>::iterator r1it = mData[r1].begin();
	map<unsigned int, valueType>::iterator r2it = mData[r2].begin();
	while (r1it != mData[r1].end() && r2it != mData[r2].end()) {
		if(r1it->first < r2it->first) {
			r1it++;
		} else if( r2it->first < r1it->first) {
			mData[r1][r2it->first] = -r2it->second*factor;;
			r2it++;
		} else {
			mData[r1][r1it->first] = r1it->second - r2it->second*factor;
			r1it++;
			r2it++;
		}
	}
	while(r2it != mData[r2].end()) {
		mData[r1][r2it->first] = -r2it->second*factor;
		r2it++;
	}
}

void SparceMatrix::Multiply(unsigned int row, valueType value) {
	for(map<unsigned int, valueType>::iterator it = mData[row].begin();
			it != mData[row].end(); it++) {
		it->second *= value;
	}
}

ADDvector SparceMatrix::LUPDecomposition() {
	for(int r = 0; r < sizeM; r++) {
		for(int c = 0; c < sizeN; c++) {
			fprintf(stderr, "%2.0f\t", Cudd_V(get(r, c).getNode()));
		}
		fprintf(stderr, "\n");
	}
	ADDvector pi(sizeM, &mgr);
	for(int i = 0; i < sizeM; i++) {
		pi[i] = mgr.constant(i);
	}
	for(int k = 0; k < sizeN; k++) {
		pi.DumpDot();
		valueType p = BACKGROUND;
		ADD kPrime = mgr.constant(k);
		for(int i = k; i < sizeN; i++) {
			valueType A_ik = get(i, k);
			valueType abs = A_ik.Maximum(-A_ik);
			p = p.Maximum(abs); 
			kPrime = kPrime.Maximum(p.Agreement(abs).Cmpl().Cmpl()*mgr.constant(i));
		}
		for(int i = 0; i < sizeN; i++) {
			swapEntryInColumn(i, k, kPrime);
		}
		swapInMatrix(pi, k, kPrime);
		valueType divisor = get(k, k);
		for(int i = k+1; i < sizeN; i++) {
			valueType A_ik = get(i, k).Divide(divisor);
			fprintf(stderr, "A[%d,%d] = %f\n", i,k, Cudd_V(A_ik.getNode()));
			for( int j = k+1; j < sizeN; j++) {
				fprintf(stderr, "A[%d,%d] = %f\n", i,j, Cudd_V(get(i,j).getNode()));
				fprintf(stderr, "A[%d,%d] = %f\n", k,j, Cudd_V(get(k,j).getNode()));
				set(i, j, get(i, j) - A_ik * get(k, j));
				fprintf(stderr, "A[%d,%d] = %f\n", i,j, Cudd_V(get(i,j).getNode()));
			}
			set(i,k, A_ik);
		}
	}
	return pi;
}

ADDvector SparceMatrix::Solve(ADDvector pi, ADDvector b) {
	ADDvector y(sizeM,&mgr);
	ADDvector x(sizeM,&mgr);
	for(int i = 0; i < sizeM; i++) {
		y[i] = x[i] = BACKGROUND;
	}
	for(int i = 0; i < sizeN; i++) {
		ADD b_pi = BACKGROUND;
		for(int j = 0; j < sizeM; j++) {
			b_pi += b[j] * eq(pi[i],j);
		}
		fprintf(stderr, "b_pi[%d] = %f\n", i, Cudd_V(b_pi.getNode()));
		ADD sum = BACKGROUND;
		for(int j = 0; j <= i-1; j++) {
			sum += get(i,j)*y[j];
		}
		fprintf(stderr, "sum[%d] = %f\n", i, Cudd_V(sum.getNode()));
		y[i] = b_pi - sum;
		fprintf(stderr, "y[%d] = %f\n", i, Cudd_V(y[i].getNode()));
	}
	for(int i = sizeN-1; i >= 0; i--) {
		ADD sum = BACKGROUND;
		for(int j = i+1; j < sizeN; j++) {
			sum += get(i,j) * x[j];
		}
		x[i] = y[i] - sum;
		x[i] = x[i].Divide(get(i,i));
	}
	return x;
}

