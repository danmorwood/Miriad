#include <cuddObj.hh>
#include <map>
#include <vector>

#ifndef MATRIX_H
#define MATRIX_H

extern Cudd mgr;
typedef ADD valueType;
#define BACKGROUND mgr.addZero()

class SparceMatrix {
	public:
		SparceMatrix();
		SparceMatrix(unsigned int n, unsigned int m);
		void resize(unsigned int n, unsigned int m);
		void set(unsigned int, unsigned int, valueType);
		valueType get(unsigned int, unsigned int) const;
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
		void insertWithOffset(SparceMatrix super, int rOffset, int cOffset) const;
		SparceMatrix Transpose();
		ADDvector LUPDecomposition();
		ADDvector Solve(ADDvector pi, ADDvector b);
		SparceMatrix operator-() const;
		SparceMatrix operator*(const SparceMatrix& rhs) const;
		ADDvector operator*(const ADDvector& rhs) const;

	private:
		std::vector< std::map<unsigned int, valueType> > mData;
		int sizeN;
		int sizeM;
};

SparceMatrix diag(ADDvector v);

ADDvector operator+(const ADDvector& lhs, const ADDvector& rhs);
ADDvector operator-(const ADDvector& lhs, const ADDvector& rhs);
ADDvector operator-(const ADDvector& rhs);
ADD dotProduct(ADDvector v1, ADDvector v2);
ADDvector ConstVector(double value, int size);
#endif //MATRIX_H
