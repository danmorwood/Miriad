#include <cstdlib>
#include <cstdio>
#include "domain.h"

using namespace std;


Literal::Literal(string name) {
	mName = name;
}

string Literal::getName() {
	return mName;
}

Effect::Effect(Literal* literal, EffectType type, bool known = true) {
	mLiteral = literal;
	mType = type;
	mKnown = known;
}

Precondition::Precondition(Literal* literal, bool value = true, bool known = true) {
	mLiteral = literal;
	mValue = value;
	mKnown = known;
}

Action::Action(string name) {
	mName = name;
}

void Action::AddEffect(Effect* effect) {
	mEffects.push_back(effect);
}

void Action::AddPrecondition(Precondition* precondition) {
	mPreconditions.push_back(precondition);
}

void Action::setUpperBoundOnDurration(double ub) {
	mUBDurration = ub;
}

void Action::setLowerBoundOnDurration(double lb) {
	mLBDurration = lb;
}

double Action::UBDurration() {
	return mUBDurration;
}

double Action::LBDurration() {
	return mLBDurration;
}

void Domain::LoadTestDomain() {
	//Literals
	Literal* P = new Literal("P");
	mLiteralDirectory["P"] = P;
	Literal* Q = new Literal("Q");
	mLiteralDirectory["Q"] = Q;

	//Actions
	Action* A1 = new Action("A1");
	A1->setLowerBoundOnDurration(3);
	A1->setUpperBoundOnDurration(5);
	A1->AddPrecondition(new Precondition(P, true, true));
	A1->AddEffect(new Effect(Q, AddEffect, true));
	mActionDirectory["A1"] = A1;

	//Initial State
	InitialState(new Effect(P, AddEffect, true));

	//Goal
	AddGoal(new Precondition(Q, true, true));

	//Deadline
	mDeadline = 4;
}

Domain::Domain() {
	LoadTestDomain();
}

Domain::Domain(const char* filename) {
	//Dang I wish this was implemented...
}

void Domain::InitialState(Effect* initialState) {
	mInitialState.push_back(initialState);
}

void Domain::AddGoal(Precondition* goal) {
	mGoals.push_back(goal);
}

Literal* Domain::LookupLiteral(string name) {
	if(mLiteralDirectory.find(name) == mLiteralDirectory.end()) {
		mLiteralDirectory[name] = new Literal(name);
		printf("created literal: %s\n", name.c_str());
	}
	return mLiteralDirectory[name];
}

Action* Domain::LookupAction(string name) {
	if(mActionDirectory.find(name) == mActionDirectory.end()) {
		mActionDirectory[name] = new Action(name);
		printf("created action: %s\n", name.c_str());
	}
	return mActionDirectory[name];
}
