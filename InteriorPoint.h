#include "cuddObj.hh"
#include "matrix.h"
#include "plan.h"
#include <vector>

double evaluate(const Plan& p);

class OptimizationProblem {
	public:
		SparceMatrix A;
		std::vector<Function> f;
		std::vector<Function> df;
		void solve();
		ADDvector x;
		ADDvector lambda;
		ADDvector nu;
};
