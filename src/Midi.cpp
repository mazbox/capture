#include "Midi.h"

void* MidiIn::midiIn = NULL;

void MyReadProc(double deltatime, std::vector< unsigned char > *message, void *userData) {
	if(message->size()==3) {
		((MidiIn*)MidiIn::midiIn)->midiReceived(message->at(1), message->at(2));
	}
}


MidiIn::MidiIn() {
	midiIn = this;
	RtMidiIn *midiin = new RtMidiIn("Capture Midi In");

	// Check available ports.
	unsigned int nPorts = midiin->getPortCount();
	if ( nPorts == 0 ) {
		std::cout << "No ports available!\n";
	} else {
		std::cout << "midi started\n";
		printf("%d midi ports to listen to\n", nPorts);
    
		midiin->openPort(0);

		// Set our callback function.  This should be done immediately after
		// opening the port to avoid having incoming messages written to the
		// queue.
		midiin->setCallback( &MyReadProc );

		// ignore sysex, timing, and active sensing messages.
		midiin->ignoreTypes( true, true, true );
	
		// open a midi port for each other input
		// so we can listen on every port
		if(nPorts>1) {
			ancilliaries = new RtMidiIn*[nPorts-1];
			for(unsigned int i = 1; i < nPorts; i++) {
				ancilliaries[i-1] = new RtMidiIn("Other Capture Midi In");
				ancilliaries[i-1]->openPort(i);
				ancilliaries[i-1]->setCallback(&MyReadProc);
				ancilliaries[i-1]->ignoreTypes(true, true, true);
			
			}
		}
	}
}



MidiIn::~MidiIn() {
	delete midiin;
}