#include "plan.h"

Link::Link(Step* pre, Step* post, Literal* literal) {
	mPre = pre;
	mPost = post;
	mLiteral = literal;
}

Step::Step(Action* action) {
	mAction = action;
}

Plan::Plan(Domain* domain) {
	mDomain = domain;
}
