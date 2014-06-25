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

    AelFilter::AelFilter(int sampleR, float gain_, float cutoff_) : AelEffect(sampleR), gain(gain_), cutoff(cutoff_) { }
    
    
    void AelFilter::set_gain(float gain){
        this->gain = gain;
    }
    
    
    /************************************
     *  AelIIR - 1st Order
     ************************************/
    
    AelIIR::AelIIR(int Sr, float G, float Coff ,int n_ch) : AelFilter(Sr, G, Coff) , out_1(n_ch), in_1(n_ch) {
        coef_a.assign(2, 0.0);
        coef_b.assign(1, 0.0);
        set_LPF();  //defaut LPF
    }
    
    void AelIIR::set_LPF(){
        
        ON = lowpass;
        
        double x = exp(-2*pi * (cutoff/getSampleRate()));
        coef_a[0] = 1.0 - x;
        coef_a[1] = 0.0;
        coef_b[0] = x;
    }
    
    void AelIIR::set_HPF(){
        
        ON = highpass;
        
        double x = exp(-2*pi * (cutoff/getSampleRate()));
        coef_a[0]= ((1.0 + x) / 2.0);
        coef_a[1] = -coef_a[0];
        coef_b[0] = x;
        
    }
    
    void AelIIR::set_cutoff(float cutoff){
        this->cutoff = cutoff;
        
        if(ON == lowpass){
            set_LPF();
        }
        else set_HPF();
    
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
    
    AelButterWorth::AelButterWorth(int Sr, float G, float Coff, int n_ch, float BW): AelFilter(Sr, G, Coff), out_1(n_ch), out_2(n_ch),
    in_1(n_ch), in_2(n_ch), BandWidth(BW)
    {
        coef_a.assign(3, 0.0);
        coef_b.assign(2, 0.0);
        set_LPF();    //default
    }
    
    void AelButterWorth::set_LPF(){
        
        ON = lowpass;
        
        float y = 1 / tan( (pi * cutoff) / getSampleRate() );
        float pow_2 = pow(y, 2);
        
        coef_a[0] = 1 / ( 1 + (2 * y) + pow_2 ) ;
        coef_a[1] = 2 * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = coef_a[1] * (1 - pow_2);
        coef_b[1] = coef_a[0] * (1 - (2 * y) + pow_2);
        
    }
    
    void AelButterWorth::set_HPF(){
        
        ON = highpass;
        
        float y = tan( (pi * cutoff) / getSampleRate() );
        float pow_2 = pow(y, 2);
        
        coef_a[0] = 1 / ( 1 + (2 * y) + pow_2) ;
        coef_a[1] = -2 * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = -coef_a[1] * ( pow_2 - 1 );
        coef_b[1] = coef_a[0] * ( 1 - ( 2 * y ) + pow_2 );
        
    }
    
    void AelButterWorth::set_BRF(){
        
        ON = rejectband;
        
        float y = tan( (pi * BandWidth) / getSampleRate() );
        float Q = 2 * cos( (2 * pi * cutoff) / getSampleRate());
        
        coef_a[0] = 1/ (1 + y);
        coef_a[1] = - Q * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = coef_a[1];
        coef_b[1] = coef_a[0] * (y -1);
        
    }
    
    void AelButterWorth::set_BPF(){
        
        ON = bandpass;
        
        float y = 1 / tan( (pi * BandWidth) / getSampleRate() );
        float Q = 2 * cos( (2 * pi * cutoff) / getSampleRate());
        
        coef_a[0] = 1/ (1 + y);
        coef_a[1] = 0;
        coef_a[2] = - coef_a[0];
        coef_b[0] = - y * Q * coef_a[0];
        coef_b[1] = coef_a[0] * (y -1);
        
    }
    
    void AelButterWorth::set_bandwidth(float BandWidth){
        
        this->BandWidth = BandWidth;
        
        switch (ON) {
            case rejectband:
                set_BRF();
                break;
            case bandpass:
                set_BPF();
                break;
            default:
                break;
        }

    }
    
    void AelButterWorth::set_cutoff(float cutoff){
        
        this->cutoff = cutoff;
        
        switch (ON) {
            case lowpass:
                set_LPF();
                break;
            case highpass:
                set_HPF();
                break;
            case rejectband:
                set_BRF();
                break;
            case bandpass:
                set_BPF();
                break;
            default:
                break;
        }
        
    }
    
    AelFrame& AelButterWorth::processFrame(AelFrame& Frame){
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
}