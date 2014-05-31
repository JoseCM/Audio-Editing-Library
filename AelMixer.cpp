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
    
    
    
    
    AelAudioStream& AelVolume::processStream(AelAudioStream &stream){
        
        AelAudioStream *temp = new AelAudioStream(stream.getchannels());
        
        for(int i = 0; i < stream.getnframes(); i++){
            AelFrame aux = stream.getNextFrame();
            aux = this->processFrame(aux);
            temp->AddFrames(aux);
            
        }
        
        return *temp;
        
    }
    
    //AELPANNING
    //SÓ TEM SUPORTE PARA MONO E STEREO
    double AelPanner::getPan(){
        return pan;
    }
    
    void AelPanner::setPan(double pan){
        
        if(pan < -1 || pan > -1)
            return;
        
        this->pan = pan;
        panright = (1 + pan) / 2.0;
        panleft = (1 - pan) / 2.0;
        
    }
    
    AelFrame& AelPanner::processFrame(AelFrame &frame){
        
        if(frame.getChannels() > 2)
            return frame;
        
        frame.toStereo();
        
        frame[0] *= panleft;
        frame[1] *= panleft;
        
        return frame;
    }
    
    AelAudioStream& AelPanner::processStream(AelAudioStream &stream){
        
        
        AelAudioStream *temp = new AelAudioStream(stream.getchannels());
        
        for(int i = 0; i < stream.getnframes(); i++){
            AelFrame aux = stream.getNextFrame();
            aux = this->processFrame(aux);
            temp->AddFrames(aux);
            
        }
        
        return *temp;
        
    }
    
    
}