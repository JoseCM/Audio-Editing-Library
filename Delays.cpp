#include "Delays.h"
#include "AelException.h"

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
		if (vdt > maxDelayLen){ AelExecption("VariableDelay greater than MaxDelay"); }
	}
	
	
	
	AelFrame AelVDelayLine::read(){

		float rp, frac;
		int rpi;

		if (maxDelayLen){
			rp = position_w - vdt;
			rp = (rp >= 0 ? (rp < maxDelayLen ? rp : rp - maxDelayLen) : rp + maxDelayLen);
			rpi = floor(rp);
			frac = rp - rpi;
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
		if (vdt > maxDelayLen){ AelExecption("VariableDelay greater than MaxDelay"); }
		
	}



/////////////////////UNICOMBFILTER

	AelUniComb::AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate, int n_ch) : BL(_BL), FB(_FB), FF(_FF), channels(n_ch), ucombdelay(time, samplerate, n_ch), AelEffect(samplerate)
	{
	}

	AelFrame& AelUniComb::processFrame(AelFrame& iframe){

		AelFrame &out = iframe;
		AelFrame xh(channels), delayout = ucombdelay.read();

		xh = iframe + delayout * FB; // xh = in + delay*FB
		out = (delayout * FF + xh * BL) * getWetLevel() + iframe * (1- getWetLevel()); // out = delay*FF + xh*BL
		ucombdelay.write(xh); //delay = xh

		return out;
	}
	
	void AelUniComb::setBL(float _BL){
		BL = _BL;
	}
	
	void AelUniComb::setFB(float _FB){
		FB = _FB;
	}
	
	void AelUniComb::setFF(float _FF){
		FF = _FF;
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

	AelUniComb::~AelUniComb()
	{}
	
	
/////////////////////AELFLANGER
	
	void AelFlanger::setDelayTime(float dt){
		
		if(dt < 0 || dt > 15.00000001)
			return;
		
		try {
			delayLine.setDelayTime(dt);
			delayTime = dt;
		} catch (AelExecption&) {
			
		}
		
	}
	
	
	void AelFlanger::setFeedBack(float fb){
		
		if(fb > 1.0 || fb < -1.0)
			return;
		
		feedBack = fb;
	}
	
	
	void AelFlanger::setLFOFreq(float freq){
		
		if(freq < 0.1 || freq > 1.00001)
			return;
		
		angleInc = 2 * M_PI *  freq / sampleRate;
		
		freq = freq;
		
	}
    
    void AelFlanger::setDepth(float dp){
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
		
		frame = (delayLine.read() * getWetLevel()) + temp * (1- getWetLevel());
		
		temp =  frame * feedBack + temp ;
		
		delayLine.write(temp);
		
		return frame;
	}
	

	
	
//////////////////////AELREVERB
	
	AelReverb::AelReverb(float RVT_, int n_ch, int samplerate): AelEffect(samplerate), RVT(RVT_),
		C1(0.0297, 0, pow(0.001, 0.0297/RVT_), 1, samplerate, n_ch),
		C2(0.0371, 0, pow(0.001, 0.0371/RVT_), 1, samplerate, n_ch),
		C3(0.0411, 0, pow(0.001, 0.0411/RVT_), 1, samplerate, n_ch),
		C4(0.0437, 0, pow(0.001, 0.0437/RVT_), 1, samplerate, n_ch),
		A1(0.09683, pow(0.001, 0.09683/0.005), -pow(0.001, 0.09683/0.005), 1, samplerate, n_ch),
		A2(0.03292, pow(0.001, 0.03292/0.0017), -pow(0.001, 0.03292/0.0017), 1, samplerate, n_ch)
	{

	}
	

	void AelReverb::setRVT(float rvt){
		
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

	

////////////////////////AELECHO

	AelEcho::AelEcho(float echo_time, float feedback, int n_ch, int samplerate) : AelEffect(samplerate), echodelay(echo_time, 1, feedback, 0, samplerate, n_ch)
	{}

	bool  AelEcho::setFB(float fb){

		if (fb < 5 || fb > 95) return false;
		echodelay.setFB(fb);
		return true;
	}

	float AelEcho::getFB(){
		return echodelay.getFB();
	}

	AelFrame& AelEcho::processFrame(AelFrame& iFrame){
		return echodelay.processFrame(iFrame);
	}

	

}