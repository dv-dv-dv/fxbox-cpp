#pragma once
#include <iostream>
#include <tgmath.h>
#include <AudioFile.h>
#include "a2d.h"
#include "a2d_fftw3.h"
#include "globals.h"
class Convolver {
private:
	class Filter {
	public:
		long buffers_needed, offset, real_size, complex_size;
		a2d::Array2Dy<pflt> real;
		a2d::Array2Dy<pflt> complex;
		a2d::Array2Dy<pflt> filter_rfft;
		a2d_fft_f::a2d_rfft<pflt> rfft1; 
		Filter();
		Filter(int abuffers_needed, int aoffset, a2d::Array2D<pflt>& afilter);
		void create(int abuffers_needed, int aoffset, a2d::Array2D<pflt>& afilter);
		int convolve();
		int convolve_rfft();
	};

	long count = 0, longest_filter, convolution_buffer_blength;
	a2d::Array2Dy<Filter> filters;
	a2d::Array2Dy<pflt> convolution_buffer;
	a2d::Array2Dy<pflt> previous_buffers;
	a2d::Array2Dy<pflt> covolution_buffer_getter;

	int add_to_convolution_buffer(a2d::Array2D<pflt>& audio_in, long count, long offset);
	a2d::Array2D<pflt>& get_previous_buffers(a2d::Array2D<pflt>& real, int no_previous_buffers, long count);
	a2d::Array2D<pflt>& get_from_convolution_buffer(a2d::Array2D<pflt>& current_buffer);
	int partition_impulse(int ff_blength, int height, int n_cap, int n_step, int n_start);
public:
	Convolver();
	a2d::Array2D<pflt>& convolve(a2d::Array2D<pflt>& audio_in);
};

