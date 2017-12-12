/*
 *  Sampler.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 18/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Sampler.h"

Sampler::Sampler() {
	volume = 0;
	recordPos = 0;
	nextNote = -1;
	triggerLevel = 0.02f;
	autoTrigger = false;
	recordMode = false;
	recording = false;
	legatoMode = true;
	startAudio();
}

/**
 * AUDIO OUT
 */
void Sampler::audioRequested(float * output, int bufferSize, int nChannels) {
	
		for(int i = 0; i < bufferSize*nChannels; i++) {
			output[i] = 0.f;
		}
		
		map<int,Sample*>::iterator it;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			sample = (*it).second;
			if(recordMode) {
				if(sample->playing) sample->stop();
			} else if(sample->playing) {
				sample->addSamples(output, bufferSize);
			}
		}
	
}

/**
 * AUDIO IN
 */
void Sampler::audioReceived(float * input, int bufferSize, int nChannels) {
	if(&detector!=NULL && input!=NULL) {
	
		volume = detector.process(input, bufferSize);
		if(autoTrigger && recordMode) {
				if(!recording && volume>triggerLevel) {
				recording = true;
			} else if(recording && volume<triggerLevel) {
				if(nextNote>-1) samples[nextNote] = new Sample(recordBuffer, recordPos, this); 
				recordPos = 0;
				recording = false;
			}
		}
		
		if(recording) {
			for(int i = recordPos; i < recordPos + bufferSize && i < MAX_RECORD_TIME; i++) {
				recordBuffer[i] = input[i-recordPos];
			}
			recordPos += bufferSize;
		}
	}
}



/**
 * This should be cleverer, but for now, if you receive a note with a non-zero
 * velocity it's a note on, and if it's a zero velocity then it's a note-off.
 */
void Sampler::midiReceived(int note, int velocity) {
	//printf("Note: %d %d\n", note, velocity);
	if(velocity>0) notePressed(note, velocity);
	else noteReleased(note);
}

/**
 * Happens when a note is pressed by either midi or the computer keyboard.
 */
void Sampler::notePressed(int note, int velocity) {
	if(recordMode==false) {
		// play sounds
		// show content:
		map<int,Sample*>::iterator it;
		for ( it=samples.begin() ; it != samples.end(); it++ )
			if((*it).first==note) {
				(*it).second->trigger(((float)velocity)/127.f);
			}
	} else {
		if(!autoTrigger) {
			recording = true;
		}
	}
	nextNote = note;
}

void Sampler::noteReleased(int note) {
	if(recording && !autoTrigger) {
		samples[note] = new Sample(recordBuffer, recordPos, this); 
		recordPos = 0;
		recording = false;
	}
	if(!recordMode) {
		map<int,Sample*>::iterator it;
		for ( it=samples.begin() ; it != samples.end(); it++ )
			if((*it).first==note) {
				samples[note]->noteOff();
			}
	}
}









/*****************************************   AUDIO IO   *******************************************/





// audio stuff
bool Sampler::startAudio() {
	PaStreamParameters inputParameters, outputParameters;
	PaError err;

	err = Pa_Initialize();
	if( err != paNoError ) goto error;


	inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
	inputParameters.channelCount = 2;       /* stereo input */
	inputParameters.sampleFormat = paFloat32;
	inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;

	outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32;
	outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	err = Pa_OpenStream(&stream, &inputParameters, &outputParameters, SAMPLERATE, BUFFER_SIZE, 0, audioCallback, this);
	if( err != paNoError ) goto error;

	err = Pa_StartStream( stream );
	if( err != paNoError ) goto error;
	printf("Portaudio started without errors\n");
	return true;

error:
	Pa_Terminate();
	fprintf( stderr, "An error occured while using the portaudio stream\n" );
	fprintf( stderr, "Error number: %d\n", err );
	fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
	return false;
    
}   

bool Sampler::stopAudio() {
	PaError err = Pa_CloseStream( stream );
	Pa_Terminate();
	
}

float mono[BUFFER_SIZE];
static int audioCallback( const void *inputBuffer, void *outputBuffer,
                         unsigned long framesPerBuffer,
                         const PaStreamCallbackTimeInfo* timeInfo,
                         PaStreamCallbackFlags statusFlags,
                         void *userData )
{
	Sampler *sampler = (Sampler*) userData;
	
	const float *in = (const float*)inputBuffer;
    float *out = (float*)outputBuffer;
    
	// need to mono-ize input 
    for(int i = 0; i < framesPerBuffer; i++) {
    	mono[i] = *in++; // save the left channel
    	*in++; // move past right channel
    }
	sampler->audioReceived(mono, framesPerBuffer, 1);
	sampler->audioRequested(mono, framesPerBuffer, 1);
    
    // and stereoize output here
    
   

    if( inputBuffer == NULL ) {
        for(int i = 0; i<framesPerBuffer; i++ ) {
            *out++ = 0;  /* left - silent */
            *out++ = 0;  /* right - silent */
        }
        gNumNoInputs += 1;
    } else {
        for(int i = 0; i<framesPerBuffer; i++ ) {
        	
            *out++ = mono[i];  /* left - distorted */
            *out++ = mono[i];

        }
    }
    
    return paContinue;
}

