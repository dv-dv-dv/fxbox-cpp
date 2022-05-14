#pragma once
#include <iostream>
#define SAFE
namespace a2d {
	template <typename T>
	class Array2D {
	protected:
		bool dynamic = true;
		int length, cols, rows, vrows, vrows1, vrows2, vcols, vcols1, vcols2;
	public:
		T* ptr = NULL;
		const int& l = length;
		const int& c = cols;
		const int& r = rows;
		const int& vr = vrows;
		const int& vr1 = vrows1;
		const int& vr2 = vrows2;
		const int& vc = vcols;
		const int& vc1 = vcols1;
		const int& vc2 = vcols2;
		template<typename U>
		Array2D<T>& copy(const Array2D<U>& test) {
			if (&test == this) return *this;
			if (length != test.l) {
				if (!dynamic) { std::cout << "\nArray2D copy error: cannot resize nondynamic array\n"; throw std::runtime_error(""); }
				delete[] ptr;
				ptr = new T[test.r * test.c];
				length = test.r * test.c;
			}
			rows = test.r;
			cols = test.c;
			vrows1 = test.vr1;
			vrows2 = test.vr2;
			vcols1 = test.vc1;
			vcols2 = test.vc2;
			vrows = test.vr;
			vcols = test.vc;
			acopy(*this, test);
			return *this;
		}
		Array2D<T>& vpart(int avrows1 = 0, int avrows2 = -1, int avcols1 = 0, int avcols2 = -1) {
			if (avrows2 == -1) avrows2 = rows;
			if (avcols2 == -1) avcols2 = cols;

			#if defined(SAFE)
			if (avrows1 < 0) { std::cout << "\nerror in a2d vpart, vrows1 is less than 0\n"; throw std::runtime_error(""); }
			else if (avrows1 > rows) { std::cout << "\nerror in a2d vpart, vrows1 is greater than rows\n"; throw std::runtime_error(""); }
			else if (avrows2 < 0) { std::cout << "\nerror in a2d vpart, vrows2 is less than 0\n"; throw std::runtime_error(""); }
			else if (avrows2 > rows) { std::cout << "\nerror in a2d vpart, vrows2 is greater than rows\n"; throw std::runtime_error(""); }
			else if (avcols1 < 0) { std::cout << "\nerror in a2d vpart, vcols1 is less than 0\n"; throw std::runtime_error(""); }
			else if (avcols1 > cols) { std::cout << "\nerror in a2d vpart, vcols1 is greater than cols\n"; throw std::runtime_error(""); }
			else if (avcols2 < 0) { std::cout << "\nerror in a2d vpart, vcols2 is less than zero\n"; throw std::runtime_error(""); }
			else if (avcols2 > cols) { std::cout << "\nerror in a2d vpart, vcols 2 is greater than cols\n"; throw std::runtime_error(""); }
			#endif

			vrows1 = avrows1;
			vrows2 = avrows2;
			vcols1 = avcols1;
			vcols2 = avcols2;
			vrows = vr2 - vr1;
			vcols = vc2 - vc1;
			return *this;
		}
		Array2D<T>& vreset() {
			vrows1 = vcols1 = 0;
			vrows = vrows2 = rows;
			vcols = vcols2 = cols;
			length = rows * cols;
			return *this;
		}
		T& operator()(int index1, int index2) {
			if (index1 == -1) { index1 = rows - 1; }
			if (index2 == -1) { index2 = cols - 1; }
#if defined(SAFE)
			if (index1 >= this->rows) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index2 >= this->cols) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index1 < 0) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index2 < 0) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
#endif
			return ptr[index1 * cols + index2];
		}
		T& operator()(int index) {
			if (index == -1) { index = length - 1; }
#if defined(SAFE)
			if (index >= this->length) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index < 0) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
#endif
			return ptr[index];
		}
		const T& operator()(int index1, int index2) const {
			if (index1 == -1) { index1 = rows - 1; }
			if (index2 == -1) { index2 = cols - 1; }
#if defined(SAFE)
			if (index1 >= this->rows) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index2 >= this->cols) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index1 < 0) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index2 < 0) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
