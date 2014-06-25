#ifndef __DELAYS__
#define __DELAYS__

#include "AelEffects.h"
#include <cmath>

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
		AelFrame read();
		AelFrame readWrite(AelFrame&);

	protected:
		int position_r;
		int position_w;

	};

	class AelVDelayLine : public AelDelayLine {
	public:
		AelVDelayLine(float del = 0, float maxdel = 1, float sampleRate = 44100, int n_ch = 2);
		AelFrame read();
		bool write(AelFrame&);
		AelFrame readWrite(AelFrame&);
        void setDelayTime(float delayTime);

	private:
		float vdt;
		int position_w;
	};

	

	class AelUniComb : public AelEffect {

	public:
		AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate = 44100, int n_ch = 2);
		AelFrame& processFrame(AelFrame&);
		~AelUniComb();

	private:
		AelFixDelayLine ucombdelay;
		int channels;
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
        AelVDelayLine delayLine;
        
    public:
        
        AelFlanger(float delay, float feedback,  int n_chan = 2, int samplerate = 44100) : AelEffect(samplerate), delayLine(delay, delay + 0.001, sampleRate, n_chan) , delayTime(delay), feedBack(feedback), modAngle(0), angleInc(2 * M_PI *  1 / sampleRate), LFOfreq(1) { }
    
        void setDelayTime(float dt);
        void setFeedBack(float fb);
        void setLFOFreq(float freq);
        
        float getDelayTime();
        float getFeedBack();
        float getLFOFreq();
        
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
        AelReverb(float RVT_, int n_ch=2, int samplerate=44100): AelEffect(samplerate), RVT(RVT_),
            C1(0.0297, 0, pow(0.001, 0.0297/RVT_), 1, samplerate, n_ch),
            C2(0.0371, 0, pow(0.001, 0.0371/RVT_), 1, samplerate, n_ch),
            C3(0.0411, 0, pow(0.001, 0.0411/RVT_), 1, samplerate, n_ch),
            C4(0.0437, 0, pow(0.001, 0.0437/RVT_), 1, samplerate, n_ch),
            A1(0.09683, pow(0.001, 0.09683/0.005), -pow(0.001, 0.09683/0.005), 1, samplerate, n_ch),
            A2(0.03292, pow(0.001, 0.03292/0.0017), -pow(0.001, 0.03292/0.0017), 1, samplerate, n_ch)
        {
        
        }
        
        void setRVT(float rvt);
        float getRVT();
        virtual AelFrame& processFrame(AelFrame& iFrame);

        //float time, float _BL, float _FB, float _FF, float samplerate = 44100, int n_ch = 2)
    
    };



}

#endif