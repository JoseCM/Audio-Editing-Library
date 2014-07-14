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
    

/////////////////AELCHANNEL
    
    int AelChannel::ID = 0;
    
    AelFrame AelChannel::getNextFrame(){
        
        if(isEOC())
            return AelFrame(stream.getchannels());
        
        AelFrame frame = stream.getNextFrame();
        
        for( AelEffect* &effect : effectChain )
            if(effect->isOn())
                effect->processFrame(frame);

        if(!isOn())
            return AelFrame(stream.getchannels());
        
        panner.processFrame(frame);
        volume.processFrame(frame);

        return frame;
        
    }
    
    AelAudioStream* AelChannel::getFullyProcessed(){
        
        AelAudioStream *new_stream = new AelAudioStream(2);
        list<AelEffect*> temp_list;
        
        int init_pos = stream.getCurrPosition();
        stream.rewind();
        
        for(auto it = effectChain.begin(); it != effectChain.end(); it++)
            temp_list.push_back((*it)->getCopy());
        

        for(int i = 0; i < stream.getnframes(); i++){
            
            AelFrame frame = stream.getNextFrame();
            
            for( AelEffect* &effect : temp_list)
                if(effect->isOn())
                    effect->processFrame(frame);
            
            panner.processFrame(frame);
            volume.processFrame(frame);
            
            new_stream->AddFrames(frame);
            
        }
        
        stream.setCurrPosition(init_pos);
        
        for(auto it = temp_list.begin(); it != temp_list.end(); it++)
            delete *it;
        
        new_stream->rewind();
        
        return new_stream;
    }
    
    bool AelChannel::removeEffect(int effectId){
        
        for(list<AelEffect*>::iterator it = effectChain.begin(); it != effectChain.end(); it++){
            
            if((*it)->getId() == effectId){
                effectChain.erase(it);
                return true;
            }
    
        }
        
        return false;
        
    }
    
    AelEffect* AelChannel::getEffect(int effectId){
        
        for(list<AelEffect*>::iterator it = effectChain.begin(); it != effectChain.end(); it++){
            
            if((*it)->getId() == effectId)
               return  *it;
            
        }
        
        return nullptr;
        
    }
    
    
    
//////////////AELMIXER
    
    /*AelChannel* AelMixer::addChannel(const string &filename){
        
        AelChannel *newchannel = new AelChannel(filename);
        channel_list.push_back(newchannel);
        
        if(newchannel->stream.getnframes() > m_nMaxFrames)
            m_nMaxFrames = newchannel->stream.getnframes();
        
        m_nChannels++;

        newchannel->stream.setCurrPosition(currPos);
        
        return newchannel;
        
    }*/

    AelChannel* AelMixer::addChannel(AelChannel *newchannel){

        channel_list.push_back(newchannel);

        if(newchannel->stream.getnframes() > m_nMaxFrames)
            m_nMaxFrames = newchannel->stream.getnframes();

        m_nChannels++;

        newchannel->stream.setCurrPosition(currPos);

        return newchannel;

    }
    
    AelChannel* AelMixer::getChannel(const int &channelID){
        
        list<AelChannel*>::iterator it = findChannel(channelID);
        
        if(it == channel_list.end())
            return NULL;
        
        return *it;
        
    }
    
    list<AelChannel*>::iterator AelMixer::findChannel(const int &channelID){
        
        for(list<AelChannel*>::iterator it = channel_list.begin(); it != channel_list.end(); it++){
            
            if((*it)->getID() == channelID){
                return it;
            }
            
        }
        
        return channel_list.end();
        
    }
    
    /*void AelMixer::addEffect(int channelID, AelEffect &effect) {
        
        list<AelChannel*>::iterator it = findChannel(channelID);
        
        if(it == channel_list.end())
            return;
        
        (*it)->addEffect(effect);
        
    }
    */
    
    bool AelMixer::removeChannel(const int &channelID){
        
        /*list<AelChannel*>::iterator it = findChannel(channelID);
        
        if(it == channel_list.end())
            return false;
        
        //delete *it;
        */
        list<AelChannel*>::iterator it;

        for( it = channel_list.begin(); it != channel_list.end(); it++){

            if((*it)->getID() == channelID)
               break;

        }

        if(it == channel_list.end())
            return false;

        channel_list.erase(it);
        
        m_nMaxFrames = 0;
        
        for(list<AelChannel*>::iterator iter = channel_list.begin(); iter != channel_list.end(); iter++){
            
            if((*iter)->stream.getnframes() > m_nMaxFrames)
                m_nMaxFrames = (*iter)->stream.getnframes();
            
        }
        
        m_nChannels--;
        
        return true;
        
    }
    
    void AelMixer::setPosMsec(int mseg){
        
        int frame_pos = (static_cast<float>(mseg)/1000.0) * 44100;
        
        setPosFrames(frame_pos);
        
    }
    
    void AelMixer::setPosFrames(int nframe){
        
        if(nframe < 0)
            return;
        
        for(list<AelChannel*>::iterator iter = channel_list.begin(); iter != channel_list.end(); iter++){
            (*iter)->stream.setCurrPosition(nframe);
        }
        
        currPos = nframe;
        
    }

    bool AelMixer::removeEffect(int effectId){

        for(list<AelEffect*>::iterator it = master_effects.begin(); it != master_effects.end(); it++){

            if((*it)->getId() == effectId){
                master_effects.erase(it);
                return true;
            }

        }

        return false;

    }

    
    
    AelFrame AelMixer::getNextFrame(){
        
        AelFrame new_frame(2);

        if(isEOM())
            return new_frame;

        for(AelChannel* &channel : channel_list){
            if((!channel->isEOC()))
                new_frame = new_frame + channel->getNextFrame();
        }
        
        for( AelEffect* &effect : master_effects )
            if(effect->isOn())
                effect->processFrame(new_frame);
        
        masterPan.processFrame(new_frame);
        masterVolDb.processFrame(new_frame);
        
        currPos++;
        
        return new_frame;
        
    }
    
    AelAudioStream* AelMixer::getFullMix(){
        
        
        list<AelAudioStream*> stream_list;
        list<AelEffect*> effect_list;
        AelAudioStream *fullmix = new AelAudioStream(2);
        int x = 0;
        
        int pos = getPosFrames();
        bool endflag = false;
        
        setPosFrames(0);
        
        for(AelChannel* &channel : channel_list)
            stream_list.push_back(channel->getFullyProcessed());
        
        for(auto it = master_effects.begin(); it != master_effects.end(); it++)
            effect_list.push_back((*it)->getCopy());
        
        while(!endflag){
            
            endflag = true;
            AelFrame tempframe(2);
            
            //if(x > 150000){
                
            //}
            
            for(AelAudioStream* &stream : stream_list){
                
                if(!(stream->isEOS())){
                    
                    tempframe = tempframe + stream->getNextFrame();
                    
                    for( AelEffect* &effect : effect_list){
                        if(effect->isOn())
                            effect->processFrame(tempframe);
                    }
                    
                    masterPan.processFrame(tempframe);
                    masterVolDb.processFrame(tempframe);
                    
                    endflag = false;
                    
                }
                
            }
            
            //x++;
            currPos++;
            if(!endflag)
                fullmix->AddFrames(tempframe);
        }
        
        setPosFrames(pos);
        
        for(auto it = effect_list.begin(); it != effect_list.end(); it++)
            delete *it;
        
        for(auto it = stream_list.begin(); it != stream_list.end(); it++)
            delete *it;
        
        return fullmix;
    }
    
}









