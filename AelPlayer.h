#ifndef __AEL_PLAYER__
#define __AEL_PLAYER__

#include "AelUtilities.h"
#include "AelMixer.h"
#include "RtAudio.h"
#include <thread>

using namespace std;

namespace Ael{


	enum STATUSPLAYER { PLAYING, PAUSED, STOPPED };

	class AelPlayer {
	public:
		AelPlayer(int n_channels=2, float samplerate=44100.0, int bufferFrames=512);
		AelPlayer(AelMixer*);
		void start();
		void pause();
		void stop();
		AelMixer* getMixer(){ return mixerptr; }
		static void tick(AelPlayer* player);
		~AelPlayer();
		
		friend int tick(void*, void*, unsigned int, double, RtAudioStreamStatus, void*);
		
	private:
		AelMixer* mixerptr;
		RtAudio dac;
		thread* threadptr;
		int* frames;
		int channels;
        float sampleRate;
		int  bufferFrames;
		STATUSPLAYER status;
        
        void openStream();
		
	};




}





#endif