#endif
			return ptr[index1 * cols + index2];
		}
		const T& operator()(int index) const {
			if (index == -1) { index = length - 1; }
#if defined(SAFE)
			if (index >= this->length) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
			else if (index < 0) { std::cout << "\na2d indexing error: out of bounds\n"; throw std::runtime_error("");
			}
#endif
			return ptr[index];
		}
	};
	class vpart{
	
	};
	// Static array
	template<typename T, int R, int C = 1>
	class Array2Ds : public Array2D<T> {
	private:
		T pptr[R * C];
	public:
		Array2Ds() {
			this->cols = C;
			this->rows = R;
			this->dynamic = false;
			this->ptr = pptr;
			this->vreset();
		}
	};
	// Dynamic array
	template<typename T>
	class Array2Dy : public Array2D<T> {
	public:
		Array2Dy() {
			this->cols = 0;
			this->rows = 0;
			this->ptr = NULL;
			this->vreset();
		}
		Array2Dy(int R, int C = 1) {
			this->cols = C;
			this->rows = R;
			this->ptr = new T[R * C]();
			this->vreset();
		}
		template <typename U>
		Array2Dy(const Array2D<U>& test) {
			this->cols = test.c;
			this->rows = test.r;
			this->ptr = new T[test.c * test.r]();
			this->copy(test);
		}
		Array2D<T>& resize(int R, int C = 1) {
			delete[] this->ptr;
			this->ptr = new T[R * C]();
			this->rows = R;
			this->cols = C;
			this->vreset();
			return *this;
		}
		~Array2Dy() {
			delete[] this->ptr;
		}
	};
	template <typename T>
	int pprint(const Array2D<T>& test) {
		for (auto i = test.vr1; i < test.vr2; i++) {
			for (auto j = test.vc1; j < test.vc2; j++) {
				std::cout << test(i, j) << " ";
			}
			std::cout << "\n";
		}
		std::cout << "\n";
		return 0;
	}
	template <typename T>
	int aprint(const Array2D<T>& test) {
		for (auto i = 0; i < test.r; i++) {
			for (auto j = 0; j < test.c; j++) {
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
		if (t > test.r) { testr = test.r; }
		if (t > test.c) { testc = test.c; }
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
		for (auto i = test.vr1; i < test.vr2; i++) {
			for (auto j = test.vc1; j < test.vc2; j++) {
				test(i, j) = j - test.vc1;
			}
		}
		return test;
	}
	template <typename T, typename U = T>
	Array2D<T>& kmult(Array2D<T>& test, U k) {
		for (auto i = test.vr1; i < test.vr2; i++) {
			for (auto j = test.vc1; j < test.vc2; j++) {
				test(i, j) *= k;
			}
		}
		return test;
	}
	template <typename T, typename U = T>
	Array2D<T>& kadd(Array2D<T>& test, U k) {
		for (auto i = test.vr1; i < test.vr2; i++) {
			for (auto j = test.vc1; j < test.vc2; j++) {
				test(i, j) += k;
			}
		}
		return test;
	}
	template <typename T, typename U>
	Array2D<T>& kset(Array2D<T>& test, U k) {
		for (auto i = test.vr1; i < test.vr2; i++) {
			for (auto j = test.vc1; j < test.vc2; j++) {
				test(i, j) = k;
			}
		}
		return test;
	}
	template <typename T, typename U>
	Array2D<T>& acopy(Array2D<T>& test1, const Array2D<U>& test2) {
		#if defined(SAFE)
		if (test1.r != test2.r) { std::cout << "\na2d aset error: row/cols mismatch\n"; throw std::runtime_error(""); }
		if (test1.c != test2.c) { std::cout << "\na2d aset error: row/cols mismatch\n"; throw std::runtime_error(""); }
		#endif
		for (auto i = 0; i < test1.r; i++) {
			for (auto j = 0; j < test1.c; j++) {
				test1(i, j) = test2(i, j);
			}
		}
		return test1;
	}
	template <typename T, typename U, typename V = T>
	Array2D<T>& aset(Array2D<T>& test1, const Array2D<U>& test2, V k=1) {
		#if defined(SAFE)
		if (test1.vr != test2.vr) { std::cout << "\na2d aset error: row/cols mismatch\n"; throw std::runtime_error(""); }
		if (test1.vc != test2.vc) { std::cout << "\na2d aset error: row/cols mismatch\n"; throw std::runtime_error(""); }
		#endif
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc; j++) {
				test1(i + test1.vr1, j + test1.vc1) = k*test2(i + test2.vr1, j + test2.vc1);
			}
		}
		return test1;
	}
	template <typename T>
	T asum(Array2D<T>& test1) {
		T sum = 0;
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc; j++) {
				sum += test1(i + test1.vr1, j + test1.vc1);
			}
		}
		return sum;
	}
	template <typename T, typename U, typename V = T>
	Array2D<T>& amult(Array2D<T>& test1, const Array2D<U>& test2, V k=1) {
		#if defined(SAFE)
		if (test1.vr != test2.vr) { std::cout << "\na2d amult error: row/cols mismatch\n"; throw std::runtime_error(""); }
		if (test1.vc != test2.vc) { std::cout << "\na2d amult error: row/cols mismatch\n"; throw std::runtime_error(""); }
		#endif
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc; j++) {
				test1(i + test1.vr1, j + test1.vc1) *= k*test2(i + test2.vr1, j + test2.vc1);
			}
		}
		return test1;
	}
	template <typename T, typename U, typename V = T>
	Array2D<T>& aadd(Array2D<T>& test1, const Array2D<U>& test2, V k=1) {
		#if defined(SAFE)
		if (test1.vr != test2.vr) { std::cout << "\na2d aadd error: row/cols mismatch\n"; throw std::runtime_error(""); }
		if (test1.vc != test2.vc) { std::cout << "\na2d aadd error: row/cols mismatch\n"; throw std::runtime_error(""); }
		#endif
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc; j++) {
				test1(i + test1.vr1, j + test1.vc1) += k*test2(i + test2.vr1, j + test2.vc1);
			}
		}
		return test1;
	}
}