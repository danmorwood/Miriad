#include "../InteriorPoint.h"

Domain* CreateTestDomain() {
}

Plan CreateTestPlan(Domain* domain) {
}

int main() {
	Domain* domain = CreateTestDomain();
	Plan plan = CreateTestPlan(domain);
	double value = plan.evaluate();
	printf("The value = %lf\n", value);
	return 0;
}

