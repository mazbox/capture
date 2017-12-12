/*
 *  Sample.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 17/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Sample.h"
#include "Sampler.h"
#define ABS(A) (A>0?A:-A)

Sample::Sample(float* _data, int _length, Sampler *_sampler) {
	pos = 0;
	sampler = _sampler;
	playing = false;
	needsRetrigger = false;
	released = false;
	envelope = 1.f;
	length = _length;
	volume = 0.05;

	data = new float[length];
	for(int i = 0; i < length; i++) {
		data[i] = _data[i];
	}
}

void Sample::trigger(float _volume) {
	pos = 0;
	envelope = 1.f;
	
	// if it's already playing and we're in legato mode,
	// wait for a zero crossing to start again
	if(sampler->getLegatoMode()) needsRetrigger = playing;
	
	playing = true;
	
	released = false;
	volume = _volume;
	printf("starting sample\n");
}

void Sample::getSamples(float* out, int _length) {
	int count = 0;
	float vol;

	for(int i = pos; i < _length+pos && i < length; i++) {
		if(released) envelope *= 0.99f;
		vol = envelope*volume;
		if(i<3) {
			out[i-pos] = ((float)(i+1.f)/3.f)*vol*data[i];
		} else if(i+3>length) {
			out[i-pos] = ((float)(length - i)/3.f)*vol*data[i];
		} else {
			out[i-pos] = volume*data[i];
		}
		count++;
	}
	if(envelope<0.001f) stop();
	pos += count;
	// if we've reached the end
	if(pos>=length-1) {
		stop();
		for(int i = count; i < _length; i++) {
			out[i] = 0.f;
		}
	}
}

void Sample::addSamples(float* out, int _length) {
	int count = 0;
	float vol;
	
	for(int i = pos; i < _length+pos && i < length; i++) {
		if(released) envelope *= 0.99f;
		else if(needsRetrigger && ABS(data[i])<0.001f) {
			stop();
			needsRetrigger = false;
			trigger(volume);
		}
		vol = envelope*volume;
		if(i<3) {
			out[i-pos] += ((float)(i+1.f)/3.f)*vol*data[i];
		} else if(i+3>length) {
			out[i-pos] += ((float)(length - i)/3.f)*vol*data[i];
		} else {
			out[i-pos] += vol*data[i];
		}
		count++;
	}
	
	pos += count;
	
	// if we're at the end, stop
	if(pos>=length-1) {
		stop();
	}
	// if the envelope is almost over, stop
	if(envelope<0.001f) stop();
	
	if(needsRetrigger) stop();
}

void Sample::stop() {
	playing = false;
	pos = 0;
}


void Sample::noteOff() {
	if(!sampler->getLegatoMode()) {
		released = true;
		printf("note offed\n");
	}
}