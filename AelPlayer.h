#ifndef __AEL_PLAYER__
#define __AEL_PLAYER__

#include "AelUtilities.h"
#include "AelMixer.h"
#include "RtAudio.h"
#include <thread>

using namespace std;

namespace Ael{

	class AelPlayer {
		
	public:
		
		friend int tick(void*, void*, unsigned int, double, RtAudioStreamStatus, void*);
		enum STATUSPLAYER { PLAYING, PAUSED, STOPPED };
		
		AelPlayer(int n_channels=2, float samplerate=44100.0, int bufferFrames=512);
		AelPlayer(AelMixer*);
		void start();
		void pause();
		void stop();
		AelMixer* getMixer(){ return mixerptr; }
		static void tick(AelPlayer* player);
		STATUSPLAYER getStatus() { return status; }
		~AelPlayer();
		
		
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
