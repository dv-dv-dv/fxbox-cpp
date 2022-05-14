#pragma once
#include <iostream>
#include "convolver.h"
Convolver::Filter::Filter() {
	buffers_needed = offset = real_size = complex_size = 0;
}
Convolver::Filter::Filter(int abuffers_needed, int aoffset, a2d::Array2D<pflt>& afilter) {
	create(abuffers_needed, aoffset, afilter);
}
void Convolver::Filter::create(int abuffers_needed, int aoffset, a2d::Array2D<pflt>& afilter) {
	using namespace a2d_fft_f; using namespace a2d;
	Array2Dy<pflt> filter;
	buffers_needed = abuffers_needed;
	offset = aoffset;
	real_size = 2 * buffer_size * buffers_needed;
	complex_size = real_size / 2 + 1;

	real.resize(real_size, 2);
	filter.resize(real_size, 2);
	complex.resize(complex_size, 4);
	filter_rfft.resize(complex_size, 4);

	rfft1.plan(filter, filter_rfft, 0);
	aset(filter.vpart(0, afilter.r), afilter).vreset();
	rfft(rfft1);

	rfft1.plan(real, complex); 
}
int Convolver::Filter::convolve() {
	using namespace a2d_fft_f;
	rfft(rfft1);
	c_amult(complex, filter_rfft);
	irfft(rfft1);
	return 0;
}
int Convolver::Filter::convolve_rfft() {

	return 0;
}

