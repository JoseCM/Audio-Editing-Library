//
//  AelMixer.cpp
//  AudioEditingLibrary
//
//  Created by José Martins on 31/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelUtilities.h"
#include "AelEffects.h"
#include "AelMixer.h"


namespace Ael {
    

//////////////////////////////////////////////////////////////////
//Classe AelChannel
//////////////////////////////////////////////////////////////////
    
    int AelChannel::ID = 0;
    
    
    /******************************************************************
     *Função membro getNextFrame da classe AelChannel
     *Parâmetros: void
     *Responsável por processar próxima frame de um canal por todos os
     *efeitos no estado ON. 
     *Retorna frame processada (sucesso) /frame vazia (insucesso).
     *******************************************************************/
    AelFrame AelChannel::getNextFrame(){
        
        if(isEOC())      //caso canal chegue ao fim
            return AelFrame(stream.getchannels());    //retorna frame vazia
        
        AelFrame frame = stream.getNextFrame();  //obtem próxima frame a ser processada
        
        for( AelEffect* &effect : effectChain )
            if(effect->isOn())
                effect->processFrame(frame);    //Frame é processada por todos os efeitos noe stado ON
                                                //presentes na lista de efeitos do canal (effectChain)

        if(!isOn())                     //caso canal não esteja ON
            return AelFrame(stream.getchannels());   //retorn frame vazia
        
        panner.processFrame(frame);
        volume.processFrame(frame);

        return frame;
        
    }
    
    /******************************************************************
     *Função membro getFullyProcessed da classe AelChannel
     *Parâmetros: void
     *Responsável por processar todas as amostras da stream do canal
     *pela lista de efeitos.
     *Retorna Stream do Canal processada.
     *******************************************************************/
    AelAudioStream* AelChannel::getFullyProcessed(){
        
        AelAudioStream *new_stream = new AelAudioStream(2);
        list<AelEffect*> temp_list;
        
        int init_pos = stream.getCurrPosition();    //obtem posição actual da stream do canal no ficheiro audioFstream
        stream.rewind();    //posicionamento para o início do ficheiro
        
        for(auto it = effectChain.begin(); it != effectChain.end(); it++)  //cria lista de cópias dos efeitos
            temp_list.push_back((*it)->getCopy());
        

        for(int i = 0; i < stream.getnframes(); i++){   //processa todas as frames da stream do canal por todos os efeito copia do canal
            
            AelFrame frame = stream.getNextFrame();
            
            for( AelEffect* &effect : temp_list)
                if(effect->isOn())
                    effect->processFrame(frame);
            
            panner.processFrame(frame);
            volume.processFrame(frame);
            
            new_stream->AddFrames(frame);  //adiciona frame processada à nova stream
            
        }
        
        stream.setCurrPosition(init_pos);    //reposicionamento da posição no ficheiro da stream
        
        for(auto it = temp_list.begin(); it != temp_list.end(); it++)   //elimina lista de efeitos cópia
            delete *it;
        
        new_stream->rewind();    //posiciona apontador no ficheiro da nova stream para o início
        
        return new_stream;
    }
    
    
    /******************************************************************
     *Função membro removeEffect da classe AelChannel
     *Parâmetros: int (effectId)
     *Função responsável por remover efeito da lista de efeitos do canal.
     *Procura do efeito a remover pelo ID.
     *Retorna sucesso/insucesso da operação.
     *******************************************************************/
    bool AelChannel::removeEffect(int effectId){
        
        for(list<AelEffect*>::iterator it = effectChain.begin(); it != effectChain.end(); it++){
            
            if((*it)->getId() == effectId){
                effectChain.erase(it);
                return true;
            }
    
        }
        
        return false;
        
    }
    
