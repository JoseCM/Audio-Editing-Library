

#include "AelAudioBuf.h"


namespace Ael{

	//////////////////////////////////////////////
	// AUDIO STREAM
    
    int AelAudioStream::ID = 0;

	AelAudioStream::AelAudioStream(string FileName) : streamID(ID++), eos(false)
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

			peak = 0;

			for (int i = 0; i < STREAM_LEN; i++){
				if (abs(aux_vector[i]) > peak) peak = abs(aux_vector[i]);
			}
            
            audioFstream.write(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);
            audioFstream.seekg(ios_base::beg);
            
       
			delete [] aux_vector;
            
		}
        
        
        
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
        
	}

	AelAudioStream::AelAudioStream(int nChannels, int nSampleRate) : streamID(ID++),  currPosition(0), channels(nChannels), sampleRate(nSampleRate), peak(0), m_nframes(0), eos(true){
        
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
        
        eos = false;
        
		return true;
	}

	void AelAudioStream::SaveToFile(string file_name){
        
		int* aux_vector;

		try{
            
			aux_vector = new int[STREAM_LEN];
            streampos current_pos = audioFstream.tellg();
            
			audioFstream.seekg(0, ios_base::beg);
            audioFstream.read(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);
            
            
			SndfileHandle file = SndfileHandle(file_name, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, channels, sampleRate);
			file.write(aux_vector, STREAM_LEN);

			delete [] aux_vector;
            audioFstream.seekg(current_pos);
    
		}
        
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
		
	}


	AelFrame AelAudioStream::getNextFrame(){

		if (eos){
			//throw AelExecption("No more Frames");
            return AelFrame(channels);
		}
		
        AelFrame new_frame(channels);
    
        //if(audioFstream.tellg() != sizeof(int) * channels  * currPosition)
        //    audioFstream.seekg(sizeof(int) * channels  * currPosition, ios_base::beg);
        
        audioFstream.read(reinterpret_cast<char*>(new_frame.samples), sizeof(int) * new_frame.n_channels);
        
        if(++currPosition == m_nframes)
            eos = true;
            
        return new_frame;
		
	}
    
    void AelAudioStream::setCurrPosition(int pos){
        
        if(pos < 0)
            return;
        
        else if(pos >= m_nframes){
            audioFstream.seekg(audioFstream.end);
            eos = true;
        }
    
        else {
            audioFstream.seekg(currPosition, audioFstream.beg);
            currPosition = pos;
            eos = false;
        }
        
    }
    
    
    void AelAudioStream::rewind(){
        audioFstream.seekg(audioFstream.beg);
        currPosition = 0;
        eos = false;
    }
    
    

	AelAudioStream::~AelAudioStream() {
    
        audioFstream.close();
        
        
        if(remove(to_string(streamID).c_str())){ //ATENÇAO COLOCAR PROTEÇOES
            //cout << "nao foi possivel apagar o ficheiro " << to_string(streamID).c_str() << endl;
        }
        //else
            //cout << "ficheiro apagado com sucesso " << to_string(streamID).c_str() << endl;
        
        
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
            samples[i] = source.samples[i];
        
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
    
    int AelFrame::maxSample() const{
        
        int max = 0;
        
        for(int i = 0; i < n_channels; i++)
            if(abs(samples[i]) > max)
                max = samples[i];
        
        return max;
    }
    
    void AelFrame::toStereo(){
        
        if(n_channels <= 0)
            return;
        
        int tempsamples[2];
        
        if(n_channels == 1)
            tempsamples[0] = tempsamples[1] = samples[0];
        else{
            tempsamples[0] = samples[0];
            tempsamples[1] = samples[1];
        }
        
        delete[] samples;
        samples = new int[2];
        
        samples[0] = tempsamples[0];
        samples[1] = tempsamples[1];
        
        n_channels = 2;
        
    }
    
    AelFrame& AelFrame::operator=(const AelFrame& from) {
        
        if(this->n_channels != from.n_channels)
            throw exception();
        
        for(int i = 0; i < n_channels; i++){
            (*this)[i] = from[i];
        }
        
        return *this;
    }
    
    AelFrame AelFrame::operator+(const AelFrame& to) const{
        
        if(this->n_channels != to.n_channels)
            throw exception();
        
        AelFrame frame(n_channels);
        long int tempsample;
        
        
        for(int i = 0; i < n_channels; i++){
            
            tempsample = static_cast<long>(samples[i]) + to[i];
            
            if(abs(tempsample) <= MAX_SAMPLE_VALUE)
                frame.samples[i] = static_cast<int>(tempsample);
            
            else if(tempsample < 0)
                frame.samples[i] = - MAX_SAMPLE_VALUE;
            
            else
                frame.samples[i] = MAX_SAMPLE_VALUE;
            
        }
        
        return frame;
        
    }
    
    AelFrame AelFrame::operator-(const AelFrame& to) const{
        
        if(this->n_channels != to.n_channels)
            throw exception();
        
        AelFrame frame(n_channels);
        long int tempsample;
        
        
        for(int i = 0; i < n_channels; i++){
            
            tempsample = static_cast<long>(samples[i])  - to[i];
            
            if(abs(tempsample) <= MAX_SAMPLE_VALUE)
                frame.samples[i] = static_cast<int>(tempsample);
            
            else if(tempsample < 0)
                frame.samples[i] = - MAX_SAMPLE_VALUE;
            
            else
                frame.samples[i] = MAX_SAMPLE_VALUE;
            
        }
        
        return frame;
    
    
    }

	AelFrame AelFrame::operator*(float gain) const{
		
		AelFrame frame(n_channels);
        long int tempsample;

		for (int i = 0; i < n_channels; i++){
            
            tempsample = static_cast<long>(samples[i])  * gain;
            
            if(abs(tempsample) <= MAX_SAMPLE_VALUE)
                frame.samples[i] = static_cast<int>(tempsample);
            
            else if(tempsample < 0)
                frame.samples[i] = - MAX_SAMPLE_VALUE;
            
            else
                frame.samples[i] = MAX_SAMPLE_VALUE;
		}

		return frame;
	}
    
	AelFrame::~AelFrame(){
        delete[] samples;
    }
	//////////////////////////////////////////////
}