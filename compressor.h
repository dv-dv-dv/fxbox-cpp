#pragma once
#include <iostream>
#include <cmath>
#include "globals.h"
#include "a2d.h"
#ifndef COMPRESS_HPP
class Compressor {
private:
	static constexpr pflt k = 0.1660964047443681; // = log2(10)/20
	static constexpr pflt very_small = 0.00000000000001;
	pflt gcp1, gcp2, gcp3, gcp4, atk, rel, atk1, rel1, ldpo;
	a2d::Array2Ds<pflt, buffer_size> audio_level1;
	a2d::Array2Ds<pflt, buffer_size> audio_level2;
	void compute_params();
	void gain_computer(pflt& audio_level);
	void level_detector(pflt& audio_level);
	void linear_to_db(pflt& audio_level);
	void db_to_linear(pflt& audio_level);
	void compute_gain(a2d::Array2D<pflt>& audio_in);
public:
	pflt thresh, ratio, knee, pre, post, attack, release;
	Compressor(pflt thresh, pflt ratio, pflt knee, pflt pre, pflt post, pflt attack, pflt release);
	a2d::Array2D<pflt>& compress(a2d::Array2D<pflt>& audio_in);
};
#endif