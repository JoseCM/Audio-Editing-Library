#include "AelPlayer.h"

namespace Ael{

//////////////////////////////////////////////////////////////////
//Classe AelPlayer
//////////////////////////////////////////////////////////////////

    /****************************************************************************
     *Função tick
     *Parâmetros: void*, void*, unsigned int, double, RtAudioStreamStatus, void*
     *Função callback utilizada pela RTAudio
     ****************************************************************************/
	int tick(void* outputBuffer, void* inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* dataPointer){


		Ael::AelPlayer* player = reinterpret_cast<AelPlayer*>(dataPointer);
		int* out = reinterpret_cast<int*>(outputBuffer);

		player->threadptr->join();    //retorna quando thread termina
		memcpy(outputBuffer, player->frames, player->bufferFrames * player->channels * sizeof(int));  //copia novo buffer com novas amostras
		delete player->threadptr; player->threadptr = NULL;   //destroi thread em execução
		player->threadptr = new thread(player->tick, player);  //inicia uma nova thread

		/*if (player->status == PLAYING) return 0;
		else if (player->status == PAUSED) return 1;
		else  return 2;
		*/
		
		return 0;
	}


    /****************************************************************************
     *Construtor da AelPlayer
     *Parâmetros: int, float, int
     ****************************************************************************/
	AelPlayer::AelPlayer(int n_channels, float samplerate, int bufferFrames) : mixerptr(new AelMixer), threadptr(NULL), frames(new int[bufferFrames * 2]), sampleRate(samplerate), channels(n_channels),status(STOPPED), bufferFrames(bufferFrames){

		
	}

    
    /****************************************************************************
     *Função membro openStream (utility function)
     *Parâmetros: void
     *Abre a stream para reprodução com parâmetros pretendidos
     ****************************************************************************/
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

    /****************************************************************************
     *Função membro start
     *Parâmetros: void
     *Inicia/retoma reprodução de audio
     ****************************************************************************/
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

    /****************************************************************************
     *Função membro pause
     *Parâmetros: void
     *Suspende reprodução de áudio
     ****************************************************************************/
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

    /****************************************************************************
     *Função membro top
     *Parâmetros: void
     *Termina reprodução de áudio
     ****************************************************************************/
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
    
    
    /****************************************************************************
     *Função membro tick
     *Parâmetros: void
     *Preenche buffer da classe com amostras da mixer
     ****************************************************************************/
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
