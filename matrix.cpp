#include "matrix.h" 

using namespace std;


template <class T>
T& SparceArray<T>::operator[](int index) {
	if(this == NULL)
		return NULL;
	typename map<int, T>::iterator it = mData.find(index);
	if(it == mData.end())
		return NULL;
	else 
		return it.second;
}

template <class T>
SparceArray<T>& SparceMatrix<T>::operator[](int index) {
	return mData[index];
}
