#pragma once
#include <iostream>
#include "convolver.h"
Convolver::FilterPartition::FilterPartition() {
	buffers_needed = offset = filter_blength = count = deadline = -1;
	spec = NULL;
	real = NULL;
}
Convolver::FilterPartition::FilterPartition(a2d::Array2D<pflt>& filter, Spectra& spec, int offset, int buffers_needed, int filter_blength = -1) {
	this->create(filter, spec, offset, buffers_needed, filter_blength);
}
void Convolver::FilterPartition::create(a2d::Array2D<pflt>& filter, Spectra& spec, int offset, int buffers_needed, int filter_blength = -1) {
	using namespace a2d_fft_f; using namespace a2d;
	Array2Dy<pflt> filter_temp;
	this->filter_blength = filter_blength;
	if (filter_blength == -1) { 
		this->filter_blength = buffers_needed;
	}
	this->buffers_needed = buffers_needed;
	this->spec = &spec;
	this->real = &this->spec->real;
	this->offset = offset;
	int real_size = buffer_size * (this->buffers_needed + this->filter_blength);
	int complex_size = real_size / 2 + 1;

	filter_temp.resize(real_size, 2);
	filter_rfft.resize(complex_size, 4);

	rfft1.plan(filter_temp, filter_rfft, 0);
	aset(filter_temp.vpart(0, filter.rows), filter).vreset();
	rfft(rfft1);
	rfft1.plan(this->spec->real, this->spec->complex1);
	irfft1.plan(this->spec->real, this->spec->complex2);
}
bool Convolver::FilterPartition::spectra_needed() {
	if (spec->count == this->count) {
		return false;
	}
	else {
		spec->count = this->count;
		return true;
	}
}
void Convolver::FilterPartition::schedule(int count) {
	this->count = count;
	this->deadline = count + this->offset;
}
a2d::Array2D<pflt>& Convolver::FilterPartition::convolve_rfft() {
	using namespace a2d_fft_f; using namespace a2d;
	rfft(rfft1);
	c_amult(spec->complex2, spec->complex1, filter_rfft);
	irfft(irfft1);
	return spec->real;
}
a2d::Array2D<pflt>& Convolver::FilterPartition::convolve_no_rfft() {
	using namespace a2d_fft_f; using namespace a2d;
	c_amult(spec->complex2, spec->complex1, filter_rfft);
	irfft(irfft1);
	return spec->real;
}
int Convolver::Spectra::create(int buffers_needed, int filter_blength = -1) {
	this->filter_blength = filter_blength;
	if (filter_blength == -1) {
		this->filter_blength = buffers_needed;
	}
	this->buffers_needed = buffers_needed;
	const int real_size = buffer_size * (this->buffers_needed + this->filter_blength);
	const int complex_size = real_size / 2 + 1;
	real.resize(real_size, 2);
	complex1.resize(complex_size, 4);
	complex2.resize(complex_size, 4);
	return 0;
}
Convolver::Spectra::Spectra() {
	count = -1;
	buffers_needed = -1;
	filter_blength = -1;
}
int Convolver::add_to_convolution_buffer(a2d::Array2D<pflt>& convolution_buffer, a2d::Array2D<pflt>& audio_in, int count, int offset) {
	using namespace a2d;
	const int index1 = ((count + offset) % convolution_buffer_blength) * buffer_size;
	const int index2 = index1 + audio_in.rows;
	audio_in.vreset(); convolution_buffer.vreset();
	if (index2 > convolution_buffer.rows) {
		const int delta1 = convolution_buffer.rows - index1;
		const int delta2 = audio_in.rows - delta1;
		aadd(convolution_buffer.vpart(index1, index1 + delta1), audio_in.vpart(0, delta1));
		aadd(convolution_buffer.vpart(0, delta2), audio_in.vpart(delta1, delta1 + delta2));
	}
	else {
		aadd(convolution_buffer.vpart(index1, index2), audio_in);
	}
	audio_in.vreset(); convolution_buffer.vreset();
	kset(audio_in, 0);
	return 0;
} 
a2d::Array2D<pflt>& Convolver::get_previous_buffers(a2d::Array2D<pflt>& real, int no_previous_buffers, int count) {
	using namespace a2d;
	const int count_pb = count % longest_filter;
	const int index1 = (count_pb + 1 - no_previous_buffers) * buffer_size;
	real.vreset(); previous_buffers.vreset();
	if (index1 < 0) {
		const int index = index1 + previous_buffers.rows;
		const int delta1 = previous_buffers.rows - index1;
		const int delta2 = no_previous_buffers - delta1;
		real.vpart(0, delta1);
		previous_buffers.vpart(index1, index1 + delta1);
		aset(real, previous_buffers);
		real.vpart(delta1, delta1 + delta2); 
		previous_buffers.vpart(0, delta2);
		aset(real, previous_buffers);
	}
	else {
		const int index2 = (count_pb + 1) * buffer_size;
		aset(real.vpart(0, no_previous_buffers * buffer_size), previous_buffers.vpart(index1, index2));
	}
	real.vreset(); previous_buffers.vreset();
	return real;
}
a2d::Array2D<pflt>& Convolver::get_from_convolution_buffer(a2d::Array2D<pflt>& convolution_buffer, a2d::Array2D<pflt>& getter) {
	using namespace a2d;
	const int index1 = count  * buffer_size;
	const int index2 = index1 + buffer_size;
	convolution_buffer.vpart(index1, index2);
	convolution_buffer_lock = true;
	aset(getter, convolution_buffer);
	kset(convolution_buffer, 0);
	convolution_buffer.vreset();
	convolution_buffer_lock = false;
	kmult(getter, 0.25);
	return getter;
}
int Convolver::partition_impulse(std::string filename, int ff_blength, int height, int n_cap, int n_step, int n_start) {
	using namespace a2d;
	Array2Dy<pflt> filter_partition;
	AudioFile<pflt> infile;
	infile.load(filename);
	int num_samples = infile.getNumSamplesPerChannel();
	int f_blength = (num_samples + buffer_size) / buffer_size;
	Array2Dy<int> buffers_needed(f_blength);
	Array2Dy<int> filter_indices(f_blength);
	Array2Dy<int> offsets(f_blength);
	kset(offsets, ff_blength);
	filter_indices(0) = ff_blength * buffer_size;

	// first filter init
	filter_partition.resize(ff_blength * buffer_size, 2);
	for (auto i = 0; i < ff_blength * buffer_size; i++) {
		filter_partition(i, 0) = infile.samples[0][i];
		filter_partition(i, 1) = infile.samples[1][i];
	}
	first_spectra.create(1, ff_blength);
	first_filter.create(filter_partition, first_spectra, 0, 1, ff_blength);

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
		sum_of_previous_filters += previous_buffers_needed;
		filter_indices(num_filters + 1) = buffer_size * buffers_needed(num_filters) + filter_indices(num_filters);
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
	std::cout << "convolution_buffer_blength: " << convolution_buffer_blength << "\n";
	std::cout << "asum(buffers_needed): " << asum(buffers_needed) << "\n";
	std::cout << "actual blength: " << f_blength << "\n";
	convolution_buffer.resize(convolution_buffer_blength * buffer_size, 2);
	previous_buffers.resize(buffers_needed(num_filters - 1) * buffer_size, 2);

	filters.resize(num_filters);
	spectras.resize(num_spectras);
	auto k = 0;
	for (auto i = 0; i < num_filters; i++) {
		filter_partition.resize(buffers_needed(i) * buffer_size, 2);
		for (auto j = 0; j < filter_partition.rows; j++) {
			if (j + filter_indices(i) >= num_samples) { break; }
			filter_partition(j, 0) = infile.samples[0][j + filter_indices(i)];
			filter_partition(j, 1) = infile.samples[1][j + filter_indices(i)];
		}
		if (i == 0) {
			spectras(0).create(buffers_needed(0));
		}
		else if(buffers_needed(i) != buffers_needed(i - 1)) {
			k++;
			spectras(k).create(buffers_needed(i));
		}
		filters(i).create(filter_partition, spectras(k), offsets(i), buffers_needed(i));
	} 
	longest_filter = filters(num_filters - 1).buffers_needed;
	bool test = true;
	if (test) {
		std::cout << "filter number // buffers needed // offsets // filter indices\n";
		for (auto i = 0; i < num_filters; i++) {
			std::cout << i << " " << buffers_needed(i) << " " << offsets(i) << " " << filter_indices(i) << "\n"; 
		}
		std::cout << "longest_filter: " << longest_filter << "\n";
		std::cout << "unique_spectras: " << num_spectras << "\n"; 
	}
	convolution_queue.resize(num_filters);
	return 0;
}
void Convolver::convolution_worker(a2d::Array2D<pflt>* convolution_buffer, q::d_rqueue<FilterPartition*>* convolution_queue) {
	FilterPartition* test = NULL;
	while (allow_convolution_worker_thread) {
		if (!convolution_queue->empty()) {
			test = convolution_queue->front();
			while (test->count == count) {}
			if (test->spectra_needed()) {
				get_previous_buffers(*test->real, test->buffers_needed, test->count);
				test->convolve_rfft();
			}
			else {
				test->convolve_no_rfft();
			}
			while (convolution_buffer_lock) {}
			add_to_convolution_buffer(*convolution_buffer, *test->real, test->count, test->offset);
			convolution_queue->pop();
		}
	}
}
Convolver::Convolver(std::string filename, bool realtime) {
	std::cout << "initializing convolver...\n";
	const int ff_power = 2;
	const int n_step = 2;
	const int ff_blength = pow(2, ff_power) - 1;
	const int n_cap = ceil(log2(pow(2, 14) / buffer_size));
	const int height = 4;
	const int n_start = ff_power - 1;
	this->realtime = realtime;
	partition_impulse(filename, ff_blength, height, n_cap, n_step, n_start);
	count = 0; 
	allow_convolution_worker_thread = true;
	convolution_buffer_lock = false;
	convolution_worker_thread = std::thread(&Convolver::convolution_worker, this, &convolution_buffer, &convolution_queue);
}
int Convolver::close() {
	allow_convolution_worker_thread = false;
	convolution_worker_thread.join();
	return 0;
}
Convolver::~Convolver() {
	this->close();
} 
a2d::Array2D<pflt>& Convolver::convolve(a2d::Array2D<pflt>& audio_in) {
	for (auto i = 0; i < filters.length; i++) {
		if ((count + 1) % filters(i).buffers_needed != 0) break;
		filters(i).schedule(count);
		convolution_queue.push(&filters(i));
	}
	const int buffer_pos = buffer_size * (count % longest_filter);
	previous_buffers.vpart(buffer_pos, buffer_pos + buffer_size);
	aset(previous_buffers, audio_in).vreset();
	aset((*first_filter.real).vpart(0, audio_in.rows), audio_in).vreset();
	add_to_convolution_buffer(this->convolution_buffer, first_filter.convolve_rfft(), count, 0);
	get_from_convolution_buffer(this->convolution_buffer, audio_in);
	count = (count + 1) % convolution_buffer_blength;
	while (!convolution_queue.empty()&&!realtime) {}
	return audio_in;
}