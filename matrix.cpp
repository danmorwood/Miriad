#include "matrix.h" 
#include <cstdio>

using namespace std;

SparceMatrix::SparceMatrix(unsigned int n, unsigned int m) {
	sizeN = n;
	mData.resize(n);
	sizeM = m;
}

void SparceMatrix::set(unsigned int x, unsigned int y, valueType value) {
	if(x >= sizeN || y >= sizeM) {
		printf("Error: array out of bounds... requested [%d, %d] in matrix of size [%d, %d]\n",
				x, y, sizeN, sizeM);
		return;
	}
	if(value == BACKGROUND)
		mData[x].erase(y);
	else
		mData[x][y] = value;
}

valueType SparceMatrix::get(unsigned int x, unsigned int y) {
	if(x >= sizeN || y >= sizeM) {
		printf("Error: array out of bounds... requested [%d, %d] in matrix of size [%d, %d]\n",
				x, y, sizeN, sizeM);
		return BACKGROUND;
	}
	map<unsigned int, valueType>::iterator it = mData[x].find(y);
	if(it == mData[x].end())
		return BACKGROUND;
	else
		return it->second;
}

void SparceMatrix::swapRow(unsigned int r1, unsigned int r2) {
	if(r1 >= sizeN || r2 >= sizeN) {
		printf("Error: Requested rows %d and %d swaped in matrix of size [%d, %d]\n",
				r1, r2, sizeN, sizeM);
		return;
	}
	mData[r1].swap(mData[r2]);
}

void SparceMatrix::swapColumn(unsigned int c1, unsigned int c2) {
	if(c1 >= sizeM || c2 >= sizeM) {
		printf("Error: Requested columns %d and %d swaped in matrix of size [%d, %d]\n",
				c1, c2, sizeN, sizeM);
		return;
	}
	for(int i = 0; i < sizeN; i++) {
		map<unsigned int, valueType>::iterator c1it = mData[i].find(c1);
		map<unsigned int, valueType>::iterator c2it = mData[i].find(c2);
		if(c1it != mData[i].end()) {
			valueType temp = c1it->second;
			if(c2it != mData[i].end()) {
				mData[i][c1] = c2it->second;
			} else {
				mData[i].erase(c1it);
			}
			mData[i][c2] = temp;
		} else {
			if(c2it != mData[i].end()) {
				mData[i][c1] = c2it->second;
				mData[i].erase(c2it);
			}
		}
	}
}

void SparceMatrix::Subtract(unsigned int r1, unsigned int r2) {
	map<unsigned int, valueType>::iterator r1it = mData[r1].begin();
	map<unsigned int, valueType>::iterator r2it = mData[r2].begin();
	while (r1it != mData[r1].end() && r2it != mData[r2].end()) {
		if(r1it->first < r2it->first) {
			r1it++;
		} else if( r2it->first < r1it->first) {
			mData[r1][r2it->first] = -r2it->second;
			r2it++;
		} else {
			mData[r1][r1it->first] = r1it->second - r2it->second;
			r1it++;
			r2it++;
		}
	}
	while(r2it != mData[r2].end()) {
		mData[r1][r2it->first] = -r2it->second;
		r2it++;
	}
}

void SparceMatrix::Multipy(unsigned int row, valueType value) {
	for(map<unsigned int, valueType>::iterator it = mData[row].begin();
			it != mData[row].end(); it++) {
		it->second *= value;
	}
}

void SparceMatrix::rref() {
	if(sizeN != sizeM - 1) {
		printf("rref(): Im not sure what to do with that...\n");
		return;
	}
	for(int i = 0; i < sizeN; i++) {
		for(int j = i+1; j < sizeN; j++) {
			double factor = mData[i][i] / mData[j][i];
			Multipy(j, factor);
			Subtract(j, i);
		}
		Multipy(i, 1/mData[i][i]);
	}
	for(int i = sizeN -1; i >= 0; i--) {
		for(int j = i-1; j >= 0; j--) {
			double factor = mData[j][i];
			Multipy(i, factor);
			Subtract(j, i);
			Multipy(i, 1/factor);
		}
	}
}

void SparceMatrix::print() {
	for(int i = 0; i < sizeN; i++) {
		for(int j = 0; j < sizeM; j++) {
			printf("%2.1f ", get(i, j));
		}
		printf("\n");
	}
}

SparceMatrix SparceMatrix::LUDecomposition() {
	SparceMatrix ret(sizeN, sizeM);
	for(int k = 0; k < sizeN; k++) {
		ret.set(k,k,get(k,k));
		for(int i = k+1; i < sizeN; i++) {
			ret.set(i, k, get(i, k)/get(k, k));
			ret.set(k, i, get(k, i));
		}
		for(int i = k+1; i < sizeN; i++) {
			for(int j = k+1; j < sizeN; j++) {
				set(i,j,get(i,j)-ret.get(i,k)*ret.get(k,j));
			}
		}
	}
}

SparceMatrix SparceMatrix::LUPDecomposition() {
	vector<int> pi(sizeN);
	for(int i = 0; i < sizeN; i++) {
		pi[i] = i;
	}
	for(int k = 0; k < sizeN; k++) {
		int kPrime = k;
		valueType p = BACKGROUND;
		for(int i = k; i < sizeN; i++) {
			if(abs(get(i,k)) > p) {
				p = abd(get(i,k));
				kPrime = i;
			}
		}
		if(p == 0) {
			printf("Error Singular Matrix?!?\n");
		}
		swap(pi[k], pi[kPrime]);
		swapRow(k, kPrime);
		for(i = k+1; i < sizeN; i++) {
			set(i, k, get(i,k)/get(k,k));
			for(j = k+1; k < n; k++) {
				set(i,j,get(i,j)-get(i,k)*get(k,j));
			}
		}
	}
	return pi;
}
