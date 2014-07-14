#ifndef __DELAYS__
#define __DELAYS__

#include "AelEffects.h"

namespace Ael {
	
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
