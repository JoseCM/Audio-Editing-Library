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
		string effectName;
		int sampleRate;
		MOD mod;
		//CALOU TOU A TRABALHAR FODASSE! QUE DESRESPEITO BRUTAL PARA AS MINHAS 4h DE TRABALHO INTENSIVO DE ESCRITA.
		//IDE PARA AQUI IDE PO CARALHO!!
	public:
		AelEffect(string name, int sampleR) : effectName(name),sampleRate(sampleR) {}
		string getEffectName() { return effectName; }
		void setSampleRate(int sr) { sampleRate = sr; } // melhorar atribuicao
		int getSampleRate() { return sampleRate; }
		virtual AelFrame& processFrame(AelFrame&) = 0; // Nao altera original
		MOD getMod() { return mod; }
		void m_turnOn(){ mod = ON; }
		void m_turnOff() { mod = OFF; }
	};


}

#endif