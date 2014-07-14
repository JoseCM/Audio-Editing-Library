

#include "AelAudioBuf.h"
#include "sndfile.h"

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
    
    bool AelAudioStream::SaveTo8051(string FileName){
        
        fstream fileinfo;
        string path, musicname, infopath;
        int index;
        unsigned char num_musics = 0, id = 0, count = 0;
        char name_temp[13] = {0};
        
        
        if ( ( index = static_cast<int>( FileName.find_last_of('/') ) ) == string::npos)
            if( ( index = static_cast<int>(FileName.find_last_of('\\') ) ) == string::npos)
                    return false;
        
        path = FileName.substr(0, index );
        musicname = FileName.substr(index+1,12);
        //musicname[musicname.length()] = 0;
        for(int i = 0 ; i<=3;i++) musicname.pop_back();
        musicname += ".ael";
        
        std::transform(musicname.begin(), musicname.end(),musicname.begin(), ::toupper);
        const char* vtr = musicname.c_str();
        char vtr2[13] = {0};
        
        strcpy(vtr2, vtr);

        infopath = path + "/info";
        
        fileinfo.open(infopath, std::fstream::binary | std::fstream::in | std::fstream::out );
        
        if(!fileinfo.is_open()) {
            return false;
        }
        
        fileinfo.seekg(0,ios_base::beg);
        
        fileinfo >> num_musics;
        
        while(1){
            if(count++ == num_musics ) {
                fileinfo << id;
                fileinfo.write(vtr2, 13);
                fileinfo.seekp(0, ios_base::beg);
                fileinfo << ++num_musics;
                
                break;
                
            }
            fileinfo >> id;
            fileinfo.read(name_temp, 13);
            id++;
            if(strcmp(name_temp, musicname.c_str()) == 0) break;
        }
        
        fileinfo.close();
        //------------------------------------------
        string musicpath = path + "/" + musicname;
        
        SndfileHandle file = SndfileHandle(musicpath, SFM_WRITE, SF_FORMAT_RAW | SF_FORMAT_PCM_U8, 1, 5555);
        
        int *samples, *mono;
        int *aux_vector;
    
        try{
            aux_vector = new int[STREAM_LEN];
			streampos current_pos = audioFstream.tellg();
			
			audioFstream.seekg(0, ios_base::beg);
			audioFstream.read(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);
            
            mono = new int[m_nframes];
            samples = new int[m_nframes / 8];
            
            for(int i = 0; i < m_nframes; i++)
                mono[i] = aux_vector[i*2];
            
            int id=0;
            for(int i=0; i < m_nframes; i=i+8 )
                samples[id++] = mono[i];
            
            file.write(samples, m_nframes / 8);
            
			delete [] aux_vector;
            delete [] mono;
            delete [] samples;
            
			audioFstream.seekg(current_pos);
        
        }
        catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
            return false;
		}
     
        return true;
    }
    
    bool AelAudioStream::RemoveFrom8051(string FileName){
        
        fstream fileinfo;
        
        string path, musicname, infopath;
        int index;
        unsigned char num_musics = 0;
        
        
        if ( ( index = static_cast<int>( FileName.find_last_of('/') ) ) == string::npos)
            if( ( index = static_cast<int>(FileName.find_last_of('\\') ) ) == string::npos)
                return false;
        
        path = FileName.substr(0, index );
        musicname = FileName.substr(index+1,13);
        
        std::transform(musicname.begin(), musicname.end(),musicname.begin(), ::toupper);
        const char* vtr = musicname.c_str();
        char vtr2[13] = {0};
        
        strcpy(vtr2, vtr);
        
        infopath = path + "/info";
        string musicpath = path + "/" + musicname;
        
        fileinfo.open(infopath, std::fstream::binary | std::fstream::app | std::fstream::in | std::fstream::out );
        
        if(!fileinfo.is_open()) {
            return false;
        }
        
        fileinfo.seekg(0,ios_base::beg);
        
        fileinfo >> num_musics;
        
        if(!num_musics) return false;
        
        char *buffer1, *name;
        
        try{
            buffer1 = new char [num_musics * 14];
            name = new char [13];
            fileinfo.read(buffer1, num_musics * 14);
            
            for(int i=0; i < num_musics ; i++){
                
               for(int j=0; j<13; j++){
                   name[j] = buffer1[i*14+j+1];
               }
                
               if(!strcmp(name,musicname.c_str())){
                   
                   for(int z = i*14; z< num_musics*14-14; z++)
                       buffer1[z] = buffer1[z+14];
                   
                   for(int z = 0; z<num_musics;z++){
                       buffer1[z*14] = z;
                   }
                   num_musics -=1 ;
                   
                   fileinfo.close();
                   
                   fileinfo.open(infopath, std::fstream::binary | std::fstream::trunc | std::fstream::out | std::fstream::in );
                   fileinfo << num_musics;
                   fileinfo.write(buffer1, num_musics * 14);
                   
                   fileinfo.close();
                   
                   remove(musicpath.c_str());
                   
                   return true;
                   
               }
                    
            }
        
        }
        catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
            return false;
		}
    
        
        return false;
    }

	AelFrame AelAudioStream::getNextFrame(){

		if (eos){
			//throw AelExecption("No more Frames");
			return AelFrame(channels);
		}
		
		AelFrame new_frame(channels);
		
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
			audioFstream.seekg(pos, audioFstream.beg);
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
