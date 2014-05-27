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
    
    
    
    class VolumeGain : public AelDynamicEffect {
        
    private:
        double volumeGain;
        
    
    public:
        VolumeGain(double gain = 0.5) : volumeGain(gain) { }
        double getGain();
        void setGain(double);
        
        
    };
    
    
    
}


#endif /* defined(__Audio_Editing_Library__AelDynamicEffect__) */
