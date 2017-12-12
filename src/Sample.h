/*
 *  Sample.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */
#include <stdio.h>
#include <string>

#ifndef SAMPLE
#define SAMPLE
using namespace std;
class Sampler;
class Sample {
	
public:
	
	Sample(float* data, int length, Sampler *_sampler);
	~Sample() {
		delete [] data;
	};
	void trigger(float _volume);
	void noteOff();
	void getSamples(float* out, int length);
	void addSamples(float* out, int length);
	void stop();
	bool playing;
	Sampler *sampler;
	
	/** See SamplerFileIO.cpp for implementation */
	//Sample(string path);
	bool save(string path);
	int length;
	float *data;
private:
	int pos;
	
	
	float volume;
	float envelope;
	bool needsRetrigger;
	bool released;
};
#endif

