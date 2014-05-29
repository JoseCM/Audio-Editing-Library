//
//  AelDynamicEffect.cpp
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelDynamicEffect.h"
#include <cmath>

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
        
        AelFrame *temp = new AelFrame(frame.getChannels());
        
        for(int i = 0; i < frame.getChannels(); i++);
        
        return *temp;
    }
    


}