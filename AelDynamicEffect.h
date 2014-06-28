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
    
    class AelCompressor: public AelEffect {

        double ratio;
        int threshold;
        double thresholdDB, attack, release;
        
        public:
        
        AelCompressor(double rt = 1.0, double thdb = 0.0, double atck = 0.00001, double rls = 0.130, int samplerate = 44100);
        void setRatio(double ratio_);
        void setThreshold(double thresholddb);
        void setAttack(double attack_);
        void setRelease(double release_);
        
        virtual AelFrame& processFrame(AelFrame&);
        
    };

    
}


#endif /* defined(__Audio_Editing_Library__AelDynamicEffect__) */
