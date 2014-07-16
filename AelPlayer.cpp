#include "AelPlayer.h"

namespace Ael{


	int tick(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* dataPointer){


		Ael::AelPlayer* player = reinterpret_cast<AelPlayer*>(dataPointer);
        int* out = reinterpret_cast<int*>(outputBuffer);

		player->threadptr->join();
		memcpy(outputBuffer, player->frames, player->bufferFrames * player->channels * sizeof(int));
		delete player->threadptr; player->threadptr = NULL;
		player->threadptr = new thread(player->tick, player);

		/*if (player->status == PLAYING) return 0;
		else if (player->status == PAUSED) return 1;
		else  return 2;
        */
        
        return 0;
	}



	AelPlayer::AelPlayer(int n_channels, float samplerate, int bufferFrames) : mixerptr(new AelMixer), threadptr(NULL), frames(new int[bufferFrames * 2]), sampleRate(samplerate), channels(n_channels),status(STOPPED), bufferFrames(bufferFrames){

		
	}
    
    void AelPlayer::openStream(){
        
        RtAudio::StreamParameters parameters;
		parameters.deviceId = dac.getDefaultOutputDevice();
		parameters.nChannels = channels;
        
		RtAudioFormat format = RTAUDIO_SINT32;
        
		try{
			dac.openStream(&parameters, NULL, format, (unsigned int)sampleRate, (unsigned int*)&bufferFrames, Ael::tick, (void*) this);
		}
		catch (RtAudioError &error){
			error.printMessage();
		}
        
        
    }

	void AelPlayer::start(){
        
        //static int firstTimePlaying = 1;
        
		if (status == STOPPED){
            threadptr = new thread(this->tick, this);
            openStream();
		}

		try{
			dac.startStream();
            status = PLAYING;
		}
		catch (RtAudioError &error){
			error.printMessage();
		}

	}


	void AelPlayer::pause(){
        
        if(status != PLAYING)
            return;
        
		try{
			if (dac.isStreamOpen())
				dac.stopStream();
            status = PAUSED;
		}
		catch (RtAudioError &error){
			error.printMessage();
		}
	}

	void AelPlayer::stop(){
        
        if(status != STOPPED)
            dac.closeStream();
        
        if(threadptr != NULL){
            if(threadptr->joinable())
                threadptr->join();
            delete threadptr;
        }

        mixerptr->setPosFrames(0);
        status = STOPPED;
	}

	void AelPlayer::tick(AelPlayer* player){

		int* initframes = player->frames;

		for (unsigned int i = 0; i < player->bufferFrames; i++){
            Ael::AelFrame aux = player->mixerptr->getNextFrame();
			for (int j = 0; j < player->channels; j++){
				*initframes++ = aux[j];
			}
		}
        
	}

	AelPlayer::~AelPlayer(){
		delete [] frames;
		delete mixerptr;
		//delete threadptr;
		dac.closeStream();

	}
}
