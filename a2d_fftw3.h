#pragma once
#include <iostream>
#include <fftw3.h>
#include "a2d.h"
#define a2d_fftw3_SAFE
namespace a2d_fft_f {
	// forward rfft will go into complex, inverse rfft will go into real
	//
	template <typename T = float>
	class a2d_rfft {
	private:
		bool destroyed = false;
		int real_r, real_c, complex_r, complex_c;
	public:
		float n;
		int axis;
		fftwf_plan fwd;
		a2d::Array2D<float>* real;
		a2d::Array2D<float>* complex;
		a2d_rfft() {
			fwd = NULL;
			real = NULL;
			complex = NULL;
			real_r = real_c = complex_r = complex_c = 0;
			n = 0.0;
		}
		void plan(a2d::Array2D<float>& areal, a2d::Array2D<float>& acomplex, int aaxis = 0) {
			const unsigned int fftw_flag = FFTW_MEASURE;
			if (aaxis == 0) {
				if (acomplex.rows != (areal.rows / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << ", real.r: " << areal.rows << ", complex.r: " << acomplex.rows << "\n"; throw std::runtime_error(""); }
				else if (acomplex.cols != 2 * areal.cols) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				axis = aaxis;
				int rank = 1;
				int nn[] = { areal.rows };
				n = areal.rows;
				int howmany = areal.cols; // col
				int idist = 1, odist = 1; // ???
				int istride = areal.cols, ostride = areal.cols; // cols
				int* inembed = nn, * onembed = nn;
				fwd = fftwf_plan_many_dft_r2c(rank, nn, howmany, &areal(), inembed, istride, idist, (fftwf_complex*)&acomplex(), onembed, ostride, odist, fftw_flag);
			}
			else if (aaxis == 1) {
				if (acomplex.cols != 2 * (areal.cols / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				else if (acomplex.rows != areal.rows) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				int rank = 1;
				int nn[] = { areal.cols };
				n = areal.cols;
				int howmany = 3; // col
				int idist = 20, odist = 11; // ???
				int istride = 1, ostride = 1; // cols
				int* inembed = nn, * onembed = nn;
				fwd = fftwf_plan_many_dft_r2c(rank, nn, howmany, &areal(), inembed, istride, idist, (fftwf_complex*)&acomplex(), onembed, ostride, odist, fftw_flag);
			}
			else { std::cout << "\na2d_rfft: invalid axis\n"; throw std::runtime_error(""); }
			real_r = real->rows;
			real_c = real->cols;
			complex_r = complex->rows;
			complex_c = complex->cols;
		}
		~a2d_rfft() {
			if (!destroyed) {
				fftwf_destroy_plan(fwd);
			}
		}
		bool verify_arrays() {
			bool passed = true;
			if (real->rows != real_r) { passed = false; }
			else if (real->cols != real_c) { passed = false; }
			else if (complex->rows != complex_r) { passed = false; }
			else if (complex->cols != complex_c) { passed = false; }
			return passed;
		}
		void destroy() {
			if (!destroyed) {
				fftwf_destroy_plan(fwd);
			}
			destroyed = true;
		}
	};
	template <typename T = float>
	class a2d_irfft {
	private:
		bool destroyed = false;
		int real_r, real_c, complex_r, complex_c;
	public:
		float n;
		int axis;
		fftwf_plan bck;
		a2d::Array2D<float>* real;
		a2d::Array2D<float>* complex;
		a2d_irfft() {
			bck = NULL;
			real = NULL;
			complex = NULL;
			real_r = real_c = complex_r = complex_c = 0;
			n = 0.0;
		}
		void plan(a2d::Array2D<float>& areal, a2d::Array2D<float>& acomplex, int aaxis = 0) {
			const unsigned int fftw_flag = FFTW_MEASURE;
			if (aaxis == 0) {
				if (acomplex.rows != (areal.rows / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << ", real.r: " << areal.rows << ", complex.r: " << acomplex.rows << "\n"; throw std::runtime_error(""); }
				else if (acomplex.cols != 2 * areal.cols) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				axis = aaxis;
				int rank = 1;
				int nn[] = { areal.rows };
				n = areal.rows;
				int howmany = areal.cols; // col
				int idist = 1, odist = 1; // ???
				int istride = areal.cols, ostride = areal.cols; // cols
				int* inembed = nn, * onembed = nn;
				bck = fftwf_plan_many_dft_c2r(rank, nn, howmany, (fftwf_complex*)&acomplex(), onembed, ostride, odist, &areal(), inembed, istride, idist, fftw_flag);
			}
			else if (aaxis == 1) {
				if (acomplex.cols != 2 * (areal.cols / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				else if (acomplex.rows != areal.rows) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				int rank = 1;
				int nn[] = { areal.cols };
				n = areal.cols;
				int howmany = 3; // col
				int idist = 20, odist = 11; // ???
				int istride = 1, ostride = 1; // cols
				int* inembed = nn, * onembed = nn;
				bck = fftwf_plan_many_dft_c2r(rank, nn, howmany, (fftwf_complex*)&acomplex(), onembed, ostride, odist, &areal(), inembed, istride, idist, fftw_flag);
			}
			else { std::cout << "\na2d_rfft: invalid axis\n"; throw std::runtime_error(""); }
			real_r = real->rows;
			real_c = real->cols;
			complex_r = complex->rows;
			complex_c = complex->cols;
		}
		~a2d_irfft() {
			this->destroy();
		}
		bool verify_arrays() {
			bool passed = true;
			if (real->rows != real_r) { passed = false; }
			else if (real->cols != real_c) { passed = false; }
			else if (complex->rows != complex_r) { passed = false; }
			else if (complex->cols != complex_c) { passed = false; }
			if (!passed) { "\na2d_rfft: arrays sizes have changed, plan is invalid\n"; }
			return passed;
		}
		void destroy() {
			if (!destroyed) {
				fftwf_destroy_plan(bck);
			}
			destroyed = true;
		}
	};
	// safety checks
	template<typename T = float>
	bool a2d_check_dims_complex(const a2d::Array2D<T>& test1, const a2d::Array2D<T>& test2) {
		bool passed = true;
		if (test1.vrows != test2.vrows) { 
			std::cout << "a2d_check_dims_complex: rows do not match\n";
			passed = false;
		}
		if (test1.vcols != test2.vcols) { 
			std::cout << "a2d_check_dims_complex: complex cols aren't twice real cols\n";
			passed = false;
		}
		if (test1.vcols % 2 != 0) { 
			std::cout << "a2d_check_dims_complex: complex cols not divisible by 2\n";
			passed = false;
		}
		return passed;
	}
	// transforms
	template<typename T = float>
	a2d::Array2D<T>& rfft(a2d_rfft<float>& test) {
#if defined(a2d_fftw3_SAFE)
		if (!test.verify_arrays()) { throw std::runtime_error(""); }
#endif
		fftwf_execute(test.fwd);
		return *test.complex;
	}
	template<typename T = float>
	a2d::Array2D<T>& irfft(a2d_irfft<float>& test) {
#if defined(a2d_fftw3_SAFE)
		if (!test.verify_arrays()) { throw std::runtime_error(""); }
#endif
		fftwf_execute(test.bck);
		return a2d::kmult(*test.real, (float) 1 / test.n);
	}
	template<typename T = float>
	a2d::Array2D<T>& c_amult(a2d::Array2D<float>& test1, const a2d::Array2D<float>& test2) {
		float a = 0, b = 0, cols = 0, d = 0;
#if defined(a2d_fftw3_SAFE)
		if (!a2d_check_dims_complex(test1, test2)) { throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols / 2; j++) {
				a = test1(i, 2 * j);
				b = test1(i, 2 * j + 1);
				cols = test2(i, 2 * j);
				d = test2(i, 2 * j + 1);
				test1(i, 2 * j) = a * cols - b * d;
				test1(i, 2 * j + 1) = a * d + b * cols;
			}
		}
		return test1;
	}
	template<typename T = float>
	a2d::Array2D<T>& c_amult(a2d::Array2D<float>& test1, const a2d::Array2D<float>& test2, const a2d::Array2D<float>& test3) {
		float a = 0, b = 0, c = 0, d = 0;
#if defined(a2d_fftw3_SAFE)
		if (!a2d_check_dims_complex(test1, test2)) { throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols / 2; j++) {
				a = test2(i, 2 * j);
				b = test2(i, 2 * j + 1);
				c = test3(i, 2 * j);
				d = test3(i, 2 * j + 1);
				test1(i, 2 * j) = a * c - b * d;
				test1(i, 2 * j + 1) = a * d + b * c;
			}
		}
		return test1;
	}
	template<typename T = float>
	a2d::Array2D<T>& c_kmult(a2d::Array2D<float>& test1, float real, float imag) {
		float a = 0, b = 0;
#if defined(a2d_fftw3_SAFE)
		if (test1.vcols % 2 != 0) { std::cout << "\na2d c_kmult error: cols must be a multiple of 2, cols: " << test1.cols << "\n"; throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vrows; i++) {
			for (auto j = 0; j < test1.vcols / 2; j++) {
				a = test1(i, 2 * j);
				b = test1(i, 2 * j + 1);
				test1(i, 2 * j) = a * real - b * imag;
				test1(i, 2 * j + 1) = a * imag + b * real;
			}
		}
		return test1;
	}
}