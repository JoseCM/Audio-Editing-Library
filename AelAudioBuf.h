#ifndef __AEL_AUDIO_STREAM__
#define __AEL_AUDIO_STREAM__

#pragma once

#include <sndfile.hh>
#include <iostream>
#include <stdexcept>

using namespace std;

namespace Ael {


	class AelFrame {

	private:
		int* samples;
		int n_channels;

	public:
		AelFrame(int);
		AelFrame(int*, int);
		int& operator[](int);
		~AelFrame();
	};

//////////////////////////////////////////////
// AUDIO BUFFER 

	class AelAudioStream
	{
		int *m_panStream;
		int *currPosition;
		int m_nframes;
		char channels;
		int peek;
		int sampleRate;

	public:
		AelAudioStream(string);
		AelAudioStream(int nChannels, int nFrames, int nSampleRate = 44100);
		int getframes() { return m_nframes; }
		int getchannels(){ return channels; }
		int getpeek() { return peek; }
		int getsampleRate() { return sampleRate; }
		AelFrame& getNextFrame();
		void SaveToFile(string);
		virtual ~AelAudioStream();
		
	};


}

#endif

