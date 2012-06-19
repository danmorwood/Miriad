#include "matrix.h"
#include "domain.h"

class Step;

const double epsilon = .0001;

namespace StepTime {
	enum Time {
		Start,
		End,
	};
}

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
		Action* getAction();
	private:
		Action* mAction;
		std::vector<Link*> mSupport;
};

class Ordering {
	public:
		Ordering(Step* pre, StepTime::Time preTime, Step* post, StepTime::Time postTime);
		Step* Pre();
		Step* Post();
		StepTime::Time PreTime();
		StepTime::Time PostTime();
	private:
		Step* mPre;
		Step* mPost;
		StepTime::Time mPreTime;
		StepTime::Time mPostTime;
};

struct Function {
	int index;
	bool inverted;
};

class Plan {
	public:
		Plan(Domain* domain);
		int getNumSteps();
		int getNumStepsWithUnknownDurration();
		int getNumSlackVars();
		int getNumFunctions();
		void CalcEncoding();

		int getSlackVar(int i);
		int getStepCol(Step* step, StepTime::Time time);

		void addStep(Action* action);
		void addOrdering(Ordering* order);
		void addLink(Link* link);
		void setPlanDeadline(double deadline);

		SparceMatrix* A();
		ADDvector F(ADDvector x, bool phase1);
		SparceMatrix DF_x(bool phase1);
		ADDvector DeltaF_0(ADDvector x, bool phase1);
		ADDvector getInitalX();

	private:
		unsigned int numSlackVars;
		unsigned int numUnknownDurations;
		Domain* mDomain;
		std::vector<Step*> mSteps;
		std::vector<Link*> mLinks;
		std::vector<Ordering*> mOrderings;
		//the step representing the initial and goal dummy steps
		Step* InitGoal;
		
		//Used for encoding plan
		SparceMatrix* mA;
		ADDvector* mb;
		std::vector<Function> mf;
		int numOfUnknownDurations;
};
