#ifndef __AEL_EFFECTS__
#define __AEL_EFFECTS__

#include <string>
#include "AelAudioBuf.h"

using namespace std;

//////////////////////////////////////////////
// AUDIO EFFECTS

namespace Ael {

	class AelEffect{
	protected:
		static int ID;   //tem que ser inicializada em algum lado! (?)
		const int effectId;
		string effectName;
		int sampleRate;
		double wet_level;
        int n_channels;
		bool onoff;
		
	public:
		AelEffect(int n_ch=2, int sampleR = 44100) : n_channels(n_ch), sampleRate(sampleR), effectId(ID++), onoff(true), wet_level(1.0) {}
		int getId() { return effectId; }
		void setSampleRate(int sr) { sampleRate = sr; } // melhorar atribuicao
		int getSampleRate() { return sampleRate; }
        int getNChannels() {return n_channels; }
		virtual AelFrame& processFrame(AelFrame&) = 0; // retorna a propria frame processada
		virtual AelAudioStream& processStream(AelAudioStream &); // retorna a propria frame processada
		bool isOn() { return onoff; }
		void m_turnOn(){ onoff = true; }
		void m_turnOff() { onoff = false; }
		void setWetLevel(double);
		double getWetLevel() { return wet_level; }
		virtual ~AelEffect() { }
	};
}

#endif