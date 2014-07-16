#ifndef __DELAYS__
#define __DELAYS__

#include "AelEffects.h"

namespace Ael {
	
	
///////////////////////////////////////////////////////////////////////////////
// Classe AelDelayLine
// Representa a interface (classe Base) de uma estrutura de
//   dados auxiliar com as frames atrasadas no tempo
// Variáveis-Membro:
//   sampleRate(int), maxDelayLen(int), channels(int), delay(vector<AelFrames>)
// Funções-Membro:
//   1 Construtores
//   Métodos de escrita/leitura na delay line (puramente virtuais)
//////////////////////////////////////////////////////////////////////////////
	
	class AelDelayLine
	{
	public:
		AelDelayLine(float delayTime, float sampleRate = 44100, int n_ch = 2) : maxDelayLen(delayTime*sampleRate + 1), sampleRate(sampleRate), channels(n_ch), delay(maxDelayLen, AelFrame(n_ch)) { }   // time in seconds
		virtual bool write(AelFrame&) = 0;
		virtual AelFrame read() = 0;
		virtual AelFrame readWrite(AelFrame&) = 0;
		virtual ~AelDelayLine() { }
		
	protected:
		int sampleRate;
		int maxDelayLen;
		int channels;
		vector<AelFrame> delay;
		
	};

///////////////////////////////////////////////////////////////////////////////
// Classe AelFixDelayLine
// Representa um delay de frames de uma stream, com tamanho fixo
// Variáveis-Membro:
//   position_r(int), position_w(int)
// Funções-Membro:
//   1 Construtores
//   Métodos virtuais implementados que traduzem a escrita/leitura no delay
//  com tamanho fixo
//   Método que retorna tempo de delay
//////////////////////////////////////////////////////////////////////////////
	class AelFixDelayLine : public AelDelayLine
	{
	public:
		AelFixDelayLine(float time = 0, float sampleRate = 44100, int n_ch = 2); // time in seconds
		bool write(AelFrame&);
		float getDelayTime(){ return maxDelayLen / 44100.0; }
		AelFrame read();
		AelFrame readWrite(AelFrame&);
		
	protected:
		int position_r;
		int position_w;
		
	};

///////////////////////////////////////////////////////////////////////////////
// Classe AelVDelayLine
// Representa um delay de frames de uma stream, com tamanho variável
// Variáveis-Membro:
//   vdt (float), position_w (int)
// Funções-Membro:
//   1 Construtores
//   Métodos virtuais implementados que traduzem a escrita/leitura no delay
//  com tamanho variável
//   Método que retorna tempo de delay
//   Método que atribui novo tamanho ao delay
//////////////////////////////////////////////////////////////////////////////
	class AelVDelayLine : public AelDelayLine {
	public:
		AelVDelayLine(float del = 0.1, float maxdel = 2, float sampleRate = 44100, int n_ch = 2);
		AelFrame read();
		bool write(AelFrame&);
		AelFrame readWrite(AelFrame&);
		void setDelayTime(float delayTime);
		float getDelayTime() { return vdt / sampleRate;}
		
	private:
		float vdt;
		int position_w;
	};
	
	
///////////////////////////////////////////////////////////////////////////////
// Classe AelUniComb
// Representa um filtro Comb Universal, a partir do qual é possível obter dois
//   tipos diferentes de filtros Comb (IIR e FIR), um filtro Allpass e um delay
//   por variação dos coeficientes
// Variáveis-Membro:
//   ucombdelay(AelVDelayLine), BL(float), FF(float), FB(float)
// Funções-Membro:
//   1 Construtores
//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
//   Método virtual (processFrame) que retorna frame processada pelo efeito
//   Métodos set/get dos coeficientes do filtro e do tempo de atraso
/////////////////////////////////////////////////////////////////////////////
	class AelUniComb : public AelEffect {
		
