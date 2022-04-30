#pragma once
#include <iostream>
#include <tgmath.h>
#include <AudioFile.h>
#include <fftw3.h>
#include "a2d.h"
#include "globals.h"
class Convolver {
private:
	class Filter {
	public:
		int buffers_needed, offset;
		a2d::Array2Dy<pflt> filter_rfft;
		a2d::Array2Dy<pflt> audio_in_rfft;
		fftwf_plan p;
		Filter() {
			buffers_needed = offset = 0;
			p = NULL;
		}
		Filter(int abuffers_needed, int aoffset, a2d::Array2D<pflt>& afilter) {
			create(abuffers_needed, aoffset, afilter);
		}
		void create(int abuffers_needed, int aoffset, a2d::Array2D<pflt>& afilter) {
			buffers_needed = abuffers_needed;
			offset = aoffset;
		}
		int convolve() {
			return 0;
		}
		int convolve_rfft() {
			return 0;
		}
	};
	long count;
	a2d::Array2Dy<Filter> filters;
	a2d::Array2Dy<pflt> convolution_buffer;
	a2d::Array2Dy<pflt> previous_buffers;
	int add_to_convolution_buffer(a2d::Array2D<pflt>& audio_in, long count, long offset) {
		long index1 = count + offset;
		long index2 = index1 + audio_in.c;
		audio_in.vreset(); convolution_buffer.vreset();
		if (index2 > convolution_buffer.c) {
			long delta1 = convolution_buffer.c - index1;
			long delta2 = audio_in.c - delta1;
			aadd(convolution_buffer.vpart(0, -1, index1, index1 + delta1), audio_in.vpart(0, -1, 0, delta1));
			aadd(convolution_buffer.vpart(0, -1, 0, delta2), audio_in.vpart(0, -1, delta1, delta1 + delta2));
		}
		else {
			aadd(convolution_buffer.vpart(0, -1, index1, index2), audio_in);
		}
		audio_in.vreset(); convolution_buffer.vreset();
		return 0;
	}
	a2d::Array2Dy<pflt>& get_n_previous_buffers(int n, long count) {
		using namespace a2d;
		Array2Dy<pflt> n_previous_buffers(2, n * buffer_size);
		long count_pb = count % (filters(filters.l - 1).buffers_needed);
		long index1 = (count_pb + 1 - n)*buffer_size;
		n_previous_buffers.vreset(); previous_buffers.vreset();
		if (index1 < 0) {
			index1 += previous_buffers.c;
			long delta1 = previous_buffers.c - index1;
			long delta2 = n - delta1;
			aset(n_previous_buffers.vpart(0, -1, 0, delta1), previous_buffers.vpart(0, -1, index1, index1 + delta1));
			aset(n_previous_buffers.vpart(0, -1, delta1, delta1 + delta2), previous_buffers.vpart(0, -1, 0, delta2));
		}
		else {
			long index2 = (count_pb + 1) * buffer_size;
			aset(n_previous_buffers, previous_buffers.vpart(0, -1, index1, index2));
		}
		n_previous_buffers.vreset(); previous_buffers.vreset();
		return n_previous_buffers;
	}
	a2d::Array2Dy<pflt>& get_from_convolution_buffer() {
		using namespace a2d;
		long index1 = count * buffer_size;
		long index2 = index1 + buffer_size;
		convolution_buffer.vreset();
		Array2Dy<pflt> audio_out(convolution_buffer.vpart(0, -1, index1, index2));
		kset(convolution_buffer, 0);
		convolution_buffer.vreset();
		return audio_out;
	}
	int partition_impulse(int ff_blength, int height, int n_cap, int n_step, int n_start) {
		using namespace a2d;
		AudioFile<pflt> infile;
		infile.load("shortverb.wav");
		int num_samples = infile.getNumSamplesPerChannel();
		int f_blength = num_samples / buffer_size;

		Array2Dy<int> buffers_needed(1, f_blength);
		Array2Dy<int> filter_indices(1, f_blength);
		Array2Dy<int> offsets(1, f_blength);
		kset(offsets, ff_blength);

		int numf = 0;
		int n = n_start;
		int sum_of_previous_buffers = 0;
		int sum_of_previous_filters = 0;
		int previous_buffers_needed = 0;
		int space_left = f_blength;

		while (space_left > 0) {
			buffers_needed(numf) = pow(2, n);
			space_left -= buffers_needed(numf);
			sum_of_previous_filters += buffers_needed(numf);
			filter_indices(numf + 1) = buffers_needed(numf) + filter_indices(numf);
			if (buffers_needed(numf) != previous_buffers_needed) {
				sum_of_previous_buffers += buffers_needed(numf) - previous_buffers_needed;
			}
			offsets(numf) += sum_of_previous_filters - sum_of_previous_buffers + 1;
			previous_buffers_needed = buffers_needed(numf);
			n += n_step * floor((numf % height + 1) / (pflt)height);
			if (n > n_cap) { n = n_cap; }
			numf++;
		}

		int convolution_buffer_blength = ceil(asum(buffers_needed) / (float)buffers_needed(numf - 1)) * buffers_needed(numf - 1);
		convolution_buffer.resize(2, convolution_buffer_blength * buffer_size);
		previous_buffers.resize(2, buffers_needed(numf - 1) * buffer_size);
		filters.resize(numf);
		for (auto i = 0; i < numf; i++) {
			Array2Dy<pflt> filter_partition = Array2Dy<pflt>(2, buffers_needed(i) * buffer_size);
			if (i > numf) { std::cout << "aasdasd"; }
			for (auto j = 0; j < filter_partition.c; j++) {
				if (j + filter_indices(i) == num_samples) { break; }
				filter_partition(0, j) = infile.samples[0][j + filter_indices(i)];
				filter_partition(1, j) = infile.samples[1][j + filter_indices(i)];
			}
			filters(i).create(buffers_needed(i), offsets(i), filter_partition);
		}
		bool test = true;
		if (test) {
			for (auto i = 0; i < numf; i++) {
				std::cout << i << " " << buffers_needed(i) << " " << offsets(i) << " " << filter_indices(i) << "\n";
			}
		}
		return 0;
	}
public:
	Convolver() {
		count = 0;
		partition_impulse(3, 4, 5, 2, 1);
	}
	int convolve() {
		for (int i = 0; i < filters.l; i++) {
			if (!((count + 1) % filters(i).buffers_needed)) break;
		}
		count++;
		return 0;
	}
};

