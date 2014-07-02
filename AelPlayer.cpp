#include "AelPlayer.h"

namespace Ael{


	int tick(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* dataPointer){


		Ael::AelPlayer* player = reinterpret_cast<AelPlayer*>(dataPointer);
		register int* out = reinterpret_cast<int*>(outputBuffer);

		//cout << counter++ << endl;
		player->threadptr->join();
		memcpy(outputBuffer, player->frames, player->bufferFrames);
		delete player->threadptr;
		player->threadptr = new thread(player->tick, player);
		//cout << "end" << counter << endl;
		

		if (player->status == PLAYING) return 0;
		if (player->status == PAUSED) return 1;
		if (player->status == STOPPED) return 2;
	}



	AelPlayer::AelPlayer(int n_channels, float samplerate, int bufferFrames) : mixerptr(new AelMixer), threadptr(NULL), frames(NULL), channels(n_channels),status(STOPPED){

		RtAudio::StreamParameters parameters;
		parameters.deviceId = dac.getDefaultOutputDevice();
		parameters.nChannels = n_channels;

		RtAudioFormat format = RTAUDIO_SINT32;

		try{
			dac.openStream(&parameters, NULL, format, (unsigned int)samplerate, (unsigned int*)&bufferFrames, &Ael::tick, (void*) this);
			frames = new int[bufferFrames];
			this->bufferFrames = bufferFrames;
		}
		catch (RtAudioError &error){
			error.printMessage();
		}
		
	}

	void AelPlayer::start(){
		static int firstTimePlaying = 1;
		if (firstTimePlaying){
			threadptr = new thread(this->tick, this);
			firstTimePlaying = 0;
		}

		threadptr->join();

		try{
			dac.startStream();
		}
		catch (RtAudioError &error){
			error.printMessage();
		}

	}


	void AelPlayer::pause(){
		try{
			if (dac.isStreamOpen())
				dac.stopStream();
		}
		catch (RtAudioError &error){
			error.printMessage();
		}
	}

	void AelPlayer::stop(){
		dac.closeStream();
	}

	void AelPlayer::tick(AelPlayer* player){

		int* initframes = player->frames;

		for (register unsigned int i = 0; i < player->bufferFrames; i++){
			for (int j = 0; j < player->channels; j++){
				Ael::AelFrame aux = player->mixerptr->getNextFrame();
				*initframes++ = aux[j];
			}

		}

	}

	AelPlayer::~AelPlayer(){
		delete [] frames;
		delete mixerptr;
		delete threadptr;
		dac.closeStream();

	}
}