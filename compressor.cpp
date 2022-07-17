#pragma once
#include "compressor.h"
void Compressor::compute_params() {
	gcp1 = (1 / ratio - 1) / (2 * knee + very_small);
	gcp2 = -thresh + knee / 2;
	gcp3 = (1 - 1 / (ratio + very_small)) * thresh;
	gcp4 = 1 / (ratio + very_small);
	atk = exp(-1 / (attack * sampfreq + very_small));
	rel = exp(-1 / (release * sampfreq + very_small));
	atk1 = 1 - atk;
	rel1 = 1 - rel;
}
void Compressor::gain_computer(pflt& audio_level) {
	const pflt test = 2 * (audio_level - thresh);
	if (test < -knee) { }
	else if (abs(test) <= knee) { 
		audio_level += gcp1 * pow(audio_level + gcp2, 2);
	}
	else if (test > knee) {
		audio_level = gcp3 + audio_level * gcp4;
	}
}
// level detector - produce a smooth representation of a signal's loudness
void Compressor::level_detector(pflt& audio_level) {
	switch (signbit(audio_level - ldpo)) {
	case 0:
		audio_level = atk * ldpo + atk1 * audio_level;
		break;
	case 1:
		audio_level = rel * ldpo + rel1 * audio_level;
		break;
	}
	ldpo = audio_level;
}
// linear to db - convert a linear signal to log domain, specifically dB
// input - reference to a2d array that will be processed and modified
// output - reference to that array
void Compressor::linear_to_db(pflt& audio_level) {
	audio_level =  log2(abs(audio_level + very_small));
}
// db to linear - convert a db signal to linear
void Compressor::db_to_linear(pflt& audio_level) {
	audio_level = pow(2, audio_level);
}
//a2d::Array2D<pflt>& Compressor::convert_to_mono() {
//	
//}
// compute gain - given
void Compressor::compute_gain(a2d::Array2D<pflt>& audio_in) {
	using namespace a2d;
	for (auto i = 0; i < buffer_size; i++) {
		audio_level1(i) = (audio_in(i, 0) + audio_in(i, 1))/2;
		linear_to_db(audio_level1(i));
		gain_computer(audio_level2(i) = audio_level1(i) + pre);
		level_detector(audio_level1(i) -= audio_level2(i));
		db_to_linear(audio_level1(i) = post - audio_level1(i));
	}
}
Compressor::Compressor(pflt thresh, pflt ratio, pflt knee, pflt pre, pflt post, pflt attack, pflt release) {
	this->thresh = k * thresh;
	this->ratio = ratio;
	this->knee = k * knee;
	this->pre = k * pre;
	this->post = k * post;
	this->attack = attack;
	this->release = release;
	compute_params();
}
a2d::Array2D<pflt>& Compressor::compress(a2d::Array2D<pflt>& audio_in) {
	using namespace a2d;
	compute_gain(audio_in);
	for (auto i = 0; i < buffer_size; i++) {
		audio_in(i, 0) = audio_level1(i);
		audio_in(i, 1) = audio_level1(i);
	}
	return audio_in;
}