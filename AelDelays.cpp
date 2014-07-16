//
//  AelDelays.cpp
//  AEL_PROJECT
//
//  Created by Paulo Jorge Felix Oliveira on 01/07/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#include "AelDelays.h"

namespace Ael{
	
//////////////////////////////////////////////////////////////////
//Classe AelCFixDelayTime
//////////////////////////////////////////////////////////////////

	/******************************************************************
	 *Construtor da classe AelFixDelayTime
	 *Parâmetros: void
	 *Responsável por atribui valores às variáveis membro
	 *******************************************************************/
	AelFixDelayLine::AelFixDelayLine(float time, float sampleRate, int n_ch) : AelDelayLine(time, sampleRate, n_ch), position_r(0), position_w(0)
	{
		
	}
	
	/******************************************************************
	 *Função membro write da classe AelFixDelayLine
	 *Parâmetros: AelFrame& (iframe)
	 *Escrita de Frame no buffer de frames atrasadas
	 *******************************************************************/
	bool AelFixDelayLine::write(AelFrame& iframe){
		
		if (maxDelayLen){
			delay[position_w] = iframe;
			position_w = (position_w != (delay.size() - 1) ? position_w + 1 : 0);
			return true;
		}
		
		return false;
		
	}
	
	/******************************************************************
	 *Função membro read da classe AelFixDelayLine
	 *Parâmetros: void
	 *Leitura e retorno de Frae do buffer de Frames atrasadas
	 *******************************************************************/
	AelFrame AelFixDelayLine::read(){
		
		AelFrame new_frame(channels);
		if (maxDelayLen){
			new_frame = AelFrame(delay[position_r]);
			position_r = (position_r != (delay.size() - 1) ? position_r + 1 : 0);
		}
		
		return new_frame;
	}
	
	/******************************************************************
	 *Função membro readWrite da classe AelFixDelayLine
	 *Parâmetros: AelFrame& (iframe)
	 *Escrita e Leitura de Frame no buffer de Frames atrasadas
	 *******************************************************************/
	AelFrame AelFixDelayLine::readWrite(AelFrame& iframe){
		write(iframe);
		return read();
	}
	

//////////////////////////////////////////////////////////////////
//Classe AelVDelayLine
//////////////////////////////////////////////////////////////////

	/******************************************************************
	 *Construtor AelVDelayLine da classe AelVDelayLine
	 *Parâmetros: float (del), float(maxdel), float (samplerate), int (n_ch)
	 *******************************************************************/
	AelVDelayLine::AelVDelayLine(float del, float maxdel, float sampleRate, int n_ch) : AelDelayLine(maxdel, sampleRate, n_ch), position_w(0), vdt(del * sampleRate)
	{
		if (MORETHAN(vdt, maxDelayLen)) {
			AelExecption("VariableDelay greater than MaxDelay"); }
	
	}
	
	
	/******************************************************************
	 *Função membro read da classe AelVDelayLine
	 *Parâmetros: void
	 *Leitura e Retorno de Frame do buffer de Frames atrasadas
	 *******************************************************************/
	AelFrame AelVDelayLine::read(){
		
		float rp, frac;
		int rpi;
		
		if (maxDelayLen){
			rp = position_w - vdt;
			
			rp = ( MOREEQ(rp, 0)  ? (LESSTHAN(rp, maxDelayLen) ? rp : rp - maxDelayLen) : rp + maxDelayLen);
			
			if(rp >= maxDelayLen || rp < 0.0)
				rp = 0;
			
			rpi = floor(rp);
			frac = rp - rpi;
			
			AelFrame next = (rpi != maxDelayLen - 1 ? delay[rpi + 1] : delay[0]);
			AelFrame out = delay[rpi] + (next - delay[rpi]) * frac;
			return out;
		}
		
		return AelFrame(channels);
	}
	
	/******************************************************************
	 *Função membro write da classe AelVDelayLine
	 *Parâmetros: AelFrame& (iframe)
	 *Escrita de Frame no buffer de frames atrasadas
	 *Retorna sucesso/insucesso da operação
	 *******************************************************************/
	bool AelVDelayLine::write(AelFrame& iframe){
		if (maxDelayLen){
			delay[position_w] = iframe;
			position_w = (position_w != maxDelayLen - 1 ? position_w + 1 : 0);
			return true;
		}
		return false;
	}
	
