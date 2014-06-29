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
#include "AelUtilities.h"
#define pi 3.14159265359

namespace Ael {
    
    
    class AelFilter : public AelEffect {
    
    protected:
        vector<float> coef_a, coef_b;
        float gain, cutoff;
        
    public:
        AelFilter(float =1.0, float =4410, int n_ch=2, int =44100);
        virtual ~AelFilter() {    }
        virtual void set_gain(float);
        virtual void set_cutoff(float) = 0;
        virtual AelFrame& processFrame(AelFrame&) = 0;
    };
    
    //1st Order
    class AelIIR: public AelFilter{
        
        enum iir {lowpass, highpass} ON;
        AelFrame out_1, in_1;
        
    public:
        AelIIR(float G=1.0, float Coff=4410 ,int n_ch=2, int Sr=44100);
        virtual ~AelIIR() {  }
        void set_LPF();
        void set_HPF();
        virtual void set_cutoff(float);
        virtual AelFrame& processFrame(AelFrame&);
    };
    
    //2nd Order
    class AelButterWorth: public AelFilter{
        
        enum butterworth {lowpass, highpass, bandpass, rejectband} ON;
        AelFrame out_1, out_2, in_1, in_2;
        float BandWidth;
        
    public:
        AelButterWorth(float G=1.0, float Coff=4410, float BW =970, int n_ch=2, int Sr=44100);
        virtual ~AelButterWorth(){ }
        void set_LPF();
        void set_HPF();
        void set_BRF();
        void set_BPF();
        void set_bandwidth(float);
        virtual void set_cutoff(float);
        virtual AelFrame& processFrame(AelFrame&);
    };
    
}
#endif /* defined(__AEL__AelFilter__) */
