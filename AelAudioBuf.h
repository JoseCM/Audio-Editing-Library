#ifndef __AEL_AUDIO_STREAM__
#define __AEL_AUDIO_STREAM__

#pragma once

#include "sndfile.hh"
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

namespace Ael {


	class AelFrame {

	private:
		int* samples;
		int n_channels;

	public:
		AelFrame(int);
        AelFrame(const AelFrame&);
		AelFrame(int*, int);
		int getChannels();
		int& operator[](int);
        int& operator[](int) const;
		~AelFrame();
	};

//////////////////////////////////////////////
// AUDIO BUFFER 

	class AelAudioStream
	{
		vector<int> m_panStream;
		int currPosition;
		int m_nframes;
		char channels;
		int peek;
		int sampleRate;

	public:
		AelAudioStream(string);
		AelAudioStream(int nChannels, int nSampleRate = 44100);
		AelAudioStream(int nChannels, int nFrames, int nSampleRate = 44100);
		bool AddFrames(AelFrame&);
		int getnframes() { return m_nframes; }
		int getchannels(){ return channels; }
		int getpeek() { return peek; }
		int getsampleRate() { return sampleRate; }
		AelFrame& getNextFrame();
		void SaveToFile(string);
		virtual ~AelAudioStream();
		
	};


}

#endif

