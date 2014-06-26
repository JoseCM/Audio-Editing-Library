#ifndef __DELAYS__
#define __DELLAYS__


#include "AelEffects.h"
#include <cmath>

namespace Ael {

	class AelDelayLine
	{
	public:
		AelDelayLine(float time = 0, float sampleRate = 44100, int n_ch = 2); // time in seconds
		bool write(AelFrame&);
		AelFrame read();
		AelFrame readWrite(AelFrame&);
		~AelDelayLine();

	protected:
		int delayLen;
		int position_r;
		int position_w;
		int channels;
		vector<AelFrame> delay;

	};

	class AelVDelayLine{
	public:
		AelVDelayLine(float del = 0, float maxdel = 1, float sampleRate = 44100, int n_ch = 2);
		AelFrame read();
		bool write(AelFrame&);
		AelFrame readWrite(AelFrame&);
		~AelVDelayLine();

	private:
		float vdt;
		int mdt;
		int position_w;
		int channels;
		vector<AelFrame> vdelay;
	};

	

	class AelUniComb : public AelEffect {


	public:
		AelUniComb(float time, float _BL, float _FB, float _FF, float samplerate = 44100, int n_ch = 2);
		AelFrame& processFrame(AelFrame&);
		~AelUniComb();
	private:
		AelDelayLine ucombdelay;
		int channels;
		float BL;
		float FF;
		float FB;

	};



}

#endif