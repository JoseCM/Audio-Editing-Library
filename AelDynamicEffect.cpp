//
//  AelDynamicEffect.cpp
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelDynamicEffect.h"
#include <cmath>

#define MAX_SAMPLE_VALUE ( (pow(2.0, 32.0)/2) - 1 )

namespace Ael {

    double Volume::getVolume() { return volume; }
    
    void Volume::setVolume(double vol) {
        
        if(vol > 2.0 || vol < 0) return;
        
        this->volume = vol;
    }
    
    double Volume::getVolumeDb(){
        
        if(volume == 0) return -70.0;
        
        else return 20.0 * log(volume);
    }
    
    void Volume::setVolumeDb(double voldb){
        
        if(voldb > 6 || voldb < 70) return;
        
        volume = pow(10, voldb/20);
        
    }

    AelFrame& Volume::processFrame(AelFrame& frame){
        
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
    
    
    AelAudioStream& Volume::processStream(AelAudioStream &stream){
        
        AelAudioStream *temp = new AelAudioStream(stream.getchannels());
        
        for(int i = 0; i < stream.getnframes(); i++){
            AelFrame aux = stream.getNextFrame();
            aux = processFrame(aux);
            temp->AddFrames(aux);
            
        }
        
        
        return *temp;
        
    }

}