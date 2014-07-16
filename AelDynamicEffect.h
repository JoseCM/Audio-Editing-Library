//
//  AelDynamicEffect.h
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#ifndef __Audio_Editing_Library__AelDynamicEffect__
#define __Audio_Editing_Library__AelDynamicEffect__

#include "AelEffects.h"

namespace Ael {
	
	
	///////////////////////////////////////////////////////////////////////////////
	// Classe AelVolume
	//  Classe que implementa um controlador de volume
	// Variáveis-Membro:
	//   double (volume)
	// Funções-Membro:
	//   1 Construtores
	//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
	//   Método virtual (processFrame) que retorna frame processada pelo efeito
	//   Métodos set/get do parametro volume em modo absoluto ou em decibeis
	/////////////////////////////////////////////////////////////////////////////
	
	class AelVolume : public AelEffect  {
		
	private:
		double volume;
		
	public:
		AelVolume(double gain = 1.0, int n_chn = 2, int sampRate = 44100) : Ael::AelEffect(n_chn, sampRate), volume(gain) { }
		double getVolume();
		void setVolume(double);
		double getVolumeDb();
		void setVolumeDb(double);
		
		AelEffect* getCopy();
		AelFrame& processFrame(AelFrame&);
	};
	
	///////////////////////////////////////////////////////////////////////////////
	// Classe AelPanner
	//  Classe que implementa um algoritmo de Panner que varia a intensidade de sinais
	// stereo entre o canal direito e esquerdo
	// Variáveis-Membro:
	//   double (pan, panright, panleft)
	// Funções-Membro:
	//   1 Construtores
	//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
	//   Método virtual (processFrame) que retorna frame processada pelo efeito
	//   Métodos set/get do parametro pan
	/////////////////////////////////////////////////////////////////////////////
	class AelPanner : public AelEffect {
		
	private:
		double pan;
		double panright;
		double panleft;
		
	public:
		AelPanner(double pan = 0.0, int n_chn = 2, int sampRate = 44100) : Ael::AelEffect(n_chn, sampRate), pan(pan), panright( (sin((1 + this->pan) / 2.0 * M_PI / 2.0)) ), panleft((sin((1 - this->pan) / 2.0 * M_PI / 2.0)) ) { }
		double getPan();
		void setPan(double);
		
		AelEffect* getCopy();
		AelFrame& processFrame(AelFrame&);
		
	};
	
	///////////////////////////////////////////////////////////////////////////////
	// Classe AelCompressor
	//  Classe que implementa um algoritmo de compressor, que reduz a gama dinamica
	// do sinal
	// Variáveis-Membro:
	//   double (ratio, thressholDB, attack, release, ratio)
	//   int (threshold)
	// Funções-Membro:
	//   1 Construtores
	//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
	//   Método virtual (processFrame) que retorna frame processada pelo efeito
	//   Métodos set/get para todos os parametros
	/////////////////////////////////////////////////////////////////////////////
	class AelCompressor : public AelEffect {

		int threshold;
		double thresholdDB, attack, release, ratio;
		
		public:
		
		AelCompressor(double rt = 1.0, double thdb = -1.0, double atck = 0.00001, double rls = 0.130, int n_chn = 2, int samplerate = 44100);
		void setRatio(double ratio_);
		void setThreshold(double thresholddb);
		void setAttack(double attack_);
		void setRelease(double release_);
		
		double getRatio() { return ratio; }
		double getThreshold() { return thresholdDB; }
		double getAttackTime() { return -1.0/(log(attack)*sampleRate); }
		double getReleaseTime() { return -1.0/(log(release)*sampleRate); }
		
		virtual AelEffect* getCopy();
		virtual AelFrame& processFrame(AelFrame&);
		
	};

	
	///////////////////////////////////////////////////////////////////////////////
	// Classe AelGate
	//  Classe que implementa um algoritmo de compressor, que atenua o sinal em
	// em segmentos em que o envelope do mesmo diminui abaixo de um nível threshold
	// Variáveis-Membro:
	//   double (attack, release, thresholdDB, attenuationDB, attenuation)
	//   int (threshold)
	// Funções-Membro:
	//   1 Construtores
	//   Método virtual (getCopy) que retorna uma cópia do estado do efeito
	//   Método virtual (processFrame) que retorna frame processada pelo efeito
	//   Métodos set/get para todos os parametros
	/////////////////////////////////////////////////////////////////////////////
	
	class AelGate : public AelEffect {
		
		double attack, release, thresholdDB, attenuationDB, attenuation;

		int threshold;
		
	public:
		
		AelGate(double thdb = -70.0,  double att = -70.0, double atck = 0.00001,  double rls = 0.130, int n_chn = 2, int samplerate = 44100);
		
		void setThreshold(double thresholddb);
		void setAttenuation(double att);
		void setAttack(double attack_);
		void setRelease(double release_);
		
		double getAttenuation() { return attenuationDB; }
		double getThreshold() { return thresholdDB; }
		double getAttackTime() { return - 1.0/(log(attack)*sampleRate); }
		double getReleaseTime() { return - 1.0/(log(release)*sampleRate); }
		
		virtual AelEffect* getCopy();
		virtual AelFrame& processFrame(AelFrame&);
	};
	
}


#endif /* defined(__Audio_Editing_Library__AelDynamicEffect__) */