	/*******************************************************************
	 *Função membro readWrite da classe AelVDelayLine
	 *Parâmetros: AelFrame& (iframe)
	 *Leitura/Escrita de Frame no buffer de frames atrasadas
	 *******************************************************************/
	AelFrame AelVDelayLine::readWrite(AelFrame& iframe){
		AelFrame out = read();
		write(iframe);
		return out;
	}
	
	/*******************************************************************
	 *Função membro setDelayTime da classe AelVDelayLine
	 *Parâmetros: float (delayTime)
	 *Método set da variável-membro vdt
	 *******************************************************************/
	void AelVDelayLine::setDelayTime(float delayTime){
		
		vdt = delayTime * sampleRate;
		if (MORETHAN(vdt, maxDelayLen)  ){ AelExecption("VariableDelay greater than MaxDelay"); }
		//vdt > maxDelayLen
	}
	

//////////////////////////////////////////////////////////////////
//Classe AelUniComb
//////////////////////////////////////////////////////////////////
	
	/*******************************************************************
	 *Construtor da classe AelUniComb
	 *Parâmetros: float time, float _BL, float _FB, float _FF, 
	 *  float samplerate, int n_ch
	 *******************************************************************/
	AelUniComb::AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate, int n_ch) : BL(_BL), FB(_FB), FF(_FF), ucombdelay(1, 2 ,samplerate, n_ch), AelEffect(n_ch,samplerate)
	{
		if (MORETHAN(time, 1.99))
			ucombdelay.setDelayTime(1.9);
		else if (LESSTHAN(time, 0.001))
			ucombdelay.setDelayTime(0.001);
		else
			ucombdelay.setDelayTime(time);

	}
	
	/*******************************************************************
	 *Função membro processFrame da class AelUniComb
	 *Parâmetros: AelFrame& iframe
	 *Responsável por processar a frame recebida pelo efeito produzido 
	 *pelo filtro Unicomb
	 *******************************************************************/
	AelFrame& AelUniComb::processFrame(AelFrame& iframe){
		
		AelFrame &out = iframe;
		AelFrame xh(n_channels), delayout = ucombdelay.read();
		
		xh = iframe + delayout * FB; // xh = in + delay*FB
		out = (delayout * FF + xh * BL) * getWetLevel() + iframe * (1- getWetLevel()); // out = delay*FF + xh*BL
		ucombdelay.write(xh); //delay = xh
		
		return out;
	}
	
	/*******************************************************************
	 *Função membro setBL da class AelUniComb
	 *Parâmetros: float _BL
	 *Método Set da variável membro BL
	 *******************************************************************/
	void AelUniComb::setBL(float _BL){
		
		if(MORETHAN(_BL, 1.0))
			BL = 1.0;
		
		else if (LESSTHAN(_BL, 0.0))
			BL = 0.0;
		
		else
			BL = _BL;
	
	}
	
	/*******************************************************************
	 *Função membro setFB da class AelUniComb
	 *Parâmetros: float _FB
	 *Método Set da variável membro FB
	 *******************************************************************/
	void AelUniComb::setFB(float _FB){
		
		if(MORETHAN(_FB, 1.0))
			FB = 1.0;
		
		else if (LESSTHAN(_FB, 0.0))
			FB = 0.0;
		
		else
			FB = _FB;
		
	}
	
	/*******************************************************************
	 *Função membro setFF da class AelUniComb
	 *Parâmetros: float _FF
	 *Método Set da variável membro FF
	 *******************************************************************/
	void AelUniComb::setFF(float _FF){
		
		if(MORETHAN(_FF, 1.0))
			FF = 1.0;
		
		else if (LESSTHAN(_FF, 0.0))
			FF = 0.0;
		
		else
			FF = _FF;
		
	}
	
	/*******************************************************************
	 *Função membro setDelayTime da class AelUniComb
	 *Parâmetros: float new_delay
	 *Responsável por atribuir valor ao tamanho do delay
	 *******************************************************************/
	void AelUniComb::setDelayTime(float new_delay){
		if (MORETHAN(new_delay, 1.99))
			ucombdelay.setDelayTime(new_delay);

		else if (LESSTHAN(new_delay, 0.001))
			ucombdelay.setDelayTime(0.001);

		else
			ucombdelay.setDelayTime(new_delay);
	
	}
	
	
	/*******************************************************************
	 *Função membro getBL da class AelUniComb
	 *Parâmetros: void
	 *Retorna valor da variável membro BL
	 *******************************************************************/
	float AelUniComb::getBL(){
		return BL;
	}
	
	/*******************************************************************
	 *Função membro getFB da class AelUniComb
	 *Parâmetros: void
	 *Retorna valor da variável membro FB
	 *******************************************************************/
	float AelUniComb::getFB(){
		return FB;
	}
	
	/*******************************************************************
	 *Função membro getFF da class AelUniComb
	 *Parâmetros: void
	 *Retorna valor da variável membro FF
	 *******************************************************************/
	float AelUniComb::getFF(){
		return FF;
	}

	/*******************************************************************
	 *Função membro getCopy da class AelUniComb
	 *Parâmetros: void
	 *Retorna Cópia do estado do efeito, com os buffers de atraso vazios
	 *******************************************************************/
	AelEffect* AelUniComb::getCopy(){
			AelEffect* temp = new AelUniComb(ucombdelay.getDelayTime(), BL, FB, FF,sampleRate,n_channels);
			return temp;
	}
	
	
	AelUniComb::~AelUniComb()
	{}
	
	
