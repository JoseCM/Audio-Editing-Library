

#include "AelAudioBuf.h"
#include "sndfile.h"

namespace Ael{
    
    
////////////////////////////////////////////////////////////////////
// Classe AelFrame
////////////////////////////////////////////////////////////////////
    
    
    /*******************************************************************
     *Construtor (1) da Classe AelFrame
     *Parâmetros: int (n_ch)
     *Responsável por criar e inicializar uma nova Frame vazia, com
     *  tamanho igual ao número de canais
     *******************************************************************/
	AelFrame::AelFrame(int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {
        
		for(int i = 0; i < n_ch; i++)
			samples[i] = 0;
        
	}
	
    /*******************************************************************
     *Construtor (2) da Classe AelFrame
     *Parâmetros: int* (array), int (n_ch)
     *Responsável por criar uma nova frame a partir de um buffer de
     *  samples e do número de canais
     *******************************************************************/
	AelFrame::AelFrame(int* arr, int n_ch) : samples(new int[n_ch]), n_channels(n_ch) {
        
		for (int i = 0; i < n_channels; i++){
			samples[i] = arr[i];
		}
		
	}
	
    /*******************************************************************
     *Construtor Cópia da Classe AelFrame
     *Parâmetros: AelFrame&
     *******************************************************************/
    
	AelFrame::AelFrame(const AelFrame &source): samples(new int[source.n_channels]), n_channels(source.n_channels){
		
		for(int i = 0; i < n_channels; i++)
			samples[i] = source.samples[i];
		
	}
	
    /******************************************************************
     *Função membro getChannels da Classe AelFrame
     *Parâmetros: void
     *Retorna número de canais da Frame
     *******************************************************************/
	int AelFrame::getChannels() { return n_channels; }
	
    
    /******************************************************************
     *Overload do operador [] da Classe AelFrame
     *******************************************************************/
	int& AelFrame::operator[](int i) {
        
		if (i < 0 || i > n_channels)
			throw AelExecption("index out of range");
		else
			return samples[i];
	}
    
    /******************************************************************
     *Overload do operador [] da Classe AelFrame
     *******************************************************************/
	int AelFrame::operator[](int i) const{
		
		if (i < 0 || i > n_channels)
			throw AelExecption("index out of range");
		else
			return samples[i];
		
	}
	
    /******************************************************************
     *Função membro maxSample da Classe AelFrame
     *Parâmetros: void
     *Retorna amostra com valor absoluto máximo
     *******************************************************************/
	int AelFrame::maxSample() const{
		
		int max = 0;
		
		for(int i = 0; i < n_channels; i++)
			if(abs(samples[i]) > max)
				max = samples[i];
		
		return max;
	}
	
    /******************************************************************
     *Função membro toStereo da Classe AelFrame
     *Parâmetros: void
     *Converte Frames de 2 canais (stereo) numa Frame com 1 canal (mono)
     *******************************************************************/
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
	
    /******************************************************************
     *Overload do operador = da Classe AelFrame
     *******************************************************************/
	AelFrame& AelFrame::operator=(const AelFrame& from) {
		
		if(this->n_channels != from.n_channels)
			throw exception();
		
		for(int i = 0; i < n_channels; i++){
			(*this)[i] = from[i];
		}
		
		return *this;
	}
	
    /******************************************************************
     *Overload do operador + da Classe AelFrame
     *******************************************************************/
	AelFrame AelFrame::operator+(const AelFrame& to) const{
		
		if(this->n_channels != to.n_channels)
			throw exception();
		
		AelFrame frame(n_channels);
		long int tempsample;
		
		
		for(int i = 0; i < n_channels; i++){
			
			tempsample = static_cast<long>(samples[i]) + to[i];
			
			if(abs(tempsample) <= MAX_SAMPLE_VALUE)     //testa overflow e underflow
				frame.samples[i] = static_cast<int>(tempsample);
			
			else if(tempsample < 0)
				frame.samples[i] = - MAX_SAMPLE_VALUE;
			
			else
				frame.samples[i] = MAX_SAMPLE_VALUE;
			
		}
		
		return frame;
		
	}
	
    /******************************************************************
     *Overload do operador - da Classe AelFrame
     *******************************************************************/
	AelFrame AelFrame::operator-(const AelFrame& to) const{
		
		if(this->n_channels != to.n_channels)
			throw exception();
		
		AelFrame frame(n_channels);
		long int tempsample;
		
		
		for(int i = 0; i < n_channels; i++){
			
			tempsample = static_cast<long>(samples[i])  - to[i];
			
			if(abs(tempsample) <= MAX_SAMPLE_VALUE) //testa overflow e underflow
				frame.samples[i] = static_cast<int>(tempsample);
			
			else if(tempsample < 0)
				frame.samples[i] = - MAX_SAMPLE_VALUE;
			
			else
				frame.samples[i] = MAX_SAMPLE_VALUE;
			
		}
		
		return frame;
        
        
	}
    
    /******************************************************************
     *Overload do operador * da Classe AelFrame
     *******************************************************************/
	AelFrame AelFrame::operator*(float gain) const{
		
		AelFrame frame(n_channels);
		long int tempsample;
        
		for (int i = 0; i < n_channels; i++){
			
			tempsample = static_cast<long>(samples[i])  * gain;
			
			if(abs(tempsample) <= MAX_SAMPLE_VALUE)  //testa overflow e underflow
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

    
//////////////////////////////////////////////////////////////////
// Classe AelAudioStream
//////////////////////////////////////////////////////////////////
	
	int AelAudioStream::ID = 0;

    
    /******************************************************************
    *Construtor da Classe AelAudioStream 
    *Parâmetros: string (Filename)
    *Responsável por abrir ficheiro.wav, retirar stream de amostras e 
    *   colocar no audioFstream
    *******************************************************************/
    
	AelAudioStream::AelAudioStream(string FileName) : streamID(ID++), eos(false)
	{
		SndfileHandle file(FileName);
		int* aux_vector;
		audioFstream.open(std::to_string(streamID).c_str() , std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
		
		if(!audioFstream.is_open())
			throw AelExecption("AudioFstream Not Open");
		
		if (file.error()){
			throw AelExecption("Open File Error");
		}

		m_nframes = file.frames(); //leitura dos parâmetros caracteristicos do ficheiro .wav
		channels = file.channels();
		sampleRate = file.samplerate();
		currPosition = 0;

        if(sampleRate != 44100)
            throw AelExecption("SampleRate Error");
        
		try{
			
			aux_vector = new int[STREAM_LEN];
			file.readf(aux_vector, m_nframes);   //leitura de amostras para buffer aux_vector (readf)

			peak = 0;

			for (int i = 0; i < STREAM_LEN; i++){     //procura da amostra com máximo valor (peak)
				if (abs(aux_vector[i]) > peak) peak = abs(aux_vector[i]);
			}
			
			audioFstream.write(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);  //escrita das amostras no ficheiro audioFstream
			audioFstream.seekg(ios_base::beg);
			
	   
			delete [] aux_vector;
			
		}
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
		
	}
    
    
    /******************************************************************
     *Construtor (2) da Classe AelAudioStream
     *Parâmetros: int (n_canais), int (samplerate)
     *Responsável por abrir Ficheiro audioFstream (vazio)
     *******************************************************************/
    
	AelAudioStream::AelAudioStream(int nChannels, int nSampleRate) : streamID(ID++),  currPosition(0), channels(nChannels), sampleRate(nSampleRate), peak(0), m_nframes(0), eos(true){
		
		audioFstream.open( std::to_string(streamID).c_str() , std::fstream::binary | std::fstream::in | std::fstream::out | std::fstream::trunc );
		
	}
    
    /******************************************************************
     *Função membro AddFrames da classe AelAudioStream
     *Parâmetros: AelFrame&
     *Responsável por adicionar uma nova Frame ao ficheiro audioFstream
     *Retorna sucesso/insucesso da operação
     *******************************************************************/
	bool AelAudioStream::AddFrames(AelFrame& new_frame){
		
		if (new_frame.getChannels() != channels){
			return false;
		}
		
		audioFstream.write(reinterpret_cast<char*>(new_frame.samples), sizeof(int) * new_frame.n_channels);
		
		++m_nframes;
		
		eos = false;
		
		return true;
	}

    /******************************************************************
     *Função membro SaveToFile da classe AelAudioStream
     *Parâmetros: string (Filename)
     *Responsável por criar ficheiro .wav com a stream pretendida
     *******************************************************************/
	void AelAudioStream::SaveToFile(string file_name){
		
		int* aux_vector;

		try{
			
			aux_vector = new int[STREAM_LEN];
			streampos current_pos = audioFstream.tellg();    //guarda posição actual no audoFstream
			
			audioFstream.seekg(0, ios_base::beg);     //posiciona apontador para inicio do ficheiro
			audioFstream.read(reinterpret_cast<char*>(aux_vector), sizeof(int) * STREAM_LEN);   //leitura de bloco de amostras para buffer
                                                                                                //aux_vector
			
			SndfileHandle file = SndfileHandle(file_name, SFM_WRITE, SF_FORMAT_WAV | SF_FORMAT_PCM_32, channels, sampleRate); //cria ficheiro .wav
			file.write(aux_vector, STREAM_LEN);  //escrita do buffer aux_vector (amostras) no ficheiro .wav

			delete [] aux_vector;
			audioFstream.seekg(current_pos);  //reposiciona ficheiro na posição inicial
	
		}
		catch (bad_alloc& err){
			throw AelExecption("Allocating Error");
		}
		
	}
    
    
    /******************************************************************
     *Função membro SaveTo8051 da classe AelAudioStream
     *Parâmetros: string (Filename)
     *Responsável por guardar num SD card (posteriormente inserido
     *  no 8051) um ficheiro .wav com uma stream de amostras processadas,
     *  actualizando o ficheiro info (existente no cartão)
     *Retorna sucesso/insucesso da operação
     *******************************************************************/
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
    
    /******************************************************************
     *Função membro RemoveFrom8051 da classe AelAudioStream
     *Parâmetros: string (Filename)
     *Responsável por remover de um SD card (posteriormente inserido
     *  no 8051) um ficheiro .wav com uma stream de amostras processadas,
     *  actualizando o ficheiro info (existente no cartão)
     *Retorna sucesso/insucesso da operação
     *******************************************************************/
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

    /******************************************************************
     *Função membro getNextFrame() da classe AelAudioStream
     *Parâmetros: void
     *Responsável por obter próxima frame da stream para processamento
     *Retorna próxima Frame (sucesso) / Frame vazia (insucesso)
     *******************************************************************/
	AelFrame AelAudioStream::getNextFrame(){

		if (eos){     //fim da stream
			return AelFrame(channels);   //frame vazia
		}
		
		AelFrame new_frame(channels);
		
		audioFstream.read(reinterpret_cast<char*>(new_frame.samples), sizeof(int) * new_frame.n_channels); //leitura de uma frame do ficheiro audioFstream
		
		if(++currPosition == m_nframes) //incrementa posição actual na stream (audioFstream)
			eos = true;     //caso seja a última frame da stream
			
		return new_frame;
		
	}
	
    /******************************************************************
     *Função membro setCurrPosition() da classe AelAudioStream
     *Parâmetros: int (pos)
     *Responsável por posicionamento no ficheiro audioFstream
     *******************************************************************/
	void AelAudioStream::setCurrPosition(int pos){
		
		if(pos < 0)
			return;
		
		else if(pos >= m_nframes){
			audioFstream.seekg(audioFstream.end); //posicionamento no fim do ficheiro audioFstream
			eos = true;
		}
	
		else {
			audioFstream.seekg(pos, audioFstream.beg); //posicionamento no início do ficheiro audioFstream
			currPosition = pos;
			eos = false;
		}
		
	}
	
	/******************************************************************
     *Função membro rewind() da classe AelAudioStream
     *Parâmetros: void
     *Responsável por posicionar ficheiro audioFstream no início
     *******************************************************************/
	void AelAudioStream::rewind(){
		audioFstream.seekg(audioFstream.beg);
		currPosition = 0;
		eos = false;
	}
    

    /******************************************************************
     *Destrutor da classe AelAudioStream
     *******************************************************************/
	AelAudioStream::~AelAudioStream() {
	
		audioFstream.close();
		
        remove(to_string(streamID).c_str()); //remove ficheiro onde
                                            //são guardadas as amostras
		
		
	}
}
