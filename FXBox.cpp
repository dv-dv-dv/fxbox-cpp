#include <iostream>
#include <AudioFile.h>
#include <math.h>

// user made files
#include "a2d.h"
#include "globals.h"
#include "compressor.h"
#include "convolver.h"

int main(){
	using namespace a2d;
	AudioFile<pflt> infile;
	AudioFile<pflt> outfile;

	infile.load("guitar_sample16.wav");
	int numSamples = infile.getNumSamplesPerChannel();
	int numChannels = infile.getNumChannels();
	int numBuffers = numSamples / buffer_size;

	AudioFile<pflt>::AudioBuffer buffer;

	Array2Ds<pflt, 2, buffer_size> adata;
	Compressor comp = Compressor(-40, 7, 10, 0, 0, 0.001, 0.5);
	Convolver conv = Convolver();
	std::cout << "buffer size: " << buffer_size << "\n";
	std::cout << "number of buffers: " << numBuffers << "\n";
	buffer.resize(2);
	buffer[0].resize(numBuffers * buffer_size);
	buffer[1].resize(numBuffers * buffer_size);
	for (auto i = 0; i < numBuffers; i++) {
		a2d::kset(adata, 0);
		for (auto j = 0; j < buffer_size; j++) {
			adata(0, j) = infile.samples[0][j + i * buffer_size];
			adata(1, j) = infile.samples[1][j + i * buffer_size];
		}
		comp.compress(adata); 
		for (auto j = 0; j < buffer_size; j++) {
			buffer[0][j + i * buffer_size] = adata(0, j);
			buffer[1][j + i * buffer_size] = adata(1, j);
		}
	}
	if (!outfile.setAudioBuffer(buffer)) { std::cout << "whoops"; }
	outfile.setAudioBufferSize(numChannels, numBuffers * buffer_size);
	outfile.setNumSamplesPerChannel(numBuffers * buffer_size);
	outfile.setNumChannels(numChannels);
	outfile.setBitDepth(16);
	outfile.setSampleRate(44100);
	outfile.save("guitar_sample16_cppout.wav");
}
