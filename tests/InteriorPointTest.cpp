#include "../InteriorPoint.h"

Domain* CreateTestDomain() {
	return new Domain();
}

Plan CreateTestPlan(Domain* domain) {
	Plan p(domain);
	return p;
}

int main() {
	Domain* domain = CreateTestDomain();
	Plan plan = CreateTestPlan(domain);
	double value = evaluate(plan);
	printf("The value = %lf\n", value);
	return 0;
}