int Convolver::add_to_convolution_buffer(a2d::Array2D<pflt>& audio_in, long count, long offset) {
	long index1 = ((count + offset) % convolution_buffer_blength) * buffer_size;
	long index2 = index1 + audio_in.r;
	audio_in.vreset(); convolution_buffer.vreset();
	if (index2 > convolution_buffer.r) {
		long delta1 = convolution_buffer.r - index1;
		long delta2 = audio_in.r - delta1;
		aadd(convolution_buffer.vpart(index1, index1 + delta1), audio_in.vpart(0, delta1));
		aadd(convolution_buffer.vpart(0, delta2), audio_in.vpart(delta1, delta1 + delta2));
	}
	else {
		aadd(convolution_buffer.vpart(index1, index2), audio_in);
	}
	audio_in.vreset(); convolution_buffer.vreset();
	return 0;
} 
a2d::Array2D<pflt>& Convolver::get_previous_buffers(a2d::Array2D<pflt>& real, int no_previous_buffers, long count) {
	using namespace a2d;
	long count_pb = count % longest_filter;
	long index1 = (count_pb + 1 - no_previous_buffers) * buffer_size;
	real.vreset(); previous_buffers.vreset();
	if (index1 < 0) {
		index1 += previous_buffers.r;
		long delta1 = previous_buffers.r - index1;
		long delta2 = no_previous_buffers - delta1;
		real.vpart(0, delta1);
		previous_buffers.vpart(index1, index1 + delta1);
		aset(real, previous_buffers);
		real.vpart(delta1, delta1 + delta2); 
		previous_buffers.vpart(0, delta2);
		aset(real, previous_buffers);
	}
	else {
		long index2 = (count_pb + 1) * buffer_size;
		aset(real.vpart(0, no_previous_buffers * buffer_size), previous_buffers.vpart(index1, index2));
	}
	real.vreset(); previous_buffers.vreset();
	return real;
}
a2d::Array2D<pflt>& Convolver::get_from_convolution_buffer(a2d::Array2D<pflt>& getter) {
	using namespace a2d;
	long index1 = (count % convolution_buffer_blength) * buffer_size;
	long index2 = index1 + buffer_size;
	convolution_buffer.vpart(index1, index2);
	aset(getter, convolution_buffer);
	kset(convolution_buffer, 0);
	convolution_buffer.vreset();
	kmult(getter, 0.25);
	return getter;
}
int Convolver::partition_impulse(int ff_blength, int height, int n_cap, int n_step, int n_start) {
	using namespace a2d;
	AudioFile<pflt> infile;
	infile.load("longverb.wav");
	int num_samples = infile.getNumSamplesPerChannel();
	int f_blength = num_samples / buffer_size;

	Array2Dy<int> buffers_needed(f_blength);
	Array2Dy<int> filter_indices(f_blength);
	Array2Dy<int> unique_spectras(n_cap - n_start);
	Array2Dy<int> offsets(f_blength);
	kset(offsets, ff_blength);

	int n = n_start;
	int num_filters = 0;
	int num_spectras = 0;
	int sum_of_previous_buffers = 0;
	int sum_of_previous_filters = 0;
	int previous_buffers_needed = 0;
	int space_left = f_blength;

	while (space_left > 0) {
		buffers_needed(num_filters) = pow(2, n);
		space_left -= buffers_needed(num_filters);
		sum_of_previous_filters += buffers_needed(num_filters);
		filter_indices(num_filters + 1) = buffer_size*buffers_needed(num_filters) + filter_indices(num_filters);
		if (buffers_needed(num_filters) != previous_buffers_needed) {
			sum_of_previous_buffers += buffers_needed(num_filters) - previous_buffers_needed;
			num_spectras++;
		}
		offsets(num_filters) += sum_of_previous_filters - sum_of_previous_buffers + 1;
		previous_buffers_needed = buffers_needed(num_filters);
		n += n_step * floor((num_filters % height + 1) / (pflt)height);
		if (n > n_cap) { n = n_cap; }
		num_filters++;
	}

	convolution_buffer_blength =  ceil(asum(buffers_needed) / (float)buffers_needed(num_filters - 1)) * buffers_needed(num_filters - 1);
	convolution_buffer.resize(convolution_buffer_blength * buffer_size, 2);
	previous_buffers.resize(buffers_needed(num_filters - 1) * buffer_size, 2);
	filters.resize(num_filters);

	Array2Dy<pflt> filter_partition;
	for (auto i = 0; i < num_filters; i++) {
		filter_partition.resize(buffers_needed(i) * buffer_size, 2);
		if (i > num_filters) { std::cout << "aasdasd"; }
		for (auto j = 0; j < filter_partition.r; j++) {
			if (j + filter_indices(i) == num_samples) { break; }
			filter_partition(j, 0) = infile.samples[0][j + filter_indices(i)];
			filter_partition(j, 1) = infile.samples[1][j + filter_indices(i)];
		}
		filters(i).create(buffers_needed(i), offsets(i), filter_partition);
	}
	longest_filter = filters(-1).buffers_needed;

	bool test = true;
	if (test) {
		for (auto i = 0; i < num_filters; i++) {
			std::cout << i << " " << buffers_needed(i) << " " << offsets(i) << " " << filter_indices(i) << "\n";
		}
		std::cout << "\nlongest_filter: " << longest_filter;
		std::cout << "\nunique_spectras: " << num_spectras << "\n";
	}
	return 0;
}
Convolver::Convolver() {
	count = 0;
	partition_impulse(0, 4, 5, 1, 0);
}
a2d::Array2D<pflt>& Convolver::convolve(a2d::Array2D<pflt>& audio_in) {
	int buffer_pos = buffer_size * (count % longest_filter);
	previous_buffers.vpart(buffer_pos, buffer_pos + buffer_size);
	aset(previous_buffers, audio_in).vreset();
	for (int i = 0; i < filters.l; i++) {
		if ((count + 1) % filters(i).buffers_needed != 0) break;
		a2d::kset(filters(i).real, 0);
		a2d::kset(filters(i).complex, 0);
		get_previous_buffers(filters(i).real, filters(i).buffers_needed, count);
		filters(i).convolve();
		add_to_convolution_buffer(filters(i).real, count, filters(i).offset);
	}
	get_from_convolution_buffer(audio_in);
	count++;
	return audio_in;
}

