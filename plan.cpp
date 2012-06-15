#include "plan.h"
#include <vector>

using namespace std;

Link::Link(Step* pre, Step* post, Literal* literal) {
	mPre = pre;
	mPost = post;
	mLiteral = literal;
}

Step::Step(Action* action) {
	mAction = action;
}

Ordering::Ordering(Step* pre, StepTime::Time preTime, Step* post, StepTime::Time postTime) {
	mPre = pre;
	mPost = post;
	mPreTime = preTime;
	mPostTime = postTime;
}

Step* Ordering::Pre() {
	return mPre;
}

Step* Ordering::Post() {
	return mPost;
}

StepTime::Time Ordering::PreTime() {
	return mPreTime;
}

StepTime::Time Ordering::PostTime() {
	return mPostTime;
}

Action* Step::getAction() {
	return mAction;
}

Plan::Plan(Domain* domain) {
	mDomain = domain;
}

int Plan::getNumSteps() {
	return mSteps.size();
}

int Plan::getNumStepsWithUnknownDurration() {
	return numOfUnknownDurations;
}

int Plan::getNumSlackVars() {
	return numSlackVars;
}

int Plan::getNumFunctions() {
	return mf.size();
}

int Plan::getStepCol(Step* step, StepTime::Time time) {
	static map<Step*, int> offset;
	if(offset.find(step) == offset.end()) {
		int i = offset.size() * 2;
		offset[step] = i;
	}
	int stepOffset;
	if(step->getAction()->UBDurration() == step->getAction()->LBDurration()) {
		stepOffset = numOfUnknownDurations * 2 + offset[step];
	}
	if(time == StepTime::Start) {
		return stepOffset;
	} else {
		return stepOffset+1;
	}
}

int Plan::getSlackVar(int k) {
	if(k >= numSlackVars)
		numSlackVars = k+1;
	return numOfUnknownDurations + mSteps.size() + k;
}

SparceMatrix Plan::DF_x(bool phase1 = false) {
	int sizeOfX = numOfUnknownDurations + mSteps.size() + numSlackVars;
	if(phase1)
		sizeOfX++;
	SparceMatrix ret(mf.size(), sizeOfX); 
	for(int i = 0; i < mf.size(); i++) {
		if(mf[i].inverted) 
			ret.set(i, mf[i].index, -mgr.addOne());
		else
			ret.set(i, mf[i].index, mgr.addOne());
		if(phase1)
			ret.set(i, sizeOfX-1, -mgr.addOne());
	}
	return ret;
}

SparceMatrix* Plan::A() {
	return mA;
}

void Plan::CalcEncoding() {
	int n = mOrderings.size() + mSteps.size();
	if(mA)
		delete mA;
	mA = new SparceMatrix(n, mSteps.size()*3 + n);

	int row = 0; //The current equation
	int k = 0; //The current slack variable
	for(vector<Step*>::iterator it = mSteps.begin();
			it != mSteps.end(); it++) {
		Step* curStep = *it;
		if(curStep->getAction()->LBDurration() == 
				curStep->getAction()->UBDurration()) {
			//If they are the same we don't need slack variables
			//s_end - s_start = dur
			mA->set(row, getStepCol(curStep, StepTime::End), mgr.addOne());
			mA->set(row, getStepCol(curStep, StepTime::Start), -mgr.addOne());
			(*mb)[row] = mgr.constant(curStep->getAction()->UBDurration());
			row++;
		} else {
			//Other wise we do
			//s_end - s_start - s_k = ub;
			mA->set(row, getStepCol(curStep, StepTime::End), mgr.addOne());
			mA->set(row, getStepCol(curStep, StepTime::Start), -mgr.addOne());
			mA->set(row, getSlackVar(k), -mgr.addOne());
			(*mb)[row] = mgr.constant(curStep->getAction()->UBDurration());
			row++;
			k++;

			//s_start - s_end - s_k+1 = -lb;
			mA->set(row, getStepCol(curStep, StepTime::Start), mgr.addOne());
			mA->set(row, getStepCol(curStep, StepTime::End), -mgr.addOne());
			mA->set(row, getSlackVar(k), -mgr.addOne());
			(*mb)[row] = mgr.constant(curStep->getAction()->LBDurration());
			row++;
			k++;
		}
	}

	for(vector<Ordering*>::iterator it = mOrderings.begin();
			it != mOrderings.end(); it++) {
		Ordering* curOrdering = *it;
		//Post_time - Pre_time - s_k = epsilon
		mA->set(row, getStepCol(curOrdering->Post(), curOrdering->PostTime()), mgr.addOne());
		mA->set(row, getStepCol(curOrdering->Pre(), curOrdering->PreTime()), -mgr.addOne());
		mA->set(row, getSlackVar(k), -mgr.addOne());
		(*mb)[row] = mgr.constant(epsilon);
		row++;
		k++;
	}
}
