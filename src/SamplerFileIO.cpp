/*
 *  SamplerFileIO.cpp
 *  openFrameworks
 *
 *  Created by Marek Bereza on 18/01/2009.
 *  Copyright 2009 Royal College of Art. All rights reserved.
 *
 */

#include "Sampler.h"
#include "Sample.h"
#include <fstream>
#include <sys/stat.h>
#include <errno.h>

#include <sys/types.h>
#include <dirent.h>
#include <vector>
#include "gig.h"
#ifndef DEBUG
#define DEBUG false
#endif



// reverse:  reverse string s in place 
void reverse(char s[])
{
    int c, i, j;
	
    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}
// itoa:  convert n to characters in s 
void itoa(int n, char s[])
{
    int i, sign;
	
    if ((sign = n) < 0)  // record sign 
        n = -n;          // make n positive 
    i = 0;
    do {       // generate digits in reverse order 
        s[i++] = n % 10 + '0';   // get next digit 
    } while ((n /= 10) > 0);     // delete it 
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
} 




void floatToPCM16(float *in, short int *out, long length) {
	for(long l = 0; l < length; l++) {
		out[l] = (short int) (in[l]*32767.f);
	}
}

void pcm16ToFloat(short int *in, float *out, long length) {
	for(long l = 0; l < length; l++) {
		out[l] = (float) (in[l]*1.f/32767.f);
	}
}

/**
 * Saves the contents of the sampler
 * to a file.
 */
bool Sampler::save(string path) {
	if(path.find(".gig")!=string::npos) {
		return saveGig(path);
	} else {
		if(path.find(".dls")==string::npos) {
			path += ".dls";
		}
		return saveDLS(path);
	}
}



bool Sampler::load(string path) {
	if(path.find(".gig")!=string::npos) {
		return loadGig(path);
	} else {
		if(path.find(".dls")==string::npos) {
			path += ".dls";
		}
		return loadDLS(path);
	}
}




bool Sampler::saveDLS(string path) {
	try {
		
		// where to store it
		//string path = "presets/"+name+".gig";
		
		printf("Saving to %s\n", path.c_str());
		// create a file object
		DLS::File file;
		
		//DLS::Group* group = file.AddGroup();
		//group->Name = "Main";
		
		file.pInfo->Name = "TEST";
		
		// create an instrument
		DLS::Instrument* pInstrument = file.AddInstrument();
		DLS::Sample* pSamples[samples.size()];
		DLS::Region* pRegions[samples.size()];

		pInstrument->IsDrum = false;
		int keys [samples.size()];
		map<int,Sample*>::iterator it;
		int i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			
			int key = (*it).first;
			sample = (*it).second;
			keys[i] = key;
			
			char chars[20];
			
			itoa(i, chars);
			string name = "Sample "+ string(chars);
			
			if(sample->length>0) {
				printf("sample %d is %d long\n", i, sample->length);
				pSamples[i] = file.AddSample();
				pSamples[i]->pInfo->Name = name;
				pSamples[i]->Channels = 1; // mono
				pSamples[i]->BitDepth = 16; // 16 bits
				pSamples[i]->FrameSize = 16/*bitdepth*/ / 8/*1 byte are 8 bits*/ * 1/*mono*/;
				pSamples[i]->SamplesPerSecond = 44100;
				pSamples[i]->Resize(sample->length);
				
				//group->AddSample(pSamples[i]);
				
				pRegions[i] = pInstrument->AddRegion();
				pRegions[i]->SetSample(pSamples[i]);
				pRegions[i]->SetKeyRange(key, key);
				pRegions[i]->UnityNote = key;
				//pRegions[i]->pDimensionRegions[0]->pSample = pSamples[i];
				//pRegions[i]->pDimensionRegions[0]->UnityNote = key;
			}
			i++;
			
		}
		
				
		file.Save(path);
		//i = 0;
		//for ( it=samples.begin() ; it != samples.end(); it++ ) {
		for(i = 0; i < samples.size(); i++) {
			//int key = (*it).first;
			//sample = (*it).second;
			int key = keys[i];
			sample = samples[key];
			if(sample->length>0) {
				printf("again sample %d is %d long\n", i, sample->length);
			
				short int data[sample->length];
				floatToPCM16(sample->data, data, sample->length);
				pSamples[i]->Write(data, sample->length);
			}
		}
		
		return true;
	} catch (RIFF::Exception e) {
		e.PrintMessage();
		return false;
	}
}



