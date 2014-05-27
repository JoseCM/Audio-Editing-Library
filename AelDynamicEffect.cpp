//
//  AelDynamicEffect.cpp
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelDynamicEffect.h"
#include "cmath"

namespace Ael {

    double Volume::getVolume() { return volume; }
    
    void Volume::setVolume(double vol) {
        
        if(vol > 2.0 || vol == 0) return;
        
        this->volume = vol;
    }
    
    double Volume::getVolumeDb(){
        
        if(volume == 0) return
        
    }
    
    void Volume::setVolumeDb(double voldb){
        
        
        
        
    }

    AelFrame& Volume::processFrame(AelFrame&);
    AelAudioStream& Volume::processStream(AelAudioStream &);

}