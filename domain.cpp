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

Domain::Domain() {
}

Domain::Domain(const char* filename) {

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
