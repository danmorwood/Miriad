#include "domain.h"

class Step;

class Link {
	public:
		Link(Step* pre, Step* post, Literal* literal);
	private:
		Step* mPre;
		Step* mPost;
		Literal* mLiteral;
};

class Step {
	public:
		Step(Action* action);
	private:
		Action* mAction;
		std::vector<Link*> mSupport;
};

class Ordering {
	public:
		Ordering(Step* pre, Step* post);
	private:
		Step* mPre;
		Step* mPost;
};

class Plan {
	public:
		Plan(Domain* domain);
	private:
		Domain* mDomain;
		std::vector<Step*> mSteps;
		std::vector<Link*> mLinks;
		std::vector<Ordering*> mOrderings;
};
