// portaudio_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <portaudio.h>
#include "a2d.h"
#include "compressor.h"
#include "convolver.h"
const auto sample_format = paFloat32;
Convolver conv("longverb.wav");

a2d::Array2Dp<float> in(buffer_size, channels, NULL);
a2d::Array2Dp<float> out(buffer_size, channels, NULL);

int main2();
int callback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData); 

int callback(const void* inputBuffer, void* outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData) {
    in.update_ptr((float*)inputBuffer); // constant is cast away, do not write to in
    out.update_ptr((float*)outputBuffer);
    a2d::aset(out.vpart(0, -1, 0, 1), in.vpart(0, -1, 0, 1));
    a2d::aset(out.vpart(0, -1, 1, 2), in.vpart(0, -1, 0, 1)).vreset();
    conv.convolve(out);
    return paContinue;
}

int main2()
{
    PaStreamParameters inputParameters, outputParameters;
    PaStream* stream;
    PaError err = Pa_Initialize();
    if (err != paNoError) { 
        std::cout << "yo1\n";
        goto error; 
    }
    
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) { 
        std::cout << "input param fucked\n";
        goto error; 
    }
    inputParameters.channelCount = channels;
    inputParameters.sampleFormat = sample_format;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
   
    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    inputParameters.device = Pa_GetDefaultInputDevice();
    if (inputParameters.device == paNoDevice) {
        std::cout << "output param fucked\n";
        goto error;
    }    
    outputParameters.channelCount = channels;
    outputParameters.sampleFormat = sample_format;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, fs, buffer_size, 0, callback, NULL);
    if (err != paNoError) {
        std::cout << "yo2\n";
        goto error;
    }
    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cout << "yo2\n";
        goto error;
    }
    printf("Hit ENTER to stop program.\n");
    getchar();
    err = Pa_CloseStream(stream); 
    if (err != paNoError) goto error;
    Pa_Terminate();
    return 0;

error:
    Pa_Terminate();
    std::cout << "something bad happened :(\n";
    std::cout << Pa_GetErrorText(err);
    return -1;
}