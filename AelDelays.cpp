//
//  AelDelays.cpp
//  AEL_PROJECT
//
//  Created by Paulo Jorge Felix Oliveira on 01/07/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#include "AelDelays.h"

namespace Ael{
	
	AelFixDelayLine::AelFixDelayLine(float time, float sampleRate, int n_ch) : AelDelayLine(time, sampleRate, n_ch), position_r(0), position_w(0)
	{
		
	}
	
	bool AelFixDelayLine::write(AelFrame& iframe){
		
		if (maxDelayLen){
			delay[position_w] = iframe;
			position_w = (position_w != (delay.size() - 1) ? position_w + 1 : 0);
			return true;
		}
		
		return false;
		
	}
	
	AelFrame AelFixDelayLine::read(){
		
		AelFrame new_frame(channels);
		if (maxDelayLen){
			new_frame = AelFrame(delay[position_r]);
			position_r = (position_r != (delay.size() - 1) ? position_r + 1 : 0);
		}
		
		return new_frame;
	}
	
	AelFrame AelFixDelayLine::readWrite(AelFrame& iframe){
		write(iframe);
		return read();
	}
	
	
	//////////////////////////VDELAYLINE
	
	AelVDelayLine::AelVDelayLine(float del, float maxdel, float sampleRate, int n_ch) : AelDelayLine(maxdel, sampleRate, n_ch), position_w(0), vdt(del * sampleRate)
	{
		if (MORETHAN(vdt, maxDelayLen)) { AelExecption("VariableDelay greater than MaxDelay"); }
		//vdt > maxDelayLen
	}
	
	
	
	AelFrame AelVDelayLine::read(){
		
		float rp, frac;
		int rpi;
		
		if (maxDelayLen){
			rp = position_w - vdt;
			
			//DEBUG
			/*
			if(position_w == 4 && MORETHAN(vdt, 4.0) && LESSTHAN(vdt, 4.1)){
				
			}
			*/
			
			rp = ( MOREEQ(rp, 0)  ? (LESSTHAN(rp, maxDelayLen) ? rp : rp - maxDelayLen) : rp + maxDelayLen);
			
			if(rp >= maxDelayLen || rp < 0.0)
				rp = 0;
			
			rpi = floor(rp);
			frac = rp - rpi;
			
			if(rpi == 67){
   
			}
			
			AelFrame next = (rpi != maxDelayLen - 1 ? delay[rpi + 1] : delay[0]);
			AelFrame out = delay[rpi] + (next - delay[rpi]) * frac;
			return out;
		}
		
		return AelFrame(channels);
	}
	
	bool AelVDelayLine::write(AelFrame& iframe){
		if (maxDelayLen){
			delay[position_w] = iframe;
			position_w = (position_w != maxDelayLen - 1 ? position_w + 1 : 0);
			return true;
		}
		return false;
	}
	
	AelFrame AelVDelayLine::readWrite(AelFrame& iframe){
		AelFrame out = read();
		write(iframe);
		return out;
	}
	
	void AelVDelayLine::setDelayTime(float delayTime){
		
		vdt = delayTime * sampleRate;
		if (MORETHAN(vdt, maxDelayLen)  ){ AelExecption("VariableDelay greater than MaxDelay"); }
		//vdt > maxDelayLen
	}
	
	
	
	/////////////////////UNICOMBFILTER
	