    /******************************************************************
     *Função membro getEffect da classe AelChannel
     *Parâmetros: int (effectId)
     *Responsável por obter efeito da lista de efeitos do canal. Procura
     *do efeito pelo ID.
     *Retorna efeito (sucesso)/ nullptr (insucesso)
     *******************************************************************/
    AelEffect* AelChannel::getEffect(int effectId){
        
        for(list<AelEffect*>::iterator it = effectChain.begin(); it != effectChain.end(); it++){
            
            if((*it)->getId() == effectId)
               return  *it;
            
        }
        
        return nullptr;
        
    }
    
    
    

//////////////////////////////////////////////////////////////////
// Classe AelMixer
//////////////////////////////////////////////////////////////////
    
    
    /*AelChannel* AelMixer::addChannel(const string &filename){
        
        AelChannel *newchannel = new AelChannel(filename);
        channel_list.push_back(newchannel);
        
        if(newchannel->stream.getnframes() > m_nMaxFrames)
            m_nMaxFrames = newchannel->stream.getnframes();
        
        m_nChannels++;

        newchannel->stream.setCurrPosition(currPos);
        
        return newchannel;
        
    }*/

    
    /******************************************************************
     *Função membro addChannel da classe AelMixer
     *Parâmetros: AelChannel*
     *Responsável por adicionar novo Canal à lista de canais da mixer
     *Retorna canal adicionado (com a stream reposicionada)
     *******************************************************************/
    AelChannel* AelMixer::addChannel(AelChannel *newchannel){

        channel_list.push_back(newchannel);   //adiciona novo canal na lista de canais da mixer

        if(newchannel->stream.getnframes() > m_nMaxFrames)    //actualiza número máximo de frames (num dado canal) presente na mixer
            m_nMaxFrames = newchannel->stream.getnframes();

        m_nChannels++;

        newchannel->stream.setCurrPosition(currPos);

        return newchannel;

    }
    
    /******************************************************************
     *Função membro addChannel da classe AelMixer
     *Parâmetros: const int & (channel ID)
     *Responsável por procurar e retornar um canal da mixer (pelo ID)
     *******************************************************************/
    AelChannel* AelMixer::getChannel(const int &channelID){
        
        list<AelChannel*>::iterator it = findChannel(channelID);
        
        if(it == channel_list.end())
            return NULL;
        
        return *it;
        
    }
    
    
    /******************************************************************
     *Função membro findChannel da classe AelMixer
     *Parâmetros: cont int & (channelID)
     *Responsável por procurar e retornar um canal da mixer (pelo ID)
     *******************************************************************/
    list<AelChannel*>::iterator AelMixer::findChannel(const int &channelID){
        
        for(list<AelChannel*>::iterator it = channel_list.begin(); it != channel_list.end(); it++){
            
            if((*it)->getID() == channelID){
                return it;
            }
            
        }
        
        return channel_list.end();
        
    }
    
    /******************************************************************
     *Função membro removeChannel da classe AelMixer
     *Parâmetros: const int & (channelID)
     *Responsável por procurar e remover um canal da mixer (pelo ID)
     *Retorna sucesso/insucesso da operação
     *******************************************************************/
    bool AelMixer::removeChannel(const int &channelID){
        
        list<AelChannel*>::iterator it;

        for( it = channel_list.begin(); it != channel_list.end(); it++){

            if((*it)->getID() == channelID)  //sai do ciclo quando encontra o canal pretendido
               break;

        }

        if(it == channel_list.end())  //sai da rotina caso não encontre o canal
            return false;

        channel_list.erase(it);   //apaga o canal da lista de canais da mixer
        
        m_nMaxFrames = 0;
        
        for(list<AelChannel*>::iterator iter = channel_list.begin(); iter != channel_list.end(); iter++){
            
            if((*iter)->stream.getnframes() > m_nMaxFrames)   //nova procura do número máximo de frames
                m_nMaxFrames = (*iter)->stream.getnframes();
            
        }
        
        m_nChannels--;
        
        return true;
        
    }
    
    /******************************************************************
     *Função membro setPosMsec da classe AelMixer
     *Parâmetros: int (mseg)
     *Responsável por posicionar a mixer em milisegundos
     *******************************************************************/
    void AelMixer::setPosMsec(int mseg){
        
        int frame_pos = (static_cast<float>(mseg)/1000.0) * 44100;
        
        setPosFrames(frame_pos);
        
    }
    
