#pragma once
#include <iostream>
#include <fftw3.h>
#include "a2d.h"
#define SAFE
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
				if (acomplex.r != (areal.r / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << ", real.r: " << areal.r << ", complex.r: " << acomplex.r << "\n"; throw std::runtime_error(""); }
				else if (acomplex.c != 2 * areal.c) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				axis = aaxis;
				int rank = 1;
				int nn[] = { areal.r };
				n = areal.r;
				int howmany = areal.c; // col
				int idist = 1, odist = 1; // ???
				int istride = areal.c, ostride = areal.c; // cols
				int* inembed = nn, * onembed = nn;
				fwd = fftwf_plan_many_dft_r2c(rank, nn, howmany, areal.ptr, inembed, istride, idist, (fftwf_complex*)acomplex.ptr, onembed, ostride, odist, fftw_flag);
			}
			else if (aaxis == 1) {
				if (acomplex.c != 2 * (areal.c / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				else if (acomplex.r != areal.r) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				int rank = 1;
				int nn[] = { areal.c };
				n = areal.c;
				int howmany = 3; // col
				int idist = 20, odist = 11; // ???
				int istride = 1, ostride = 1; // cols
				int* inembed = nn, * onembed = nn;
				fwd = fftwf_plan_many_dft_r2c(rank, nn, howmany, areal.ptr, inembed, istride, idist, (fftwf_complex*)acomplex.ptr, onembed, ostride, odist, fftw_flag);
			}
			else { std::cout << "\na2d_rfft: invalid axis\n"; throw std::runtime_error(""); }
			real_r = real->r;
			real_c = real->c;
			complex_r = complex->r;
			complex_c = complex->c;
		}
		~a2d_rfft() {
			if (!destroyed) {
				fftwf_destroy_plan(fwd);
			}
		}
		bool verify_arrays() {
			bool passed = true;
			if (real->r != real_r) { passed = false; }
			else if (real->c != real_c) { passed = false; }
			else if (complex->r != complex_r) { passed = false; }
			else if (complex->c != complex_c) { passed = false; }
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
				if (acomplex.r != (areal.r / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << ", real.r: " << areal.r << ", complex.r: " << acomplex.r << "\n"; throw std::runtime_error(""); }
				else if (acomplex.c != 2 * areal.c) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				axis = aaxis;
				int rank = 1;
				int nn[] = { areal.r };
				n = areal.r;
				int howmany = areal.c; // col
				int idist = 1, odist = 1; // ???
				int istride = areal.c, ostride = areal.c; // cols
				int* inembed = nn, * onembed = nn;
				bck = fftwf_plan_many_dft_c2r(rank, nn, howmany, (fftwf_complex*)acomplex.ptr, onembed, ostride, odist, areal.ptr, inembed, istride, idist, fftw_flag);
			}
			else if (aaxis == 1) {
				if (acomplex.c != 2 * (areal.c / 2 + 1)) { std::cout << "\na2d_rfft: rows do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				else if (acomplex.r != areal.r) { std::cout << "\na2d_rfft: cols do not match for given axis of " << aaxis << "\n"; throw std::runtime_error(""); }
				real = &areal;
				complex = &acomplex;
				int rank = 1;
				int nn[] = { areal.c };
				n = areal.c;
				int howmany = 3; // col
				int idist = 20, odist = 11; // ???
				int istride = 1, ostride = 1; // cols
				int* inembed = nn, * onembed = nn;
				bck = fftwf_plan_many_dft_c2r(rank, nn, howmany, (fftwf_complex*)acomplex.ptr, onembed, ostride, odist, areal.ptr, inembed, istride, idist, fftw_flag);
			}
			else { std::cout << "\na2d_rfft: invalid axis\n"; throw std::runtime_error(""); }
			real_r = real->r;
			real_c = real->c;
			complex_r = complex->r;
			complex_c = complex->c;
		}
		~a2d_irfft() {
			if (!destroyed) {
				fftwf_destroy_plan(bck);
			}
		}
		bool verify_arrays() {
			bool passed = true;
			if (real->r != real_r) { passed = false; }
			else if (real->c != real_c) { passed = false; }
			else if (complex->r != complex_r) { passed = false; }
			else if (complex->c != complex_c) { passed = false; }
			return passed;
		}
		void destroy() {
			if (!destroyed) {
				fftwf_destroy_plan(bck);
			}
			destroyed = true;
		}
	};
	template<typename T = float>
	a2d::Array2D<T>& rfft(a2d_rfft<float>& test) {
#if defined(SAFE)
		if (!test.verify_arrays()) { std::cout << "\na2d_rfft: arrays sizes have changed, plan is invalid\n"; throw std::runtime_error(""); }
#endif
		fftwf_execute(test.fwd);
		return *test.complex;
	}
	template<typename T = float>
	a2d::Array2D<T>& irfft(a2d_irfft<float>& test) {
#if defined(SAFE)
		if (!test.verify_arrays()) { std::cout << "\na2d_rfft: arrays sizes have changed, plan is invalid\n"; throw std::runtime_error(""); }
#endif
		fftwf_execute(test.bck);
		return a2d::kmult(*test.real, (float) 1 / test.n);
	}
	template<typename T = float>
	a2d::Array2D<T>& c_amult(a2d::Array2D<float>& test1, const a2d::Array2D<float>& test2) {
		float a = 0, b = 0, c = 0, d = 0;
#if defined(SAFE)
		if (test1.vr != test2.vr) { std::cout << "\na2d aset error: rows mismatch: test1.r " << test1.r << ", test2.r: " << test2.r << "\n"; throw std::runtime_error(""); }
		if (test1.vc != test2.vc) { std::cout << "\na2d aset error: cols mismatch: test1.c " << test1.c << ", test2.c: " << test2.c << "\n"; throw std::runtime_error(""); }
		if (test1.vc % 2 != 0) { std::cout << "\na2d aset error: cols mismatch: test1.c " << test1.c << ", test2.c: " << test2.c << "\n"; throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc / 2; j++) {
				a = test1(i, 2 * j);
				b = test1(i, 2 * j + 1);
				c = test2(i, 2 * j);
				d = test2(i, 2 * j + 1);
				test1(i, 2 * j) = a * c - b * d;
				test1(i, 2 * j + 1) = a * d + b * c;
			}
		}
		return test1;
	}
	template<typename T = float>
	a2d::Array2D<T>& c_amult(a2d::Array2D<float>& test3, const a2d::Array2D<float>& test1, const a2d::Array2D<float>& test2) {
		float a = 0, b = 0, c = 0, d = 0;
#if defined(SAFE)
		if (test1.vr != test2.vr) { std::cout << "\na2d aset error: rows mismatch: test1.r " << test1.r << ", test2.r: " << test2.r << "\n"; throw std::runtime_error(""); }
		if (test1.vc != test2.vc) { std::cout << "\na2d aset error: cols mismatch: test1.c " << test1.c << ", test2.c: " << test2.c << "\n"; throw std::runtime_error(""); }
		if (test1.vc % 2 != 0) { std::cout << "\na2d aset error: cols mismatch: test1.c " << test1.c << ", test2.c: " << test2.c << "\n"; throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc / 2; j++) {
				a = test1(i, 2 * j);
				b = test1(i, 2 * j + 1);
				c = test2(i, 2 * j);
				d = test2(i, 2 * j + 1);
				test3(i, 2 * j) = a * c - b * d;
				test3(i, 2 * j + 1) = a * d + b * c;
			}
		}
		return test3;
	}
	template<typename T = float>
	a2d::Array2D<T>& c_kmult(a2d::Array2D<float>& test1, float re, float im) {
		float a = 0, b = 0, c = re, d = im;
#if defined(SAFE)
		if (test1.vc % 2 != 0) { std::cout << "\na2d c_kmult error: cols must be a multiple of 2, cols: " << test1.c << "\n"; throw std::runtime_error(""); }
#endif
		for (auto i = 0; i < test1.vr; i++) {
			for (auto j = 0; j < test1.vc / 2; j++) {
				a = test1(i, 2 * j);
				b = test1(i, 2 * j + 1);
				test1(i, 2 * j) = a * c - b * d;
				test1(i, 2 * j + 1) = a * d + b * c;
			}
		}
		return test1;
	}
}