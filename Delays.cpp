#include "Delays.h"

namespace Ael{
	DelayLine::DelayLine(float time, float sampleRate, int n_ch) : position(0), channels(n_ch)
	{
		delayLen = time * sampleRate;
		delay.resize(delayLen, AelFrame(n_ch));
	}

	AelAudioStream& DelayLine::processStream(AelAudioStream& mystream){
		AelAudioStream *temp = new AelAudioStream(mystream.getchannels());

		for (int i = 0; i < mystream.getnframes(); i++){
			AelFrame aux = mystream.getNextFrame();
			aux = processFrame(aux);
			temp->AddFrames(aux);
		}

		return *temp;
	}

	AelFrame& DelayLine::processFrame(AelFrame& myframe){

		AelFrame frame_aux = myframe;
		myframe = delay[position];
		delay[position] = frame_aux;
		position = (position != (delay.size() - 1) ? position + 1 : 0);

		return myframe;
    
	}

	DelayLine::~DelayLine()
	{
	}

	/////////////////////////////Comb Filter


	//gain(pow(0.001, time / rvbtime))
	AelCombFilter::AelCombFilter(float time, float fd_gain, float samplerate, int n_ch) : gain(fd_gain), DelayLine(time, samplerate, n_ch)
	{
	}

	AelFrame& AelCombFilter::processFrame(AelFrame& myframe){

		AelFrame frame_aux = myframe;
		myframe = delay[position];
		delay[position] = frame_aux + delay[position] * gain; //out*gain
		position = (position != (delay.size() - 1) ? position + 1 : 0);

		return myframe;
	}

	AelCombFilter::~AelCombFilter()
	{
	}

	//////////////////////////All Pass Filter

	AelAllPassFilter::AelAllPassFilter(float time, float fd_gain, float samplerate, int n_ch) : gain(fd_gain), DelayLine(time, samplerate, n_ch)
	{
	}

	AelFrame& AelAllPassFilter::processFrame(AelFrame& myframe){

		AelFrame frame_aux = myframe;
		myframe = delay[position];
		delay[position] = frame_aux + delay[position] * gain; //out*gain
		myframe = delay[position] + (myframe*(-gain));
		position = (position != (delay.size() - 1) ? position + 1 : 0);

		return myframe;
	}

	AelAllPassFilter::~AelAllPassFilter()
	{
	}


	/////////////////////////////Variable Delay

	AelVariableDelay::AelVariableDelay(float _delay, float max_delay, float samplerate, int n_ch) : channels(n_ch)
	{
		if (_delay >= max_delay) {} //throw exeption

		maxDelayLen = max_delay * samplerate;
		vdelayLen = _delay * samplerate;

		delay.resize((int)(maxDelayLen), AelFrame(channels));
		position = 0;

	}

	AelAudioStream& AelVariableDelay::processStream(AelAudioStream& mystream){
		AelAudioStream *temp = new AelAudioStream(mystream.getchannels());

		for (int i = 0; i < mystream.getnframes(); i++){
			AelFrame aux = mystream.getNextFrame();
			aux = processFrame(aux);
			temp->AddFrames(aux);
		}

		return *temp;
	}

	AelFrame& AelVariableDelay::processFrame(AelFrame& myframe){

		int rpi;
		float rp, frac;
		AelFrame next(channels), argument_copy = myframe;

		rp = position - vdelayLen;
		rp = (rp >= 0 ? (rp < maxDelayLen ? rp : rp - maxDelayLen) : rp + maxDelayLen);
		rpi = floor(rp);
		frac = rp - rpi;
		next = (rpi != maxDelayLen - 1 ? delay[rpi + 1] : delay[0]);

		for (int i = 0; i < myframe.getChannels(); i++){
			myframe[i] = delay[rpi][i] + frac*(next[i] - delay[rpi][i]);
		}

		delay[position] = argument_copy;
		position = (position != maxDelayLen - 1 ? position + 1 : 0);

		return myframe;
	}

	AelVariableDelay::~AelVariableDelay(){}





	////////////////////Chorus



	AelChorus::AelChorus(float dltime, float _depth, float _rate, float _center, float _sr, int n_channels) : center(_center), depth(_depth), rate(_rate), channels(n_channels), samplerate(_sr), AelVariableDelay(dltime, 0.7)
	{
	}

	AelAudioStream& AelChorus::processStream(AelAudioStream& mystream)
	{
		AelAudioStream *temp = new AelAudioStream(mystream.getchannels());
		int sampleLen = (1 / rate) * samplerate;

		for (int i = 0; i < mystream.getnframes(); i++){
			if (i == 15000)
			{
				vdelayLen -= 7500;
			}
			AelFrame aux = mystream.getNextFrame();
			aux = processFrame(aux);
			temp->AddFrames(aux);
		}

		return *temp;

	}

	AelChorus::~AelChorus(){}

}