#include <map>
#include <vector>

template <class T>
class SparceArray {
	public:
		T& operator[](int);
	private:
		std::map<int, T> mData;
};

template <class T>
class SparceMatrix {
	public:
		SparceArray<T>& operator[](int index);
	private:
//		map<int, SparceArray<T> > mData;
		std::vector<SparceArray<T> > mData;
};
