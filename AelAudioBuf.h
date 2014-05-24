#ifndef __AEL_AUDIO_STREAM__
#define __AEL_AUDIO_STREAM__

#pragma once

#include <sndfile.hh>
#include <iostream>

//comentario do ze

/*
sample

*/

using namespace std;

namespace Ael {

//////////////////////////////////////////////
// AUDIO EFFECTS
	class AelEffects{
		string effectName;
	public:
		virtual string getEffectName() = 0;
		// int m_turnOn();
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
		void SaveToFile(string);
		virtual ~AelAudioStream();
		
	};


}


#endif

