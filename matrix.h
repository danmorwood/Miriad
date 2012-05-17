#include "cuddObj.hh"
#include <map>
#include <vector>

extern Cudd mgr;
typedef ADD valueType;
#define BACKGROUND mgr.addZero()

class SparceMatrix {
	public:
		SparceMatrix(unsigned int n, unsigned int m);
		void set(unsigned int, unsigned int, valueType);
		valueType get(unsigned int, unsigned int);
		void swapRow(unsigned int r1, unsigned int r2);
		void swapColumn(unsigned int c1, unsigned int c2);
		void swapEntryInColumn(unsigned int col, unsigned int r1, ADD r2);

		//Subtract r2 from r1 and store result in r1
		void Subtract(unsigned int r1, unsigned int r2);
		//Subtract factor*r2 from r1 and store result in r1.
		void SubtractMultiple(unsigned int r1, valueType factor, unsigned int r2);
		void Multiply(unsigned int row, valueType value);
		void rref();
		void print();
		ADDvector LUPDecomposition();
		ADDvector Solve(ADDvector pi, ADDvector b);
	private:
		std::vector< std::map<unsigned int, valueType> > mData;
		int sizeN;
		int sizeM;
};


