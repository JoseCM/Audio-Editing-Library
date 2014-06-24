#ifndef __DELAYS__
#define __DELLAYS__


#include "AelEffects.h"
#include <cmath>

namespace Ael {

	class DelayLine
	{
	public:
		DelayLine(float time, float sampleRate = 44100, int n_ch = 2); // time in seconds
		AelAudioStream& processStream(AelAudioStream& mystream);
		virtual AelFrame& processFrame(AelFrame& myframe);
		~DelayLine();

	protected:
		int delayLen;
		int position;
		int channels;
		vector<AelFrame> delay;

	};

	///////////////////////////////////////////////////////////////
	//CombFilter

	class AelCombFilter : public DelayLine
	{
	public:
		AelCombFilter(float time, float fd_gain, float samplerate = 44100, int n_ch = 2); //gain no more than 1
		~AelCombFilter();
		AelFrame& processFrame(AelFrame&);
	private:
		float gain;
	};


	////////////////////////////////////////////////////////////////
	//All Pass Filter

	class AelAllPassFilter : public DelayLine
	{
	public:
		AelAllPassFilter(float time, float fd_gain, float samplerate = 44100, int n_ch = 2);
		~AelAllPassFilter();
		AelFrame& processFrame(AelFrame&);

	private:
		float gain;
	};

	



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//NOT WORKING

//////////////////////////////////////////////////////////////////
// VariableDelay

	class AelVariableDelay {

	public:
		AelVariableDelay(float _delay, float max_delay, float samplerate = 44100, int n_ch = 2);
		~AelVariableDelay();
		AelAudioStream& processStream(AelAudioStream& mystream);
		AelFrame& processFrame(AelFrame& mystream);

	protected:
		
		float maxDelayLen;
		float vdelayLen;
		int position;
		int channels;
		vector<AelFrame> delay;
	
	};


	class AelChorus : public AelVariableDelay{

	public:
		AelChorus(float dltime,float _depth = 0.005, float _rate = 0.15, float _center = 0.010, float _sr = 44100, int n_channels = 2);
		AelAudioStream& processStream(AelAudioStream& mystream);
		~AelChorus();


	private:
		float rate;
		float depth;
		float center;
		float samplerate;
		int channels;

	};

	
}

#endif