#include "AelAudioBuf.h"
#include "sndfile.hh"
#include <cmath>
#include <iostream>
#include "AelException.h"

#define STREAM_LEN  ((m_nframes) * (channels))

namespace Ael{

	//////////////////////////////////////////////
	// AUDIO BUFFER

	AelAudioStream::AelAudioStream(string FileName)
	{
		SndfileHandle file(FileName);
		int* aux_vector;

		if (file.error()){
			throw AelExecption("Open File Error");
			cout << "huehue";
		}

		m_nframes = file.frames();
		channels = file.channels();
		sampleRate = file.samplerate();

		try{
			aux_vector = new int[STREAM_LEN];
			m_panStream.resize(STREAM_LEN);
			currPosition = 0;
			file.readf(aux_vector, m_nframes);

			for (int i = 0; i < STREAM_LEN; i++){
				m_panStream[i] = aux_vector[i];
			}

			peek = 0;

			for (int i = 0; i < STREAM_LEN; i++){
				if (abs(m_panStream[i]) > peek) peek = abs(m_panStream[i]);
			}

			delete [] aux_vector;
		}
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
			currPosition = 0;
		}
	}

	AelAudioStream::AelAudioStream(int nChannels, int nSampleRate) : currPosition(0), channels(nChannels),
	sampleRate(nSampleRate), peek(0), m_nframes(0){}

	bool AelAudioStream::AddFrames(AelFrame& new_frame){
		if (new_frame.getChannels() != channels){
			return false;
		}

		for (int i = 0; i < channels; i++){
			if (abs(new_frame[i]) > peek) peek = abs(new_frame[i]);
			m_panStream.push_back(new_frame[i]);
		}

		++m_nframes;
		return true;
	}

	void AelAudioStream::SaveToFile(string file_name){
		int* aux_vector;

		try{
			aux_vector = new int[STREAM_LEN];

			for (int i = 0; i < STREAM_LEN; i++){
				aux_vector[i] = m_panStream[i];
			}

			SndfileHandle file = SndfileHandle(file_name, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, channels, sampleRate);
			file.write(aux_vector, STREAM_LEN);

			delete [] aux_vector;
		}
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
		
	}


	AelFrame& AelAudioStream::getNextFrame(){

		if (STREAM_LEN <= currPosition){
			throw AelExecption("No more Frames");
		}
		else {
			AelFrame* new_frame = new AelFrame(channels);
			for (int i = 0; i < channels; i++)
				new_frame[i] = m_panStream[currPosition++];
			return *new_frame;
		}

	}

	AelAudioStream::~AelAudioStream() {}
	//////////////////////////////////////////////



	//////////////////////////////////////////////
	// AELFRAME

	AelFrame::AelFrame(int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {};
	AelFrame::AelFrame(int* arr, int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {

		for (int i = 0; i < n_channels; i++){
			samples[i] = arr[i];
		}
	}

	int AelFrame::getChannels() { return n_channels; }
	int& AelFrame::operator[](int i) {

		if (i<0 || i>n_channels)
			throw AelExecption("index out of range");
		else
			return samples[i];
	}

	AelFrame::~AelFrame(){}
	//////////////////////////////////////////////
}