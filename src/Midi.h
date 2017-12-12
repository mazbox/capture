#ifndef NULL
#define NULL 0
#endif

#include "RtMidi.h"
class MidiIn {
	
public:
	static void* midiIn;
	MidiIn();
	~MidiIn();
	virtual void midiReceived(int note, int velocity) = 0;
private:
	RtMidiIn *midiin;
	RtMidiIn **ancilliaries;
		
};