    /******************************************************************
     *Função membro setPosMsec da classe AelMixer
     *Parâmetros: int (nframe)
     *Responsável por posicionar todos os canais da mixer na posição 
     *  recebida como argumento
     *******************************************************************/
    void AelMixer::setPosFrames(int nframe){
        
        if(nframe < 0)
            return;
        
        for(list<AelChannel*>::iterator iter = channel_list.begin(); iter != channel_list.end(); iter++){
            (*iter)->stream.setCurrPosition(nframe);
        }
        
        currPos = nframe;
    }
    
    
    /******************************************************************
     *Função membro removeEffect da classe AelMixer
     *Parâmetros: int (effectId)
     *Responsável por apagar um efeito da lista de efeitos master da
     *mixer. A procura é feito pelo ID do efeito.
     *Retorna sucesso/insucesso da operação.
     *******************************************************************/
    bool AelMixer::removeEffect(int effectId){

        for(list<AelEffect*>::iterator it = master_effects.begin(); it != master_effects.end(); it++){

            if((*it)->getId() == effectId){
                master_effects.erase(it);
                return true;
            }

        }

        return false;

    }

    
    /******************************************************************
     *Função membro getNextFrame da classe AelMixer
     *Parâmetros: void
     *Responsável por retornar à mixer a próxima frame a reproduzir,
     * sendo esta uma soma de frames de todos os canais processada pelos
     * efeitos master
     *Retorna frame processada (sucesso) / frame vazia (insucesso)
     *******************************************************************/
    AelFrame AelMixer::getNextFrame(){
        
        AelFrame new_frame(2);

        if(isEOM())       //retorna frame vazia caso a mixer esteja no fim
            return new_frame;

        for(AelChannel* &channel : channel_list){    //percorre lista de canais da mixer
            if((!channel->isEOC()))
                new_frame = new_frame + channel->getNextFrame();   //soma todas as frames dos diferentes canais
        }
        
        for( AelEffect* &effect : master_effects )    //percorre lista de efeitos master da mixer
            if(effect->isOn())
                effect->processFrame(new_frame);      //new_frame é processada por todos os efeitos no estado ON
        
        masterPan.processFrame(new_frame);    //processamento do Pan
        masterVolDb.processFrame(new_frame);  //processamento do Volume
        
        currPos++;
        
        return new_frame;    //retorna frame processada à mixer
        
    }
    
    
    
    /******************************************************************
     *Função membro getFullMix da classe AelMixer
     *Parâmetros: void
     *Responsável por misturar todas as frames de todos os canais 
     *  já processados. 
     *Retorna stream final à mixer.
     *******************************************************************/
    AelAudioStream* AelMixer::getFullMix(){
        
        
        list<AelAudioStream*> stream_list;   //lista de streams
        list<AelEffect*> effect_list;        //lista de efeitos (para cópia)
        AelAudioStream *fullmix = new AelAudioStream(2);
      
        int pos = getPosFrames();     //guarda posição actual da mixer
        bool endflag = false;
        
        setPosFrames(0);      //posiciona a mixer na posiçãpo de início
        
        for(AelChannel* &channel : channel_list)    //percorre lista de canais da mixer
            stream_list.push_back(channel->getFullyProcessed());  //processa streams respectivas e guarda
                                                                //na lista de streams (stream_list)
        
        for(auto it = master_effects.begin(); it != master_effects.end(); it++)  //percorre lista efeitos master
            effect_list.push_back((*it)->getCopy());                         //cópia dos efeitos para nova lista (effect_list)
        
        while(!endflag){
        
            endflag = true;
            AelFrame tempframe(2);
            
            for(AelAudioStream* &stream : stream_list){   //percorre lista de streams processadas (dos diferentes canais)
                
                if(!(stream->isEOS())){   //caso a stream não esteja no fim
                    
                    tempframe = tempframe + stream->getNextFrame();   //soma a frame da stream
                    
                    for( AelEffect* &effect : effect_list){       //percorre lista de efeitos master cópia
                        if(effect->isOn())
                            effect->processFrame(tempframe);    //processa a respectiva frame
                    }
                    
                    masterPan.processFrame(tempframe);     //pan
                    masterVolDb.processFrame(tempframe);   //volume
                    
                    endflag = false;   //coloca flag a false para obter próxima frame processada da lista de streams
                    
                }
                
            }
            
            
            currPos++;
            if(!endflag)
                fullmix->AddFrames(tempframe);      //adiciona frame processada à stream final (fullmix)
        }
        
        setPosFrames(pos);
        
        for(auto it = effect_list.begin(); it != effect_list.end(); it++)    //elimina lista de efeitos cópia
            delete *it;
        
        for(auto it = stream_list.begin(); it != stream_list.end(); it++)    //elimina lista de streams processadas por cada canal
            delete *it;
        
        return fullmix;    //retorna stream final processada
    }
    
}









