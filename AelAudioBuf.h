#ifndef __AEL_AUDIO_STREAM__
#define __AEL_AUDIO_STREAM__

#pragma once

#include "AelUtilities.h"


using namespace std;

namespace Ael {

/////////////////////////////////////////////////////////////////////////
// Classe AelFrame
// Representa a estrutura básica de uma Stream de Audio (Frame)
// Variáveis-Membro:
//    n_channels (int*), samples (int*)
// Funções-Membro:
//    3 Construtores 
//    Métodos que retornam número de canais, e máxima amostra (módulo)
//    Método que converte Frame mono (1 canal) numa stereo (2 canais)
//    Overload dos operadores [], +, -, =, *
/////////////////////////////////////////////////////////////////////////

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
		
		friend class AelAudioStream;    //Classe AelAudioStream pode aceder
										//aos membros privados da classe AelFrame
	};

/////////////////////////////////////////////////////////////////////////////
// Classe AelAudioStream
// Representa uma stream de áudio
// Variáveis-Membro:
//   ID(static int), audioFstream (fstream), streamID (int), m_nframes (int)
//   currPosition(int), channels(int), peak(int), sampleRate(int), eos(bool)
// Funções-Membro:
//   3 Construtores
//   Métodos que retornam estado das variáveis membro
//   Métodos que adicionam e retornam a próxima frame da stream
//   Métodos de posicionamento no ficheiro que represente a stream
//   Método que grava stream de áudio num ficheiro.wav
//   Métodos que grava e remove stream de áudio num SD card (usado no 8051)
////////////////////////////////////////////////////////////////////////////
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
		bool SaveTo8051(string FileName);
		bool RemoveFrom8051(string FileName);
		
		
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

