/*
 *  Sampler.h
 *  openFrameworks
 *
 *  Created by Marek Bereza on 18/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */
// maximum sample length in samples
#define MAX_RECORD_TIME 882000
#include "Detector.h"
#include <map>
#include <vector>
#include "Midi.h"
#include "Sample.h"
#include "portaudio.h"
#ifndef SAMPLER
#define SAMPLER

// portaudio stuff
#define SAMPLERATE 44100
#define BUFFER_SIZE 256
static int gNumNoInputs = 0;




class Sampler: public MidiIn {
	public:
		Sampler();
	~Sampler() { samples.clear(); stopAudio(); };
		void audioRequested(float * output, int bufferSize, int nChannels);
		void audioReceived(float * input, int bufferSize, int nChannels);
		void notePressed(int note, int velocity);
		void noteReleased(int note);
		void setRecordMode(bool _recordMode) {
			recordMode = _recordMode;
		};	
	
		bool getRecordMode() {
			return recordMode;
		};
		void toggleRecordMode() {
			recordMode ^= true;
		};

		void midiReceived(int note, int velocity);
	
		/** See SamplerFileIO.cpp for implementation */
		bool save(string name);
		bool load(string name);
		vector<string> getPresets();
	float getVolume() {
		return volume;
	};
	void setAutoTrigger(bool _t) {
		autoTrigger = _t;
	};
	
	bool getAutoTrigger() {
		return autoTrigger;
	};
	void toggleAutoTrigger() {
		autoTrigger ^= true;
	};
	
	void setTriggerLevel(float _l) {
		triggerLevel = _l;
	};
	
	float getTriggerLevel() {
		return triggerLevel;
	};
	bool getRecording() {
		return recording;
	};
	
	bool getLegatoMode() {
		return legatoMode;
	};
	
	void setLegatoMode(bool _leg) {
		legatoMode = _leg;
	};
	
	bool startAudio();
	bool stopAudio();
	
	PaStream *stream;
	
	private:
		bool saveGig(string file);
		bool loadGig(string file);
		bool saveDLS(string file);
		bool loadDLS(string file);
		
		bool legatoMode;
		bool recordMode;
		bool recording;
		Detector detector;
		Sample *sample;
		float volume;
		float recordBuffer[MAX_RECORD_TIME];
		int recordPos;
		map<int, Sample*> samples;
	int nextNote; // the note to save to when autotriggering
	bool autoTrigger;
	float triggerLevel;
};


static int audioCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData );
                         
                         
#endif
