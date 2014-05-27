//
//  AelDynamicEffect.cpp
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelDynamicEffect.h"
#include "math.h"

namespace Ael {

    double Volume::getVolume() { return volume; }
    
    void Volume::setVolume(double vol) {
        
        if(vol > 2.0) return;
        
    }
    
    double Volume::getVolumeDb();
    void Volume::setVolumeDb(double);

    AelFrame& Volume::processFrame(AelFrame&);
    AelAudioStream& Volume::processStream(AelAudioStream &);

}