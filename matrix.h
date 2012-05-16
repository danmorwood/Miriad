#include <map>
#include <vector>

typedef double valueType;
#define BACKGROUND 0

class SparceMatrix {
	public:
		SparceMatrix(unsigned int n, unsigned int m);
		void set(unsigned int, unsigned int, valueType);
		valueType get(unsigned int, unsigned int);
		void swapRow(unsigned int r1, unsigned int r2);
		void swapColumn(unsigned int c1, unsigned int c2);

		//Subtract r2 from r1 and store result in r1
		void Subtract(unsigned int r1, unsigned int r2);
		void Multipy(unsigned int row, valueType value);
		void rref();
		void print();
	private:
		std::vector< std::map<unsigned int, valueType> > mData;
		int sizeN;
		int sizeM;
};


