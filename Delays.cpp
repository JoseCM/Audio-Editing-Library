#include "Delays.h"
#include "AelException.h"

namespace Ael{
	AelDelayLine::AelDelayLine(float time, float sampleRate, int n_ch) : position_r(0), position_w(0), channels(n_ch)
	{
		delayLen = time * sampleRate;
		delay.resize(delayLen, AelFrame(n_ch));
	}

	bool AelDelayLine::write(AelFrame& iframe){

		if (delayLen){
			delay[position_w] = iframe;
			position_w = (position_w != (delay.size() - 1) ? position_w + 1 : 0);
			return true;
		}

		return false;

	}

	AelFrame AelDelayLine::read(){

		AelFrame new_frame(channels);
		if (delayLen){
			new_frame = AelFrame(delay[position_r]);
			position_r = (position_r != (delay.size() - 1) ? position_r + 1 : 0);
		}

		return new_frame;
	}

	AelFrame AelDelayLine::readWrite(AelFrame& iframe){
		write(iframe);
		return read();
	}

	AelDelayLine::~AelDelayLine()
	{
	}


//////////////////////////VDELAYLINE

	AelVDelayLine::AelVDelayLine(float del, float maxdel, float sampleRate, int n_ch) : position_w(0)
	{
		vdt = del * sampleRate;
		mdt = maxdel * sampleRate;
		if (vdt > mdt){ AelExecption("VariableDelay greater than MaxDelay"); }
		vdelay.resize(mdt, AelFrame(n_ch));

	}

	AelFrame AelVDelayLine::read(){

		float rp, frac;
		int rpi;

		if (mdt){
			rp = position_w - vdt;
			rp = (rp >= 0 ? (rp < mdt ? rp : rp - mdt) : rp + mdt);
			rpi = floor(rp);
			frac = rp - rpi;
			AelFrame next = (rpi != mdt - 1 ? vdelay[rpi + 1] : vdelay[0]);
			AelFrame out = vdelay[rpi] + (next - vdelay[rpi])*frac;
			return out;
		}
		return AelFrame(channels);

	}

	bool AelVDelayLine::write(AelFrame& iframe){
		if (mdt){
			vdelay[position_w] = iframe;
			position_w = (position_w != mdt - 1 ? position_w + 1 : 0);
			return true;
		}
		return false;
	}

	AelFrame AelVDelayLine::readWrite(AelFrame& iframe){
		AelFrame out = read();
		write(iframe);
		return out;
	}

	AelVDelayLine::~AelVDelayLine()
	{}




/////////////////////UNICOMBFILTER

	AelUniComb::AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate, int n_ch) : BL(_BL), FB(_FB), FF(_FF), channels(n_ch), ucombdelay(time, samplerate, n_ch), AelEffect()
	{
	}

	AelFrame& AelUniComb::processFrame(AelFrame& iframe){

		AelFrame &out = iframe;
		AelFrame xh(channels), delayout = ucombdelay.read();

		xh = iframe + delayout * FB; // xh = in + delay*FB
		out = delayout * FF + xh * BL; // out = delay*FF + xh*BL
		ucombdelay.write(xh); //delay = xh


		return out;
	}

	AelUniComb::~AelUniComb()
	{}

}