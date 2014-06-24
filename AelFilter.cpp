//
//  AelFilter.cpp
//  AEL
//
//  Created by Paulo Jorge Felix Oliveira on 01/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#include "AelFilter.h"


namespace Ael {
    
    /************************************
    *  AelFilter
    ************************************/

    AelFilter::AelFilter(int sampleR, float gain_, float cutoff_, int n_coefa, int n_coefb) : AelEffect(sampleR), gain(gain_), cutoff(cutoff_) {
    
    coef_a.assign(n_coefa, 0.0);
    coef_b.assign(n_coefb, 0.0);
    
    }
    
    /************************************
     *  AelIIR - 1st Order
     ************************************/
    
    void AelIIR::set_LPF(){
        
        double x = exp(-2*pi * (cutoff/getSampleRate()));
        coef_a[0] = 1.0 - x;
        coef_a[1] = 0.0;
        coef_b[0] = x;
    }
    
    void AelIIR::set_HPF(){
        
        double x = exp(-2*pi * (cutoff/getSampleRate()));
        coef_a[0]= ((1.0 + x) / 2.0);
        coef_a[1] = -coef_a[0];
        coef_b[0] = x;
        
    }
    
    AelFrame& AelIIR::processFrame(AelFrame& Frame){
        //y[n] = a0 * x[n] + a1 * x[n-1] + b1 * y[n-1]
        
        AelFrame in = Frame * gain;
        
        Frame = (in * coef_a[0]) + (in_1 * coef_a[1]) + (out_1 * coef_b[0]) ;
        
        out_1 = Frame;
        in_1 = in;
        
        Frame = (in * (1 - getWetLevel()) + (Frame * getWetLevel()));  //dry/wet
        
        return Frame;
        
    }
    
    /************************************
     *  ButterWorth - 2nd Order
     ************************************/
    
    AelFrame& ButterWorth::processFrame(AelFrame& Frame){
        //y(n) = a0x(n) + a1x(n-1) + a2x(n-2) -  b0y(n - 1) - b1y(n-2).
        
        AelFrame in = Frame * gain;
        
        Frame = (in * coef_a[0]) + (in_1 * coef_a[1]) + (in_2 * coef_a[2]);
        Frame = Frame - (out_1 * coef_b[0]) - (out_2 * coef_b[1]) ;
        
        in_2 = in_1;
        in_1 = in;
        out_2 = out_1;
        out_1 = Frame;
        
        Frame = in * (1 - getWetLevel()) + (Frame * getWetLevel());   //dry/wet
        
        return Frame;
        
    }

    void ButterWorth::set_LPF(){
        
        float w = tan( (pi * cutoff) / getSampleRate() );
        float y = 1 / w;
        
        coef_a[0] = 1 / ( 1 + (2 * y) + pow(y, 2) ) ;
        coef_a[1] = 2 * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = 2 * coef_a[0] * (1 - pow(y, 2));
        coef_b[1] = coef_a[0] * (1 - (2*y) + pow(y, 2));
        
    }
    
    void ButterWorth::set_HPF(){
        
        float y = tan( (pi * cutoff) / getSampleRate() );
        
        coef_a[0] = 1 / ( 1 + (2 * y) + pow(y, 2) ) ;
        coef_a[1] = -2 * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = 2 * coef_a[0] * ( pow(y, 2) - 1 );
        coef_b[1] = coef_a[0] * ( 1 - (2*y) + pow(y, 2) );
        
    }
    
    void ButterWorth::set_NOTCH(float bw){
        
        float BW = bw;
        float y = tan( (pi * BW) / getSampleRate() );
        float Q = 2 * cos( (2 * pi * cutoff) / getSampleRate());
        
        coef_a[0] = 1/ (1 + y);
        coef_a[1] = - Q * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = coef_a[1];
        coef_b[1] = coef_a[0] * (y -1);
        
    }

}