	public:
		AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate = 44100, int n_ch = 2);
		AelFrame& processFrame(AelFrame&);
		void setBL(float);
		void setFB(float);
		void setFF(float);
		void setDelayTime(float);
		float getBL();
		float getFB();
		float getFF();
		AelEffect* getCopy();
		float getDelayTime() { return ucombdelay.getDelayTime(); }
		~AelUniComb();
	private:

		AelVDelayLine ucombdelay;
		float BL;
		float FF;
		float FB;
		
	};

///////////////////////////////////////////////////////////////////////////////
// Classe AelFlanger
// Representa o efeito AelFlanger
// Variáveis-Membro:
//   delayline(AelVDelayLine), delayTime(float), feedBack(float), modAngle(float)
//   angleInc(float), LFOfreq(float), depth(LFO)
// Funções-Membro:
//   1 Construtores
//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
//   Método virtual (processFrame) que retorna frame processada pelo efeito
//   Métodos set/get das variáveis membro
/////////////////////////////////////////////////////////////////////////////
	class AelFlanger : public AelEffect {
		
		float delayTime;
		float feedBack;
		float modAngle;
		float angleInc;
		float LFOfreq;
		float depth;
		AelVDelayLine delayLine;
		
	public:
		
		AelFlanger(float delay, float feedback, float modFreq = 1.0, float depth_ = 0.001, int n_chan = 2, int samplerate = 44100) : AelEffect(n_chan, samplerate), delayLine(delay, delay + depth_, sampleRate, n_chan) , delayTime(delay), feedBack(feedback), modAngle(0), angleInc(2 * M_PI *  1 / sampleRate), LFOfreq(modFreq), depth(depth_) {
		}
		
		void setDelayTime(float dt);
		void setFeedBack(float fb);
		void setLFOFreq(float freq);
		void setDepth(float dp);
		
		float getDelayTime();
		float getFeedBack();
		float getLFOFreq();
		float getDepth();
		
		AelEffect* getCopy();
		
		virtual AelFrame& processFrame(AelFrame&);
		
	};

	
///////////////////////////////////////////////////////////////////////////////
// Classe AelReverb
// Representa efeito de Reverb
// Variáveis-Membro:
//   6 UniComb - 4 CombFilter 2 AllPass com parametros predefenidos na criacao
// da classe e nao variaveis
// Funções-Membro:
//   1 Construtores
//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
//   Método virtual (processFrame) que retorna frame processada pelo efeito
//   Métodos set/get do coeficiente RVT (reverb time)
/////////////////////////////////////////////////////////////////////////////
	class AelReverb: public AelEffect{
		
		float RVT;
		AelUniComb C1;
		AelUniComb C2;
		AelUniComb C3;
		AelUniComb C4;
		AelUniComb A1;
		AelUniComb A2;
		
	public:
		AelReverb(float RVT_, int n_ch=2, int samplerate=44100);
		
		void setRVT(float rvt);
		float getRVT();
		
		AelEffect* getCopy();
		virtual AelFrame& processFrame(AelFrame& iFrame);
		
		
		
	};
	
///////////////////////////////////////////////////////////////////////////////
// Classe AelEcho
//  Classe que implementa o effeito Echo utilizando um delay com feeback
// Variáveis-Membro:
//   AelUniComb (echodelay)
// Funções-Membro:
//   1 Construtores
//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
//   Método virtual (processFrame) que retorna frame processada pelo efeito
//   Métodos set/get dos coeficientes do filtro e do tempo de atraso
/////////////////////////////////////////////////////////////////////////////
	
	class AelEcho : public AelEffect{
	public:
		AelEcho(float echo_time,float feedback, int n_ch = 2, int samplerate = 44100);
		//void setECT(float ect);
		//float getECT();
		bool setFB(float fb);
		void setDelayTime(float);
		float getFB();
		float getDelayTime();
		AelEffect* getCopy();
		virtual AelFrame& processFrame(AelFrame& iFrame);
		
	private:
		AelUniComb echodelay;
		
	};
	
}

#endif
