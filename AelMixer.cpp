//
//  AelMixer.cpp
//  AudioEditingLibrary
//
//  Created by José Martins on 31/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelMixer.h"
#include <cmath>

#define MAX_SAMPLE_VALUE ( (pow(2.0, 32.0)/2) - 1 )

namespace Ael {
    
    //AEL VOLUME
    
    double AelVolume::getVolume() { return volume; }
    
    void AelVolume::setVolume(double vol) {
        
        if(vol > 2.0 || vol < 0) return;
        
        this->volume = vol;
    }
    
    double AelVolume::getVolumeDb(){
        
        if(volume == 0) return -70.0;
        
        else return 20.0 * log(volume);
    }
    
    void AelVolume::setVolumeDb(double voldb){
        
        if(voldb > 6 || voldb < 70) return;
        
        volume = pow(10, voldb/20);
        
    }
    
    AelFrame& AelVolume::processFrame(AelFrame& frame){
        
        long int tempsample = 0;
        
        for(int i = 0; i < frame.getChannels(); i++){
            
            if(frame[i] == 0) continue;
            
            tempsample = frame[i];
            tempsample *= volume;
            
            if(abs(tempsample) <= MAX_SAMPLE_VALUE)
                frame[i] = static_cast<int>(tempsample);
            
            else if(frame[i] < 0)
                frame[i] = - MAX_SAMPLE_VALUE;
            
            else
                frame[i] = MAX_SAMPLE_VALUE;
            
        }
        
        return frame;
    }
    
    
    //AELPANNING
    //SÓ TEM SUPORTE PARA MONO E STEREO
    double AelPanner::getPan(){
        return pan;
    }
    
    void AelPanner::setPan(double pan){
        
        if(pan < -1.0 || pan > 1.0)
            return;
        
        this->pan = pan;
        panright = (sin((1 + pan) / 2.0 * M_PI / 2.0));
        panleft =  (sin((1 - pan) / 2.0 * M_PI / 2.0));
        
    }
    
    AelFrame& AelPanner::processFrame(AelFrame &frame){
        
        if(frame.getChannels() > 2)
            return frame;
        
        
        frame.toStereo();
        
        frame[0] *= panleft;
        frame[1] *= panright;
        
        return frame;
    }
    
    list<AelChannel*>::iterator AelMixer::findChannel(const int &channelID){
        
        for(list<AelChannel*>::iterator it = channel_list.begin(); it != channel_list.end(); it++){
            
            if((*it)->getID() == channelID){
                return it;
            }
            
        }
        
        return channel_list.end();
        
    }
    
    
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
    
    
    int AelMixer::addChannel(const string &filename){
        
        AelChannel *newchannel = new AelChannel(filename, m_nChannels++);
        channel_list.push_back(newchannel);
        
        return newchannel->getID();
        
    }
    
    AelChannel* AelMixer::getChannel(const int &channelID){
        
        list<AelChannel*>::iterator it = findChannel(channelID);
        
        if(it == channel_list.end())
            return NULL;
        
        return *it;
        
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
        return true;
        
    }
    
    
    
    AelFrame AelMixer::getNextFrame(){
        
        AelFrame new_frame(2);
        
        for(AelChannel* &channel : channel_list){
            if((!channel->isEOC()) && channel->isOn())
                new_frame = new_frame + channel->getNextFrame();
        }
        

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
                    endflag = false;
                }
            }
            
            if(!endflag)
                fullmix->AddFrames(tempframe);
        }
        
        return fullmix;
    }
    
    
    
}









