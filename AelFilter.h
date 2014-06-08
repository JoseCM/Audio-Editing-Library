//
//  AelFilter.h
//  AEL
//
//  Created by Paulo Jorge Felix Oliveira on 01/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#ifndef __AEL__AelFilter__
#define __AEL__AelFilter__

#include <iostream>
#include "AelEffects.h"
#define pi 3.14159265359

namespace Ael {
    
    class AelFilter : public AelEffect {
    protected:
        vector<float> coef_a, coef_b;
        float gain, cutoff;
    public:
        AelFilter(int = 44100, float = 1.0, float = 1.0);
        AelFilter(int =44100, float =1.0, float =1.0, int =1, int =1);
        virtual ~AelFilter() {    }
        void set_cutoff(float cutoff) { this->cutoff = cutoff; }
        void set_gain(float gain) { this->gain = gain; }
        
        virtual AelFrame& processFrame(AelFrame&) = 0; // retorna a propria frame processada
        virtual AelAudioStream& processStream(AelAudioStream &) = 0; // retorna a propria frame processada
        virtual void set_LPF() = 0;
        virtual void set_HPF() = 0;
        virtual void set_NOTCH(float) = 0;
        
        //virtual void set_BPF() = 0;
        //virtual void set_APF() = 0;
    
    };
}

#endif /* defined(__AEL__AelFilter__) */
