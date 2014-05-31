#ifndef __AEL_EFFECTS__
#define __AEL_EFFECTS__

#include <string>
#include "AelAudioBuf.h"

using namespace std;

//////////////////////////////////////////////
// AUDIO EFFECTS
enum MOD { OFF, ON };

namespace Ael {

	class AelEffect{
	private:
        static int ID;   //tem que ser inicializada em algum lado! (?)
        const int effectId;
		string effectName;
		int sampleRate;
		MOD mod;
		
	public:
		AelEffect(int sampleR = 44100) : sampleRate(sampleR) , effectId(ID++) {}
		int getId() { return effectId; }
		void setSampleRate(int sr) { sampleRate = sr; } // melhorar atribuicao
		int getSampleRate() { return sampleRate; }
		virtual AelFrame& processFrame(AelFrame&) = 0; // retorna a propria frame processada
        virtual AelAudioStream& processStream(AelAudioStream &) = 0; // retorna a propria frame processada
		MOD getMod() { return mod; }
		void m_turnOn(){ mod = ON; }
		void m_turnOff() { mod = OFF; }
	};


}

#endif