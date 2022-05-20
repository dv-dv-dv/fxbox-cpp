#pragma once
#include <iostream>
#include <tgmath.h>
#include <thread>
#include <atomic>
#include "rqueue.h"
#include <AudioFile.h>
#include "a2d.h"
#include "a2d_fftw3.h"
#include "globals.h"
class Convolver {
private:
	class Spectra {
	public: 
		long count, buffers_needed, filter_blength;
		a2d::Array2Dy<pflt> real, complex1, complex2;
		Spectra();
		int create(int buffers_needed, int filter_blength);
	};
	class FilterPartition {
	protected:
		Spectra* spec;
		a2d::Array2Dy<pflt> filter_rfft;
		a2d_fft_f::a2d_rfft<pflt> rfft1;
		a2d_fft_f::a2d_irfft<pflt> irfft1;
	public:
		long buffers_needed, offset, filter_blength, count, deadline;
		a2d::Array2D<pflt>* real;
		FilterPartition();
		FilterPartition(a2d::Array2D<pflt>& filter, Spectra& spec, int offset, int buffers_needed, int filter_blength);
		void create(a2d::Array2D<pflt>& filter, Spectra& spec, int offset, int buffers_needed, int filter_blength);
		bool spectra_needed();
		void schedule(long count);
		a2d::Array2D<pflt>& convolve_no_rfft();
		a2d::Array2D<pflt>& convolve_rfft();
	};
	long longest_filter, convolution_buffer_blength;
	a2d::Array2Dy<FilterPartition> filters;
	a2d::Array2Dy<Spectra> spectras;
	a2d::Array2Dy<pflt> convolution_buffer;
	a2d::Array2Dy<pflt> previous_buffers;
	FilterPartition first_filter;
	Spectra first_spectra;
	q::d_rqueue<FilterPartition*> convolution_queue;
	q::d_rqueue<FilterPartition*> convolution_queue2;
	std::thread convolution_worker_thread;
	std::thread convolution_worker_thread2;
	std::atomic<bool> allow_convolution_worker_thread;
	std::atomic<bool> convolution_buffer_lock;
	std::atomic<long> count;
	int add_to_convolution_buffer(a2d::Array2D<pflt>& audio_in, long count, long offset);
	a2d::Array2D<pflt>& get_previous_buffers(a2d::Array2D<pflt>& real, int no_previous_buffers, long count);
	a2d::Array2D<pflt>& get_from_convolution_buffer(a2d::Array2D<pflt>& current_buffer);
	int partition_impulse(int ff_blength, int height, int n_cap, int n_step, int n_start);
	void convolution_worker();
public: 
	Convolver(int test);
	~Convolver();
	int close();
	a2d::Array2D<pflt>& convolve(a2d::Array2D<pflt>& audio_in);
};