bool Sampler::loadDLS(string path) {
	samples.clear();
	printf("Sampler::load called with '%s'\n", path.c_str());
			
	RIFF::File* riff = new RIFF::File(path);
	DLS::File*  DLS  = new DLS::File(riff);

	if(!DLS || DLS->Instruments!=1) {
		printf("invalid dls file\n");
		return false;
	}
	DLS::Instrument *instrument = DLS->GetFirstInstrument();
	if(instrument) {
		DLS::Region* pRegion = instrument->GetFirstRegion();
		while (pRegion) {
			
			DLS::Sample* pSample = pRegion->GetSample();		
			
			short int *sampleData = (short int*)pSample->LoadSampleData();//.pStart;
			long length = pSample->GetSize();
			int key = pRegion->KeyRange.low;
			float fData[length];
			pcm16ToFloat(sampleData, fData, length);
			samples[key] = new Sample(fData, length, this);
	
			printf("Sample for key %d is %ld samples long\n", key, length);
			
			pRegion = instrument->GetNextRegion();
	
		}
		
		delete DLS;
		delete riff;
		
		return true;
	}
	return false;
}












/**
 * Saves the contents of the sampler
 * to a file.
 */
bool Sampler::saveGig(string path) {
	try {
		
		// where to store it
		//string path = "presets/"+name+".gig";
		
		printf("Saving to %s\n", path.c_str());
		// create a file object
		gig::File file;
		
		gig::Group* group = file.AddGroup();
		group->Name = "Main";
		
		file.pInfo->Name = "TEST";
		
		// create an instrument
		gig::Instrument* pInstrument = file.AddInstrument();
		gig::Sample* pSamples[samples.size()];
		gig::Region* pRegions[samples.size()];

		pInstrument->IsDrum = false;
		
		map<int,Sample*>::iterator it;
		int i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			
			int key = (*it).first;
			sample = (*it).second;
			
			
			char chars[20];
			
			itoa(i, chars);
			string name = "Sample "+ string(chars);
			
			pSamples[i] = file.AddSample();
			pSamples[i]->pInfo->Name = name;
			pSamples[i]->Channels = 1; // mono
			pSamples[i]->BitDepth = 16; // 16 bits
			pSamples[i]->FrameSize = 16/*bitdepth*/ / 8/*1 byte are 8 bits*/ * 1/*mono*/;
			pSamples[i]->SamplesPerSecond = 44100;
			pSamples[i]->Resize(sample->length);
			
			group->AddSample(pSamples[i]);
			
			pRegions[i] = pInstrument->AddRegion();
			pRegions[i]->SetSample(pSamples[i]);
			pRegions[i]->SetKeyRange(key, key);
			pRegions[i]->UnityNote = key;
			pRegions[i]->pDimensionRegions[0]->pSample = pSamples[i];
			pRegions[i]->pDimensionRegions[0]->UnityNote = key;
			i++;
		}
		
				
		file.Save(path);
		i = 0;
		for ( it=samples.begin() ; it != samples.end(); it++ ) {
			sample = (*it).second;
			short int data[sample->length];
			floatToPCM16(sample->data, data, sample->length);
			pSamples[i]->Write(data, sample->length);
			i++;
		}
		
		return true;
	} catch (RIFF::Exception e) {
		e.PrintMessage();
		return false;
	}
}


bool Sampler::loadGig(string path) {
	samples.clear();
	printf("Sampler::load called with '%s'\n", path.c_str());
			
	RIFF::File* riff = new RIFF::File(path);
	gig::File*  gig  = new gig::File(riff);

	if(gig->Instruments!=1) {
		printf("invalid giga file\n");
	}
	gig::Instrument *instrument = gig->GetFirstInstrument();
	gig::Region* pRegion = instrument->GetFirstRegion();
    while (pRegion) {
		
		gig::Sample* pSample = pRegion->GetSample();		
		
		short int *sampleData = (short int*)pSample->LoadSampleData().pStart;
		long length = pSample->GetSize();
		int key = pRegion->KeyRange.low;
		float fData[length];
		pcm16ToFloat(sampleData, fData, length);
		samples[key] = new Sample(fData, length, this);

		printf("Sample for key %d is %ld samples long\n", key, length);
		
		pRegion = instrument->GetNextRegion();

	}
	
	delete gig;
	delete riff;
	
	return true;
}
