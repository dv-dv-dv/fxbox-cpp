#pragma once
#include "compressor.h"
int Compressor::compute_params() {
	gcp1 = (1 / ratio - 1) / (2 * knee);
	gcp2 = -thresh + knee / 2;
	gcp3 = (1 - 1 / ratio) * thresh;
	gcp4 = 1 / ratio;
	atk = exp(-1 / (attack * fs));
	rel = exp(-1 / (release * fs));
	return 0;
}
int Compressor::gain_computer(a2d::Array2D<pflt>& audio_level) {
	pflt test;
	for (auto i = 0; i < audio_level.c; i++) {
		test = 2 * (audio_level(i) - thresh);
		if (test < -knee) { break; }
		else if (abs(test) <= knee) {
			audio_level(i) += gcp1 * pow(audio_level(i) + gcp2, 2);
		}
		else if (test > knee) {
			audio_level(i) = gcp3 + audio_level(i) * gcp4;
		}
	}
	return 0;
}
int Compressor::level_detector(a2d::Array2D<pflt>& audio_level) {
	for (auto i = 0; i < audio_level.c; i++) {
		if (audio_level(i) > ldpo) {
			audio_level(i) = atk * ldpo + (1 - atk) * audio_level(i);
		}
		else {
			audio_level(i) = rel * ldpo + (1 - rel) * audio_level(i);
		}
		ldpo = audio_level(i);
	}
	return 0;
}
int Compressor::linear_to_log(a2d::Array2D<pflt>& audio_level) {
	for (auto i = 0; i < audio_level.c; i++) {
		if (audio_level(i) == 0) {
			audio_level(i) = -999.9;
		}
		else {
			audio_level(i) = 20 * log10(abs(audio_level(i)));
		}
	}
	return 0;
}
int Compressor::log_to_linear(a2d::Array2D<pflt>& audio_level) {
	for (auto i = 0; i < audio_level.c; i++) {
		audio_level(i) = pow(10, audio_level(i) / 20);
	}
	return 0;
}
int Compressor::compute_gain(a2d::Array2D<pflt>& audio_in) {
	using namespace a2d;
	kmult(aadd(aset(audio_level1, audio_in.vpart(0, 1, 0, -1)), audio_in.vpart(1, 2, 0, -1)), 0.5);
	linear_to_log(audio_level1); // actual dB value
	acopy(audio_level1, audio_level2);
	gain_computer(audio_level2); // desired dB value
	aadd(audio_level1, audio_level2, -1);
	level_detector(audio_level1); // smooth difference
	kadd(kmult(audio_level1, -1), post);
	log_to_linear(audio_level1);
	return 0;
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
int Compressor::compress(a2d::Array2D<pflt>& audio_in) {
	using namespace a2d;
	compute_gain(audio_in);
	amult(audio_in.vpart(0, 1, 0, -1), audio_level1);
	amult(audio_in.vpart(1, 2, 0, -1), audio_level1).vreset();
	return 0;
}