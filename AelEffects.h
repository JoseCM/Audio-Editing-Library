#ifndef __AEL_EFFECTS__
#define __AEL_EFFECTS__

#include "AelAudioBuf.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////
// Classe AelEffect
// Representa uma interface (classe base) para todos os efeitos criados
// Variáveis-Membro:
//   ID(static int), effectId(const int), effectName(string), onoff(bool)
//   sampleRate (int), wet_level (double), n_channels(int)
// Funções-Membro:
//   1 Construtores
//   Métodos que retornam estado das variáveis membro
//   Método que processa uma stream completa
//   Método puramente virtual (processFrame) que processa uma frame
//   Método puramente virtual (getCopy) que retorna uma cópia do efeito
//   Métodos relativos ao estado do efeito (ON/OFF)
//   Métodos realativos ao estado wet/level do efeito
/////////////////////////////////////////////////////////////////////////

namespace Ael {

	class AelEffect{
	protected:
		static int ID;
		const int effectId;
		string effectName;
		int sampleRate;
		double wet_level;
        int n_channels;
		bool onoff;
		
	public:
		AelEffect(int n_ch=2, int sampleR = 44100) : n_channels(n_ch), sampleRate(sampleR), effectId(ID++), onoff(true), wet_level(1.0) {}
		int getId() { return effectId; }
		void setSampleRate(int sr) { sampleRate = sr; }
		int getSampleRate() { return sampleRate; }
        int getNChannels() {return n_channels; }
		virtual AelFrame& processFrame(AelFrame&) = 0;
		virtual AelAudioStream& processStream(AelAudioStream &);
        virtual AelEffect* getCopy() = 0;
		bool isOn() { return onoff; }
		void m_turnOn(){ onoff = true; }
		void m_turnOff() { onoff = false; }
		void setWetLevel(double);
		double getWetLevel() { return wet_level; }
		virtual ~AelEffect() { }
	};
}

#endif