//
//  AelDynamicEffect.h
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#ifndef __Audio_Editing_Library__AelDynamicEffect__
#define __Audio_Editing_Library__AelDynamicEffect__

#include <iostream>
#include "AelEffects.h"

namespace Ael {
    
    class AelDynamicEffect : public AelEffect {
        
        
        
        
    public:
        
        
    };
    
    
    
    class Volume : public AelDynamicEffect {
        
    private:
        double volume;
        
    
    public:
        Volume(double gain = 0.5) : volume(gain) { }
        double getVolume();
        void setVolume(double);
        double getVolumeDb();
        void setVolumeDb(double);
        
        AelFrame& processFrame(AelFrame&);
        AelAudioStream& processStream(AelAudioStream &);
    };
    
    class Panner : public AelDynamicEffect {
        
        
        
        
        
    };
    
}


#endif /* defined(__Audio_Editing_Library__AelDynamicEffect__) */
