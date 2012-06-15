#include "../matrix.h"


ADD split(ADD var, int val1, int val2) {
	return var * mgr.constant(val1) + var.Cmpl()*mgr.constant(val2);
}

int main() {
	ADD var = mgr.addVar();

	SparceMatrix eq(3,3);
	//SparceMatrix eq(2,2);
	eq.set(0,0,mgr.constant(1));
	eq.set(0,1,mgr.constant(1));
	eq.set(0,2,mgr.constant(1));

	eq.set(1,0,mgr.constant(5));
	eq.set(1,1,split(var,2,1));
	//eq.set(1,1,mgr.constant(2));
	eq.set(1,2,mgr.constant(7));

	eq.set(2,0,split(var,2,3));
	//eq.set(2,0,mgr.constant(2));
	eq.set(2,1,mgr.constant(5));
	eq.set(2,2,mgr.constant(1));

	ADDvector b(3,&mgr);
	b[0] = split(var, 6, 7);
	b[1] = split(var, 30, 35);
	b[2] = split(var, 15, 17);
	//b[0] = mgr.constant(6);
	//b[1] = mgr.constant(30);
	//b[2] = mgr.constant(15);

	ADDvector pi = eq.LUPDecomposition();
	ADDvector solution = eq.Solve(pi, b);
	//solution.DumpDot();
	pi.DumpDot();
	solution.DumpDot();
}