	AelUniComb::AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate, int n_ch) : BL(_BL), FB(_FB), FF(_FF), ucombdelay(1, 2 ,samplerate, n_ch), AelEffect(n_ch,samplerate)
	{
		if (MORETHAN(time, 1.99))
			ucombdelay.setDelayTime(1.9);
		else if (LESSTHAN(time, 0.001))
			ucombdelay.setDelayTime(0.001);
		else
			ucombdelay.setDelayTime(time);

	}
	
	AelFrame& AelUniComb::processFrame(AelFrame& iframe){
		
		AelFrame &out = iframe;
		AelFrame xh(n_channels), delayout = ucombdelay.read();
		
		xh = iframe + delayout * FB; // xh = in + delay*FB
		out = (delayout * FF + xh * BL) * getWetLevel() + iframe * (1- getWetLevel()); // out = delay*FF + xh*BL
		ucombdelay.write(xh); //delay = xh
		
		return out;
	}
	
	void AelUniComb::setBL(float _BL){
		
		if(MORETHAN(_BL, 1.0))
			BL = 1.0;
		
		else if (LESSTHAN(_BL, 0.0))
			BL = 0.0;
		
		else
			BL = _BL;
	
	}
	
	void AelUniComb::setFB(float _FB){
		
		if(MORETHAN(_FB, 1.0))
			FB = 1.0;
		
		else if (LESSTHAN(_FB, 0.0))
			FB = 0.0;
		
		else
			FB = _FB;
		
	}
	
	void AelUniComb::setFF(float _FF){
		
		if(MORETHAN(_FF, 1.0))
			FF = 1.0;
		
		else if (LESSTHAN(_FF, 0.0))
			FF = 0.0;
		
		else
			FF = _FF;
		
	}

	void AelUniComb::setDelayTime(float new_delay){
		if (MORETHAN(new_delay, 1.99))
			ucombdelay.setDelayTime(new_delay);

		else if (LESSTHAN(new_delay, 0.001))
			ucombdelay.setDelayTime(0.001);

		else
			ucombdelay.setDelayTime(new_delay);
	
	}
	
	float AelUniComb::getBL(){
		return BL;
	}
	
	float AelUniComb::getFB(){
		return FB;
	}
	
	float AelUniComb::getFF(){
		return FF;
	}

	AelEffect* AelUniComb::getCopy(){
			AelEffect* temp = new AelUniComb(ucombdelay.getDelayTime(), BL, FB, FF,sampleRate,n_channels);
			return temp;
	}
	
	AelUniComb::~AelUniComb()
	{}
	
	
	/////////////////////AELFLANGER
	
	void AelFlanger::setDelayTime(float dt){
		
		//dt < 0 || dt > 15.00000001
		if( LESSTHAN(dt, 0.0) || MORETHAN(dt, 0.015))
			return;
		
		try {
			delayLine.setDelayTime(dt);
			delayTime = dt;
		} catch (AelExecption&) {
			
		}
		
	}
	
	
	void AelFlanger::setFeedBack(float fb){
		
		if(MORETHAN(fb, 1.0))
			feedBack = 1.0;
		
		else if (LESSTHAN(fb, -1.0))
			feedBack = 0.0;
		
		else
			feedBack = fb;

	}
	
	
	void AelFlanger::setLFOFreq(float freq){
		
		if(MORETHAN(freq, 1.0))
			feedBack = 1.0;
		
		else if (LESSTHAN(freq, 0.1))
			LFOfreq = 0.1;
		
		else
			LFOfreq = freq;
		
		angleInc = 2 * M_PI *  freq / sampleRate;
		
		
	}
	
	void AelFlanger::setDepth(float dp){
		
		if(MORETHAN(dp, 0.002))
			depth = 0.002;
		
		else if (LESSTHAN(dp, 0.0))
			depth = 0.0;
		
		else
			depth = dp;
	   
	}
	
	
	float AelFlanger::getDelayTime(){
		
		return delayTime;
		
	}
	
	float AelFlanger::getFeedBack(){
		return feedBack;
	}
	
	float AelFlanger::getLFOFreq(){
		return LFOfreq;
	}
	
	float AelFlanger::getDepth(){
		return depth;
	}
	
	AelFrame& AelFlanger::processFrame(AelFrame& frame){
		
		AelFrame temp = frame;
		float mod = sin(modAngle);
		modAngle += angleInc;
		
		delayLine.setDelayTime(delayTime + (mod * depth));
		
		AelFrame temp2 = delayLine.read();
		
		frame = (temp2 * getWetLevel()) + (temp * (1- getWetLevel()));
		
		temp =  frame * feedBack + temp ;
		
		delayLine.write(temp);
		
		return frame;
	}

	 AelEffect* AelFlanger::getCopy(){
		 AelEffect* temp = new AelFlanger(delayTime, feedBack, LFOfreq, depth, n_channels, sampleRate);
		 return temp;
	 }
	
	
	//////////////////////AELREVERB
	
	AelReverb::AelReverb(float RVT_, int n_ch, int samplerate): AelEffect(n_ch,samplerate), RVT(RVT_),
	C1(0.0297, 0, pow(0.001, 0.0297/RVT_), 1, samplerate, n_ch),
	C2(0.0371, 0, pow(0.001, 0.0371/RVT_), 1, samplerate, n_ch),
	C3(0.0411, 0, pow(0.001, 0.0411/RVT_), 1, samplerate, n_ch),
	C4(0.0437, 0, pow(0.001, 0.0437/RVT_), 1, samplerate, n_ch),
	A1(0.09683, pow(0.001, 0.09683/0.005), -pow(0.001, 0.09683/0.005), 1, samplerate, n_ch),
	A2(0.03292, pow(0.001, 0.03292/0.0017), -pow(0.001, 0.03292/0.0017), 1, samplerate, n_ch)
	{
		
	}
	
	
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
	
	
	float AelReverb::getRVT(){
		return RVT;
	}
	
	AelFrame& AelReverb::processFrame(AelFrame& iFrame){
		
		
		AelFrame aux1 = iFrame, aux2 = iFrame, aux3 = iFrame, aux4 = iFrame, temp = iFrame;
		
		C1.processFrame(aux1);
		C2.processFrame(aux2);
		C3.processFrame(aux3);
		C4.processFrame(aux4);
		
		iFrame = (aux1 * 0.25) + (aux2 * 0.25) + (aux3 * 0.25) + (aux4 * 0.25);
		
		A1.processFrame(iFrame);
		A2.processFrame(iFrame);
		
		iFrame = iFrame * getWetLevel() + temp * (1 - getWetLevel());
		
		return iFrame;
		
	}
	
	AelEffect* AelReverb::getCopy() {
		AelEffect* temp = new AelReverb(RVT, n_channels, sampleRate);
		return temp;
	}
	
	
	////////////////////////AELECHO
	
	AelEcho::AelEcho(float echo_time, float feedback, int n_ch, int samplerate) : AelEffect(n_ch,samplerate), echodelay(1, 1, feedback, 0, samplerate, n_ch)
	{
		echodelay.setDelayTime(echo_time);
		echodelay.setFB(feedback);
	}
	
	bool  AelEcho::setFB(float fb){
		
		if(MORETHAN(fb, 95.0))
			echodelay.setFB(95.0);
		
		else if (LESSTHAN(fb, 5.0))
			echodelay.setFB(5.0);
		
		else
			echodelay.setFB(fb);
	   
		return true;
	}
	
	void AelEcho::setDelayTime(float echo_time){
		if (MORETHAN(echo_time, 1.99))
			echodelay.setDelayTime(1.99);

		else if (LESSTHAN(echo_time, 0.025))
			echodelay.setDelayTime(0.025);

		else
			echodelay.setDelayTime(echo_time);

	}

	float AelEcho::getFB(){
		return echodelay.getFB();
	}
    
    float AelEcho::getDelayTime(){
        return echodelay.getDelayTime();
    }
	
	AelFrame& AelEcho::processFrame(AelFrame& iFrame){
		return echodelay.processFrame(iFrame);
	}
	
	AelEffect* AelEcho::getCopy() {
		AelEffect* temp = new AelEcho(echodelay.getDelayTime(), echodelay.getFB(), n_channels, sampleRate);
		return temp;
	}
	
}
