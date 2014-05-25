#include "AelAudioBuf.h"
#include "sndfile.hh"
#include <cmath>
#include <iostream>
#include <AelException.h>

#define STREAM_LEN  ((m_nframes) * (channels))

namespace Ael{

	//////////////////////////////////////////////
	// AUDIO BUFFER

	AelAudioStream::AelAudioStream(string FileName)
	{
		SndfileHandle file(FileName);

		if (file.error()){
			throw AelExecption("Open File Error");
			cout << "huehue";
		}

		m_nframes = file.frames();
		channels = file.channels();
		sampleRate = file.samplerate();

		try{

			m_panStream = new int[m_nframes * channels];
			currPosition = m_panStream;
			file.readf(m_panStream, m_nframes);

		}
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
			currPosition = m_panStream = NULL;
		}

		peek = 0;

		for (int i = 0; i < m_nframes * channels; i++){
			if (abs(m_panStream[i]) > peek) peek = abs(m_panStream[i]);
		}


	}

	void AelAudioStream::SaveToFile(string file_name){
		SndfileHandle file = SndfileHandle(file_name, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, channels, sampleRate);
		file.write(m_panStream, STREAM_LEN);
	}


	AelFrame& AelAudioStream::getNextFrame(){

		if (m_panStream + STREAM_LEN >= currPosition){
			throw AelExecption("No more Frames");
		}
		else{
			AelFrame* new_frame = new AelFrame(currPosition, channels);
			currPosition += channels;
			return *new_frame;
		}

	}

	AelAudioStream::~AelAudioStream()
	{
		if (m_panStream) delete m_panStream;
	}
	//////////////////////////////////////////////



	//////////////////////////////////////////////
	// AELFRAME

	AelFrame::AelFrame(int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {};
	AelFrame::AelFrame(int* arr, int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {

		for (int i = 0; i < n_channels; i++){
			samples[i] = arr[i];
		}
	}
	int& AelFrame::operator[](int i) {

		if (i<0 || i>n_channels)
			throw AelExecption("index out of range");
		else
			return samples[i];
	}

	AelFrame::~AelFrame(){}
	//////////////////////////////////////////////
}