//////////////////////////////////////////////////////////////////
//Classe AelFlanger
//////////////////////////////////////////////////////////////////
	
	/*******************************************************************
	 *Função membro setDelayTime da class AelFlanger
	 *Parâmetros: float dt
	 *Responsável por atribuir valor ao tamanho do delay
	 *******************************************************************/
	void AelFlanger::setDelayTime(float dt){
		
		if( LESSTHAN(dt, 0.0) || MORETHAN(dt, 0.015))
			return;
		
		try {
			delayLine.setDelayTime(dt);
			delayTime = dt;
		} catch (AelExecption&) {
			
		}
		
	}
	
	/*******************************************************************
	 *Função membro setFeedBack da class AelFlanger
	 *Parâmetros: float dt
	 *Método set da variável-membro feedback
	 *******************************************************************/
	void AelFlanger::setFeedBack(float fb){
		
		if(MORETHAN(fb, 1.0))
			feedBack = 1.0;
		
		else if (LESSTHAN(fb, -1.0))
			feedBack = 0.0;
		
		else
			feedBack = fb;
		
	}
	
	/*******************************************************************
	 *Função membro setLFOFreq da class AelFlanger
	 *Parâmetros: float freq
	 *Método set da variável-membro LFO
	 *******************************************************************/
	void AelFlanger::setLFOFreq(float freq){
		
		if(MORETHAN(freq, 1.0))
			feedBack = 1.0;
		
		else if (LESSTHAN(freq, 0.1))
			LFOfreq = 0.1;
		
		else
			LFOfreq = freq;
		
		angleInc = 2 * M_PI *  freq / sampleRate;
		
		
	}
	
	/*******************************************************************
	 *Função membro setDepth da class AelFlanger
	 *Parâmetros: float dp
	 *Método set da variável-membro LFO
	 *******************************************************************/
	void AelFlanger::setDepth(float dp){
		
		if(MORETHAN(dp, 0.002))
			depth = 0.002;
		
		else if (LESSTHAN(dp, 0.0))
			depth = 0.0;
		
		else
			depth = dp;
		
	}
	
	/*******************************************************************
	 *Função membro getDelayTime da class AelFlanger
	 *Parâmetros: void
	 *Retorna valor da variável-membro delayTime
	 *******************************************************************/
	float AelFlanger::getDelayTime(){
		
		return delayTime;
		
	}
	
	/*******************************************************************
	 *Função membro getDelayTime da class AelFlanger
	 *Parâmetros: void
	 *Retorna valor da variável-membro delayTime
	 *******************************************************************/
	float AelFlanger::getFeedBack(){
		return feedBack;
	}
	
	/*******************************************************************
	 *Função membro getLFOFreq da class AelFlanger
	 *Parâmetros: void
	 *Retorna valor da variável-membro LFOfreq
	 *******************************************************************/
	float AelFlanger::getLFOFreq(){
		return LFOfreq;
	}
	
	/*******************************************************************
	 *Função membro getDepth da class AelFlanger
	 *Parâmetros: void
	 *Retorna valor da variável-membro depth
	 *******************************************************************/
	float AelFlanger::getDepth(){
		return depth;
	}
	
	/*******************************************************************
	 *Função membro processFrame da class AelFlanger
	 *Parâmetros: AelFrame& frame
	 *Responsável por processar a frame recebida pelo efeito produzido
	 *pelo Flanger
	 *******************************************************************/
	AelFrame& AelFlanger::processFrame(AelFrame& frame){
		
		AelFrame temp = frame;
		float mod = sin(modAngle);
		modAngle += angleInc;

        
		delayLine.setDelayTime(delayTime + (mod * depth)); // set do delay time,modulada pela onda sinusoidal
        

		
		delayLine.setDelayTime(delayTime + (mod * depth));
		

		AelFrame temp2 = delayLine.read();
		
		frame = (temp2 * getWetLevel()) + (temp * (1- getWetLevel()));
		
		temp =  frame * feedBack + temp ;
		
		delayLine.write(temp);
		
		return frame;
	}



	
	/*******************************************************************
	*Função membro getCopy da class AelFlanger
	*Parâmetros: void
	*Retorna Cópia do estado do efeito, com os buffers de atraso vazios
	*******************************************************************/
	AelEffect* AelFlanger::getCopy(){
		AelEffect* temp = new AelFlanger(delayTime, feedBack, LFOfreq, depth, n_channels, sampleRate);
		return temp;
	}
	


	
