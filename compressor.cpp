#pragma once
#include "compressor.h"
int Compressor::compute_params() {
	gcp1 = (1 / ratio - 1) / (2 * knee + very_small);
	gcp2 = -thresh + knee / 2;
	gcp3 = (1 - 1 / (ratio + very_small)) * thresh;
	gcp4 = 1 / (ratio + very_small);
	atk = exp(-1 / (attack * fs + very_small));
	rel = exp(-1 / (release * fs + very_small));
	atk1 = 1 - atk;
	rel1 = 1 - rel;
	return 0;
}
a2d::Array2D<pflt>& Compressor::gain_computer(a2d::Array2D<pflt>& audio_level) {
	pflt test;
	for (auto i = 0; i < buffer_size; i++) {
		test = 2 * (audio_level(i) - thresh);
		if (test < -knee) { break; }
		else if (abs(test) <= knee) { 
			audio_level(i) += gcp1 * pow(audio_level(i) + gcp2, 2);
		}
		else if (test > knee) {
			audio_level(i) = gcp3 + audio_level(i) * gcp4;
		}
	}
	return audio_level;
}
// level detector - produce a smooth representation of a signal's loudness
// inputs - reference to a2d array that represents a signal's loudness
// outputs - reference to that same array
a2d::Array2D<pflt>& Compressor::level_detector(a2d::Array2D<pflt>& audio_level) {
	for (auto i = 0; i < buffer_size; i++) {
		if (audio_level(i) > ldpo) {
			audio_level(i) = atk * ldpo + atk1 * audio_level(i);
		}
		else {
			audio_level(i) = rel * ldpo + rel1 * audio_level(i);
		}
		ldpo = audio_level(i);
	}
	return audio_level;
}
// linear to db - convert a linear signal to log domain, specifically dB
// input - reference to a2d array that will be processed and modified
// output - reference to that array
a2d::Array2D<pflt>& Compressor::linear_to_db(a2d::Array2D<pflt>& audio_level) {
	for (auto i = 0; i < buffer_size; i++) {
		audio_level(i) = 20 * log10(abs(audio_level(i) + very_small));
	}
	return audio_level;
}
// db to linear - convert a db signal to linear
// input - reference to a2d array that will be processed
// output - the same reference to that array
a2d::Array2D<pflt>& Compressor::db_to_linear(a2d::Array2D<pflt>& audio_level) {
	for (auto i = 0; i < buffer_size; i++) {
		audio_level(i) = pow(10, audio_level(i) / 20);
	}
	return audio_level;
}
//a2d::Array2D<pflt>& Compressor::convert_to_mono() {
//	
//}
// compute gain - given
a2d::Array2D<pflt>& Compressor::compute_gain(a2d::Array2D<pflt>& audio_in) {
	using namespace a2d;
	kmult(aadd(aset(audio_level1, audio_in.vpart(0, -1, 0, 1)), audio_in.vpart(0, -1, 1, 2)), 0.5);
	linear_to_db(audio_level1); // actual dB value
	acopy(audio_level2, audio_level1);
	gain_computer(audio_level2); // desired dB value
	aadd(audio_level1, audio_level2, -1);
	level_detector(audio_level1); // smooth difference
	kadd(kmult(audio_level1, -1), post);
	db_to_linear(audio_level1);
	return audio_level1;
}
Compressor::Compressor(pflt athresh, pflt aratio, pflt aknee, pflt apre, pflt apost, pflt aattack, pflt arelease) {
	thresh = athresh;
	ratio = aratio;
	knee = aknee;
	pre = apre;
	post = apost;
	attack = aattack;
	release = arelease;
	compute_params();
}
a2d::Array2D<pflt>& Compressor::compress(a2d::Array2D<pflt>& audio_in) {
	using namespace a2d;
	compute_gain(audio_in);
	amult(audio_in.vpart(0, -1, 0, 1), audio_level1);
	amult(audio_in.vpart(0, -1, 1, 2), audio_level1).vreset();
	return audio_in;
}