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
    
    AelFrame AelChannel::getNextFrame(){
        
        if(eoc == true)
            return AelFrame(stream.getchannels());
        
        AelFrame frame = stream.getNextFrame();
        
        for( AelEffect* &effect : effectChain )
            if(effect->isOn())
                effect->processFrame(frame);
        
        panner.processFrame(frame);
        volume.processFrame(frame);
        
        if(stream.isEOS())
            eoc = true;
        
        return frame;
        
    }
    
    AelAudioStream* AelChannel::getFullyProcessed(){
        
        AelAudioStream *new_stream = new AelAudioStream(2);
        int init_pos = stream.getCurrPosition();
        
        stream.rewind();
        
        while(!eoc){
            AelFrame frame = getNextFrame();
            new_stream->AddFrames(frame);
        }
        
        stream.setCurrPosition(init_pos);
        
        
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
    
    AelChannel* AelMixer::addChannel(const string &filename){
        
        AelChannel *newchannel = new AelChannel(filename, m_nChannels++);
        channel_list.push_back(newchannel);
        
        if(newchannel->stream.getnframes() > m_nMaxFrames)
            m_nMaxFrames = newchannel->stream.getnframes();
        
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
        
        list<AelChannel*>::iterator it = findChannel(channelID);
        
        if(it == channel_list.end())
            return false;
        
        delete *it;
        channel_list.erase(it);
        
        m_nMaxFrames = 0;
        
        for(list<AelChannel*>::iterator iter = channel_list.begin(); iter != channel_list.end(); iter++){
            
            if((*iter)->stream.getnframes() > m_nMaxFrames)
                m_nMaxFrames = (*iter)->stream.getnframes();
            
        }
        
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
    
    
    AelFrame AelMixer::getNextFrame(){
        
        AelFrame new_frame(2);
        
        for(AelChannel* &channel : channel_list){
            if((!channel->isEOC()) && channel->isOn())
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
        
        AelAudioStream *fullmix = new AelAudioStream(2);
        
        bool endflag = false;
        
        while(!endflag){
            
            endflag = true;
            AelFrame tempframe(2);
            
            for(AelChannel* &channel : channel_list){
                if(!(channel->isEOC())){
                    tempframe = tempframe + channel->getNextFrame();
                    masterPan.processFrame(tempframe);
                    masterVolDb.processFrame(tempframe);
                    endflag = false;
                }
            }
            
            if(!endflag)
                fullmix->AddFrames(tempframe);
        }
        
        setPosFrames(currPos);
        
        return fullmix;
    }
    
}