//////////////////////////////////////////////////////////////////
//Classe AelReverb
//////////////////////////////////////////////////////////////////

	/*******************************************************************
	*Construtor da classe AelReverb
	*Parâmetros: float reverb time, int numero de canais, float samplerate
	*******************************************************************/
	

	AelReverb::AelReverb(float RVT_, int n_ch, int samplerate): AelEffect(n_ch,samplerate), RVT(RVT_),
	C1(0.0297, 0, pow(0.001, 0.0297/RVT_), 1, samplerate, n_ch),
	C2(0.0371, 0, pow(0.001, 0.0371/RVT_), 1, samplerate, n_ch),
	C3(0.0411, 0, pow(0.001, 0.0411/RVT_), 1, samplerate, n_ch),
	C4(0.0437, 0, pow(0.001, 0.0437/RVT_), 1, samplerate, n_ch),
	A1(0.09683, pow(0.001, 0.09683/0.005), -pow(0.001, 0.09683/0.005), 1, samplerate, n_ch),
	A2(0.03292, pow(0.001, 0.03292/0.0017), -pow(0.001, 0.03292/0.0017), 1, samplerate, n_ch)
	{
		
	}
	
	/*******************************************************************
	*Função membro setRVT da class AelReverb
	*Parâmetros: float reverb time
	*Método set da variável-membro rvt
	*******************************************************************/
	void AelReverb::setRVT(float rvt){

		if(MORETHAN(rvt, 1.5))
			rvt = 1.5;

		else if (LESSTHAN(rvt, 1))
			rvt = 1;

		else
			RVT = rvt;
		
		C1.setFB(pow(0.001, 0.0297/RVT));
		C2.setFB(pow(0.001, 0.0371/RVT));
		C3.setFB(pow(0.001, 0.0411/RVT));
		C4.setFB(pow(0.001, 0.0437/RVT));
	}
	
	
	/*******************************************************************
	*Função membro geRVT da class AelReverb
	*Parâmetros: void
	*Retorna valor da variável-membro reverb time
	*******************************************************************/
	float AelReverb::getRVT(){
		return RVT;
	}
	

	/*******************************************************************
	*Função membro processFrame da class AeReverb
	*Parâmetros: AelFrame& iframe
	*Responsável por processar a frame recebida pelo efeito produzido
	*pelo efeito AelReverb.
	*Consiste em chamar os 4 CombFilters em paralelo e 2 AllPass em série
	*******************************************************************/

	AelFrame& AelReverb::processFrame(AelFrame& iFrame){
		
		
		AelFrame aux1 = iFrame, aux2 = iFrame, aux3 = iFrame, aux4 = iFrame, temp = iFrame;
		
		C1.processFrame(aux1);
		C2.processFrame(aux2);
		C3.processFrame(aux3);
		C4.processFrame(aux4);
		
		iFrame = (aux1 * 0.25) + (aux2 * 0.25) + (aux3 * 0.25) + (aux4 * 0.25); //frame é processada por 4 filtros Comb
		
		A1.processFrame(iFrame);
		A2.processFrame(iFrame); //frame é processada por 2 filtros Allpass
		
		iFrame = iFrame * getWetLevel() + temp * (1 - getWetLevel());  //wetdry
		
		return iFrame;
		
	}
	
	/*******************************************************************
	*Função membro getCopy da class AelReverb
	*Parâmetros: void
	*Retorna Cópia do efeito, so que estado inicial, com os
	*delays associados aos UniversalComb apagados
	*******************************************************************/
	AelEffect* AelReverb::getCopy() {
		AelEffect* temp = new AelReverb(RVT, n_channels, sampleRate);
		return temp;
	}
	
	
