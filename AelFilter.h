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
#include <cmath>
#define pi 3.14159265359

namespace Ael {
    
    class AelFilter : public AelEffect {
    protected:
        vector<float> coef_a, coef_b;
        float gain, cutoff;
    public:
        AelFilter(int =44100, float =1.0, float =1.0, int =1, int =1);
        virtual ~AelFilter() {    }
        
        void set_cutoff(float cutoff) { this->cutoff = cutoff; }
        void set_gain(float gain) { this->gain = gain; }
        
        virtual void set_LPF() = 0;
        virtual void set_HPF() = 0;
        
        virtual AelFrame& processFrame(AelFrame&) = 0;
    };
    

    class AelIIR: public AelFilter{
        AelFrame out_1, in_1;
        
        
    public:
        AelIIR(int Sr=44100, float G=1.0, float Coff=1.0 ,int n_ch=2, int n_coefa=2, int n_coefb=1) : AelFilter(Sr, G, Coff, n_coefa, n_coefb) , out_1(n_ch), in_1(n_ch) { }
        
        virtual void set_LPF();
        virtual void set_HPF();
        
        virtual AelFrame& processFrame(AelFrame&);
        
    };
    
    
    class ButterWorth: public AelFilter{
        AelFrame out_1, out_2, in_1, in_2;
        
    public:
        ButterWorth(int Sr=44100, float G=1.0, float Coff=1.0, int n_ch=2 ,int n_coefa=3, int n_coefb=2): AelFilter(Sr, G, Coff, n_coefa, n_coefb), out_1(n_ch), out_2(n_ch), in_1(n_ch), in_2(n_ch) { }
        
        virtual void set_LPF();
        virtual void set_HPF();
        void set_NOTCH(float);
        
        virtual AelFrame& processFrame(AelFrame&);
    };
    
    
}

#endif /* defined(__AEL__AelFilter__) */
