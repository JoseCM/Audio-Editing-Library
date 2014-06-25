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
        double threshold;
        double attack, release;
        int prev_sample;
        
        public:
        
        AelCompressor(double rt = 1.0, double th = 0.0, double atck = 0.005, double rls = 0.13);
        void setRatio(double ratio_);
        void setThreshold(double thresholddb);
        void setAttack(double attack_);
        void setRelease(double release_);
        
        virtual AelFrame& processFrame(AelFrame&);
        
    };

    
}


#endif /* defined(__Audio_Editing_Library__AelDynamicEffect__) */
