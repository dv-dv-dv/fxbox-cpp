#pragma once
#include <iostream>
#define SAFE
namespace a2d { 
	template <typename T>
	class Array2D {
	protected:
		bool _dynamic = true;
		int _length, _cols, _rows, _vrows, _vrows1, _vrows2, _vcols, _vcols1, _vcols2;
		T* ptr = NULL;
	public: 
		const int& length = _length;
		const int& cols = _cols;
		const int& rows = _rows;
		const int& vrows = _vrows;
		const int& vrows1 = _vrows1;
		const int& vrows2 = _vrows2;
		const int& vcols = _vcols;
		const int& vcols1 = _vcols1;
		const int& vcols2 = _vcols2;
		template<typename U>
		Array2D<T>& copy(const Array2D<U>& test) {
			if (&test == this) return *this;
			if (_length != test.length) {
				if (!_dynamic) { std::cout << "\nArray2D copy error: cannot resize nondynamic array\n"; throw std::runtime_error(""); }
				delete[] ptr;
				ptr = new T[test.rows * test.cols];
				_length = test.rows * test.cols;
			}
			_rows = test.rows;
			_cols = test.cols;
			_vrows1 = test.vrows1;
			_vrows2 = test.vrows2;
			_vcols1 = test.vcols1;
			_vcols2 = test.vcols2;
			_vrows = test.vrows;
			_vcols = test.vcols;
			acopy(*this, test);
			return *this;
		}
		Array2D<T>& vpart(int vrows1 = 0, int vrows2 = -1, int vcols1 = 0, int vcols2 = -1) {
			if (vrows2 == -1) vrows2 = _rows;
			if (vcols2 == -1) vcols2 = _cols;

#if defined(SAFE)
			if (!a2d_check_vpart(*this, vrows1, vrows2, vcols1, vcols2)) { throw std::runtime_error(""); }
#endif
			_vrows1 = vrows1;
			_vrows2 = vrows2;
			_vcols1 = vcols1;
			_vcols2 = vcols2;
			_vrows = vrows2 - vrows1;
			_vcols = vcols2 - vcols1;
			return *this;
		}
		Array2D<T>& vreset() {
			_vrows1 = _vcols1 = 0;
			_vrows = _vrows2 = _rows; 
			_vcols = _vcols2 = _cols;
			_length = _rows * _cols;
			return *this;
		}
		T& operator()(int index1 = 0, int index2 = 0) {
			if (index1 == -1) { index1 = _rows - 1; }
			if (index2 == -1) { index2 = _cols - 1; }
#if defined(SAFE)
			if (!a2d_check_index(*this, index1, index2)) { throw std::runtime_error(""); }
#endif
			return ptr[index1 * _cols + index2];
		} 
		const T& operator()(int index1 = 0, int index2 = 0) const { 
			if (index1 == -1) { index1 = _rows - 1; }
			if (index2 == -1) { index2 = _cols - 1; }
#if defined(SAFE)
			if (!a2d_check_index(*this, index1, index2)) { throw std::runtime_error(""); }
#endif
			return ptr[index1 * _cols + index2]; 
		}
	};
	// Static array
	template<typename T, int R, int C = 1>
	class Array2Ds : public Array2D<T> {
	private:
		T pptr[R * C];
	public:
		Array2Ds() {
			this->_cols = C;
			this->_rows = R;
			this->_dynamic = false;
			this->ptr = pptr;
			this->vreset();
		}
	};
	// Dynamic array
	template<typename T>
	class Array2Dy : public Array2D<T> {
	public:
		Array2Dy() {
			this->_cols = 0;
			this->_rows = 0;
			this->ptr = NULL;
			this->vreset();
		}
		Array2Dy(int R, int C = 1) {
			this->_cols = C;
			this->_rows = R;
			this->ptr = new T[R * C]();
			this->vreset();
		}
		template <typename U>
		Array2Dy(const Array2D<U>* test) {
			this->copy(*test);
		}
		Array2D<T>& resize(int R, int C = 1) {
			delete[] this->ptr;
			this->ptr = new T[R * C]();
			this->_rows = R;
			this->_cols = C;
			this->vreset();
			return *this;
		}
		~Array2Dy() {
			delete[] this->ptr;
		}
	};
	// safety checks
	template <typename T>
	bool a2d_check_vpart(const Array2D<T>& test1, int vrows1, int vrows2, int vcols1, int vcols2) {
		bool passed = true;
		if (vrows1 > vrows2) {
			std::cout << "a2d_check_vpart error: vrows1 > vrows2\n";
			passed = false;
		}
		if (vcols1 > vcols2) {
			std::cout << "a2d_check_vpart error: vcols1 > vcols2\n";
			passed = false;
		}
		if (vrows1 < 0) {
			std::cout << "a2d_check_vpart error: vrows1 < 0\n";
			passed = false;
		}
		if (vrows1 > test1.rows) {
			std::cout << "a2d_check_vpart error: vrows1 > rows\n";
			passed = false;
		}
		if (vrows2 < 0) {
			std::cout << "a2d_check_vpart error: vrows2 < 0\n";
			passed = false;
		}
		if (vrows2 > test1.rows) {
			std::cout << "a2d_check_vpart error: vrows2 > rows\n";
			passed = false;
		}
		if (vcols1 < 0) {
			std::cout << "a2d_check_vpart error: vcols1 < 0\n";
			passed = false;
		}
		if (vcols1 > test1.cols) {
			std::cout << "a2d_check_vpart error: vcols1 > cols\n";
			passed = false;
		}
		if (vcols2 < 0) {
			std::cout << "a2d_check_vpart error: vcols2 < 0\n";
			passed = false;
		}
		if (vcols2 > test1.cols) {
			std::cout << "a2d_check_vpart error: vcols2 > cols\n";
			passed = false;
		}
		return passed;
	}
	template <typename T>
	bool a2d_check_dims(const Array2D<T>& test1, const Array2D<T>& test2, bool vpart = false) {
		int t1rows, t2rows, t1cols, t2cols;
		if (vpart) {
			t1rows = test1.vrows, t2rows = test2.vrows, t1cols = test1.vcols, t2cols = test2.vcols;
		}
		else {
			t1rows = test1.rows, t2rows = test2.rows, t1cols = test1.cols, t2cols = test2.cols;
		}
		bool passed = true;
		if (t1rows != t2rows) {
			std::cout << "a2d_check_dims row mismatch\n";
			passed = false;
		}
		if (t1cols != t2cols) {
			std::cout << "a2d_check_dims cols mismatch\n";
			passed = false;
		}
		if (!passed && vpart) {
			std::cout << "vparted\n";
		}
		return passed;
	}
	template <typename T>
	bool a2d_check_index(const Array2D<T>& test1, int index1, int index2) {
		bool passed = true;
		if (index1 >= test1.rows) {
			std::cout << "a2d_check_index index1 exceeds number of rows\n";
			passed = false;
		}
		if (index1 < 0) {
			std::cout << "a2d_check_index index1 less than zero\n";
			passed = false;
		}
		if (index2 >= test1.cols) {
			std::cout << "a2d_check_index index2 exceeds number of cols\n";
			passed = false;
		}
		if (index2 < 0) {
			std::cout << "a2d_check_index index2 less than zero\n";
			passed = false;
		}
		return passed;
	}
	// misc utilities
	template <typename T>
	int aprint(const Array2D<T>& test) {
		for (auto i = 0; i < test.rows; i++) {
			for (auto j = 0; j < test.cols; j++) {
				std::cout << test(i, j) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		return 0;
	}
	template <typename T>
	int atest(const Array2D<T>& test, int t) {
		int testr = t, testc = t;
		if (t > test.rows) { testr = test.rows; }
		if (t > test.cols) { testc = test.cols; }
		for (auto i = 0; i < testr; i++) {
			for (auto j = 0; j < testc; j++) {
				std::cout << test(i, j) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		std::cout << "\n";
		return 0;
	}
	template <typename T>
	Array2D<T>& arange(Array2D<T>& test) {
		for (auto i = test.vrows1; i < test.vrows2; i++) {
			for (auto j = test.vcols1; j < test.vcols2; j++) {
				test(i, j) = j - test.vcols1;
			}
		}
		return test;
	}
	template <typename T, typename U>
	Array2D<T>& acopy(Array2D<T>& test1, const Array2D<U>& test2) {
#if defined(SAFE)
		if (!a2d_check_dims(test1, test2, false)) { throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.rows; i++) {
			for (auto j = 0; j < test1.cols; j++) {
				test1(i, j) = test2(i, j);
			}
		}
		return test1;
	}
	template <typename T>
	T asum(Array2D<T>& test1) {
		T sum = 0;
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols; j++) {
				sum += test1(i + test1.vrows1, j + test1.vcols1);
			}
		}
		return sum;
	}
	// array constant operations
	template <typename T, typename U = T>
	Array2D<T>& kmult(Array2D<T>& test, U k) {
		for (auto i = test.vrows1; i < test.vrows2; i++) {
			for (auto j = test.vcols1; j < test.vcols2; j++) {
				test(i, j) *= k;
			}
		}
		return test;
	}
	template <typename T, typename U = T>
	Array2D<T>& kadd(Array2D<T>& test, U k) {
		for (auto i = test.vrows1; i < test.vrows2; i++) {
			for (auto j = test.vcols1; j < test.vcols2; j++) {
				test(i, j) += k;
			}
		}
		return test;
	}
	template <typename T, typename U>
	Array2D<T>& kset(Array2D<T>& test, U k) {
		for (auto i = test.vrows1; i < test.vrows2; i++) {
			for (auto j = test.vcols1; j < test.vcols2; j++) {
				test(i, j) = k;
			}
		}
		return test;
	}
	// array array operations
	template <typename T, typename U, typename V = T>
	Array2D<T>& aset(Array2D<T>& test1, const Array2D<U>& test2, V k=1) {
#if defined(SAFE)
		if (!a2d_check_dims(test1, test2, true)) { throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols; j++) {
				test1(i + test1.vrows1, j + test1.vcols1) = k*test2(i + test2.vrows1, j + test2.vcols1);
			}
		}
		return test1;
	}
	template <typename T, typename U, typename V = T>
	Array2D<T>& amult(Array2D<T>& test1, const Array2D<U>& test2, V k=1) {
#if defined(SAFE)
		if (!a2d_check_dims(test1, test2, true)) { throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols; j++) {
				test1(i + test1.vrows1, j + test1.vcols1) *= k*test2(i + test2.vrows1, j + test2.vcols1);
			}
		}
		return test1;
	}
	template <typename T, typename U, typename V = T>
	Array2D<T>& aadd(Array2D<T>& test1, const Array2D<U>& test2, V k=1) {
#if defined(SAFE)
		if (!a2d_check_dims(test1, test2, true)) { throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols; j++) {
				test1(i + test1.vrows1, j + test1.vcols1) += k*test2(i + test2.vrows1, j + test2.vcols1);
			}
		}
		return test1;
	}
}