#include "AelAudioBuf.h"
#include "sndfile.hh"
#include <cmath>
#include <iostream>

#define STREAM_LEN  ((m_nframes) * (channels))

Ael::AelAudioStream::AelAudioStream(string FileName)
{
	SndfileHandle file(FileName);

	if (file.error()){
		//throw FileError("Open File Error");
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
		//throw FileError("Allocating Error");
		currPosition = m_panStream = NULL;
	}

	peek = 0;
	for (int i = 0; i < m_nframes * channels; i++){
		if (abs(m_panStream[i]) > peek) peek = abs(m_panStream[i]);
	}


}

void Ael::AelAudioStream::SaveToFile(string file_name){
	SndfileHandle file = SndfileHandle(file_name, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, channels, sampleRate);
	file.write(m_panStream, STREAM_LEN);
}

Ael::AelAudioStream::~AelAudioStream()
{
	if (m_panStream) delete m_panStream;
}