//////////////////////////////////////////////////////////////////
//Classe AelEco
//////////////////////////////////////////////////////////////////
	
	/*******************************************************************
	*Construtor da classe AelEcho
	*Parâmetros: float echo time (delay), float feedback,
	*int numero de canais, int sample rate
	*******************************************************************/
	AelEcho::AelEcho(float echo_time, float feedback, int n_ch, int samplerate) : AelEffect(n_ch,samplerate), echodelay(1, 1, feedback, 0, samplerate, n_ch)
	{
		echodelay.setDelayTime(echo_time);
		echodelay.setFB(feedback);
	}
	
	/*******************************************************************
	*Função membro setFB da class AelEcho
	*Parâmetros: float feedback
	*set do membro feedback do UniComb, do echo
	*limitado superiormente por 0.95 e inferiormente por 0.05
	*******************************************************************/
	bool  AelEcho::setFB(float fb){
		
		if(MORETHAN(fb, 0.95))
			echodelay.setFB(0.95);
		
		else if (LESSTHAN(fb, 0.05))
			echodelay.setFB(0.05);
		
		else
			echodelay.setFB(fb);
	   
		return true;
	}
	
	/*******************************************************************
	*Função membro setDelayTime da class AelEcho
	*Parâmetros: float echo time ( delay time )
	*Set da variável-membro delay time, responsavel pelo 
	*delay associado ao echo, logo ao ComFilter
	*******************************************************************/
	void AelEcho::setDelayTime(float echo_time){
		
		if (MORETHAN(echo_time, 1.99))
			echodelay.setDelayTime(1.99);

		else if (LESSTHAN(echo_time, 0.025))
			echodelay.setDelayTime(0.025);

		else
			echodelay.setDelayTime(echo_time);

	}

	/*******************************************************************
	*Função membro geFB da class AelEcho
	*Parâmetros: void
	*Retorna valor da variável-membro feedback do comb associado ao Echo
	*******************************************************************/
	float AelEcho::getFB(){
		return echodelay.getFB();
	}
	

	/*******************************************************************
	*Função membro getDelayTime da class AelEcho
	*Parâmetros: void
	*Retorna valor da variável-membro delay time ( delay associado ao 
	*efeito echo e ao CombFilter)
	*******************************************************************/
	float AelEcho::getDelayTime(){
		return echodelay.getDelayTime();
	}
	
	/*******************************************************************
	*Função membro processFrame da class AeEcho
	*Parâmetros: AelFrame& iframe
	*Chama a função de processFrame associada ao CombFilter
	*******************************************************************/

	AelFrame& AelEcho::processFrame(AelFrame& iFrame){
		return echodelay.processFrame(iFrame);
	}
	

	/*******************************************************************
	*Função membro getCopy da class AelEcho
	*Parâmetros: void
	*Retorna Cópia do efeito, so que estado inicial, com o
	*delay associados ao Echo, logo ao CombFilter, apagados
	*******************************************************************/

	AelEffect* AelEcho::getCopy() {
		AelEffect* temp = new AelEcho(echodelay.getDelayTime(), echodelay.getFB(), n_channels, sampleRate);
		return temp;
	}


}
