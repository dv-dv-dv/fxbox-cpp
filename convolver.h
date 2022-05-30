#pragma once
#include <iostream>
#include <cmath>
#include <thread>
#include <atomic>
#include <string>
#include <AudioFile.h>
#include "rqueue.h"
#include "a2d.h"
#include "a2d_fftw3.h"
#include "globals.h"
class Convolver {
private:
	class Spectra {
	public: 
		int count, buffers_needed, filter_blength;
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
		short count, deadline, buffers_needed, offset, filter_blength;
		a2d::Array2D<pflt>* real;
		FilterPartition();
		FilterPartition(a2d::Array2D<pflt>& filter, Spectra& spec, int offset, int buffers_needed, int filter_blength);
		void create(a2d::Array2D<pflt>& filter, Spectra& spec, int offset, int buffers_needed, int filter_blength);
		bool spectra_needed();
		void schedule(int count);
		a2d::Array2D<pflt>& convolve_no_rfft();
		a2d::Array2D<pflt>& convolve_rfft();
	};
	bool realtime;
	int longest_filter, convolution_buffer_blength, max_threads, threads;
	q::d_rqueue<FilterPartition*> convolution_queue;
	q::d_rqueue<FilterPartition*> convolution_queue2;
	Spectra first_spectra;
	std::thread convolution_worker_thread;
	std::atomic<bool> allow_convolution_worker_thread; // only allow one writer
	std::atomic<bool> convolution_buffer_lock; // only allow one writer
	std::atomic<short> count; // only allow one writer
	a2d::Array2Dy<FilterPartition> filters;
	a2d::Array2Dy<Spectra> spectras;
	a2d::Array2Dy<pflt> convolution_buffer;
	a2d::Array2Dy<pflt> previous_buffers;
	FilterPartition first_filter;
	int add_to_convolution_buffer(a2d::Array2D<pflt>& convolution_buffer, a2d::Array2D<pflt>& audio_in, int count, int offset);
	a2d::Array2D<pflt>& get_previous_buffers(a2d::Array2D<pflt>& real, int no_previous_buffers, int count);
	a2d::Array2D<pflt>& get_from_convolution_buffer(a2d::Array2D<pflt>& convolution_buffer, a2d::Array2D<pflt>& getter);
	int partition_impulse(std::string filtername, int ff_blength, int height, int n_cap, int n_step, int n_start);
	void convolution_worker(a2d::Array2D<pflt>* convolution_buffer, q::d_rqueue<FilterPartition*>* convolution_queue);
public: 
	Convolver(std::string filename, bool realtime = false);
	~Convolver();
	int close();
	a2d::Array2D<pflt>& convolve(a2d::Array2D<pflt>& audio_in);
};

