#ifndef __AEL_AUDIO_STREAM__
#define __AEL_AUDIO_STREAM__

#pragma once

#include "AelUtilities.h"


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
        int operator[](int) const;
        int maxSample() const;
        void toStereo();
        AelFrame operator+(const AelFrame&) const;
        AelFrame operator-(const AelFrame&) const;
        AelFrame& operator=(const AelFrame&) ;
		AelFrame operator*(float) const;
		~AelFrame();
        
        friend class AelAudioStream;
	};

//////////////////////////////////////////////
// AUDIO BUFFER 

    
	class AelAudioStream
	{
        
        static int ID;
		//vector<int> m_panStream;
        fstream audioFstream;
        int streamID;
		int currPosition;
		int m_nframes;  
		int channels;
		int peak;
		int sampleRate;
        bool eos;

	public:
		AelAudioStream(string);
		AelAudioStream(int nChannels, int nSampleRate = 44100);
		AelAudioStream(int nChannels, int nFrames, int nSampleRate = 44100);
        
		bool AddFrames(AelFrame&);
        
		int getnframes() { return m_nframes; }
		int getchannels(){ return channels; }
		int getpeak() { return peak; }
		int getsampleRate() { return sampleRate; }
        
		AelFrame getNextFrame();
        
		void SaveToFile(string);
        
        int getCurrPosition() { return currPosition; }
        void setCurrPosition(int pos);
        void rewind();
        bool isEOS() { return eos; }
        
        //void toStereo();
        //void toMono();
        
		virtual ~AelAudioStream();
		
	};


}

#endif

