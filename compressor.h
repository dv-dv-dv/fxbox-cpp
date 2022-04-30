#pragma once
#include <iostream>
#include <math.h>
#include "globals.h"
#include "a2d.h"
#ifndef COMPRESS_HPP
class Compressor {
private:
	bool closed = false;
	pflt gcp1, gcp2, gcp3, gcp4, atk, rel, ldpo;
	a2d::Array2Ds<pflt, 1, buffer_size> audio_level1;
	a2d::Array2Ds<pflt, 1, buffer_size> audio_level2;
	int compute_params();
	int gain_computer(a2d::Array2D<pflt>& audio_level);
	int level_detector(a2d::Array2D<pflt>& audio_level);
	int linear_to_log(a2d::Array2D<pflt>& audio_level);
	int log_to_linear(a2d::Array2D<pflt>& audio_level);
	int compute_gain(a2d::Array2D<pflt>& audio_in);
public:
	pflt thresh, ratio, knee, pre, post, attack, release;
	Compressor(pflt athresh, pflt aratio, pflt aknee, pflt apre, pflt apost, pflt aattack, pflt arelease);
	int compress(a2d::Array2D<pflt>& audio_in);
};
#endif