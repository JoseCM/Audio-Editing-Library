#include "AelAudioBuf.h"
#include "sndfile.hh"
#include <cmath>
#include <iostream>
#include "AelException.h"

#define STREAM_LEN  ((m_nframes) * (channels))

namespace Ael{

	//////////////////////////////////////////////
	// AUDIO BUFFER
    
    int AelAudioStream::ID = 0;

	AelAudioStream::AelAudioStream(string FileName) : streamID(ID++)
	{
		SndfileHandle file(FileName);
		int* aux_vector;
        audioFstream.open(std::to_string(streamID).c_str() , std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
        
        if(!audioFstream.is_open())
            cout << "nao abriu" << endl;
        
		if (file.error()){
			throw AelExecption("Open File Error");
			cout << "Erro ao abrir" << endl;
		}

		m_nframes = file.frames();
		channels = file.channels();
		sampleRate = file.samplerate();
        currPosition = 0;

		try{
            
			aux_vector = new int[STREAM_LEN];
			file.readf(aux_vector, m_nframes);

			peek = 0;

			for (int i = 0; i < STREAM_LEN; i++){
				if (abs(aux_vector[i]) > peek) peek = abs(aux_vector[i]);
			}
            
            audioFstream.write(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);
            audioFstream.seekg(ios_base::beg);
            
       
			delete [] aux_vector;
            
		}
        
        
        
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
        
	}

	AelAudioStream::AelAudioStream(int nChannels, int nSampleRate) : streamID(ID++),  currPosition(0), channels(nChannels), sampleRate(nSampleRate), peek(0), m_nframes(0){
        
        audioFstream.open( std::to_string(streamID).c_str() , std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
        
    }

	bool AelAudioStream::AddFrames(AelFrame& new_frame){
        
		if (new_frame.getChannels() != channels){
			return false;
		}
    
        
        //if(audioFstream.tellp() != sizeof(int) * STREAM_LEN)
        //   audioFstream.seekp(0, ios_base::end); // prcber se fica a apontar para o end of file, antes ou depois !!!
        
        audioFstream.write(reinterpret_cast<char*>(new_frame.samples), sizeof(int) * new_frame.n_channels);
        
		++m_nframes;
        
		return true;
	}

	void AelAudioStream::SaveToFile(string file_name){
        
		int* aux_vector;

		try{
			aux_vector = new int[STREAM_LEN];
            
            
			audioFstream.seekg(0, ios_base::beg);
            audioFstream.read(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);
            
            
			SndfileHandle file = SndfileHandle(file_name, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, channels, sampleRate);
			file.write(aux_vector, STREAM_LEN);

			delete [] aux_vector;
    
		}
        
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
		
	}


	AelFrame AelAudioStream::getNextFrame(){

		if (STREAM_LEN <= currPosition){
			throw AelExecption("No more Frames");
		}
		
        AelFrame new_frame(channels);
    
        //if(audioFstream.tellg() != sizeof(int) * channels  * currPosition)
        //    audioFstream.seekg(sizeof(int) * channels  * currPosition, ios_base::beg);
        
        audioFstream.read(reinterpret_cast<char*>(new_frame.samples), sizeof(int) * new_frame.n_channels);
        
        currPosition++;
            
        return new_frame;
		
	}

	AelAudioStream::~AelAudioStream() {
    
        audioFstream.close();
        
        remove(to_string(streamID).c_str());
        
    }
    
	//////////////////////////////////////////////
	// AELFRAME

	AelFrame::AelFrame(int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {
    
        for(int i = 0; i < n_ch; i++)
            samples[i] = 0;
    
    }
    
	AelFrame::AelFrame(int* arr, int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {

		for (int i = 0; i < n_channels; i++){
			samples[i] = arr[i];
		}
        
	}
    
    AelFrame::AelFrame(const AelFrame &source): samples(new int[source.n_channels]), n_channels(source.n_channels){
        
        for(int i = 0; i < n_channels; i++)
            samples[i] = source[i];
        
    }
    
	int AelFrame::getChannels() { return n_channels; }
    
	int& AelFrame::operator[](int i) {

		if (i < 0 || i > n_channels)
			throw AelExecption("index out of range");
		else
			return samples[i];
	}

    int AelFrame::operator[](int i) const{
        
        if (i < 0 || i > n_channels)
			throw AelExecption("index out of range");
		else
			return samples[i];
        
    }
    
	AelFrame::~AelFrame(){
        delete[] samples;
    }
	//////////////////////////////////////////////
}