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

    AelFilter::AelFilter(float gain_, float cutoff_, int n_ch, int sampleR) : AelEffect(n_ch, sampleR), gain(gain_), cutoff(cutoff_) { }
    
    
    void AelFilter::set_gain(float gain_){
        
        if(MORETHAN(gain_, 2.0))
            gain = 1.0;
        
        else if (LESSTHAN(gain_, 0.0))
            gain = 0.0;
        
        else
            gain = gain_;
    }
    
    
    /************************************
     *  AelIIR - 1st Order
     ************************************/
    
    AelIIR::AelIIR(float G, float Coff ,int n_ch, int Sr) : AelFilter(G, Coff, n_ch, Sr) , out_1(n_ch), in_1(n_ch) {
        coef_a.assign(2, 0.0);
        coef_b.assign(1, 0.0);
        set_LPF();  //defaut LPF
    }
    
    void AelIIR::set_LPF(){
        
        ON = lowpass;
        
        double x = exp(-2*M_PI * (cutoff/getSampleRate()));
        coef_a[0] = 1.0 - x;
        coef_a[1] = 0.0;
        coef_b[0] = x;
    }
    
    void AelIIR::set_HPF(){
        
        ON = highpass;
        
        double x = exp(-2*M_PI * (cutoff/getSampleRate()));
        coef_a[0]= ((1.0 + x) / 2.0);
        coef_a[1] = -coef_a[0];
        coef_b[0] = x;
        
    }
    
    void AelIIR::set_cutoff(float cutoff_){
        
        if(MORETHAN(cutoff_, (getSampleRate()/2)))
            cutoff = getSampleRate()/2 ;
        
        else if (LESSTHAN(cutoff_, 0.0))
            cutoff = 0.0;
        
        else
            this->cutoff = cutoff_;
        
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
    
    AelEffect* AelIIR::getCopy(){
        AelIIR *cp = new AelIIR(gain, cutoff, getNChannels(), getSampleRate());
    
        cp->out_1 = out_1;
        cp->in_1 = in_1;
        
        cp->ON = ON;
        
        if(ON == lowpass){
            cp->set_LPF();
        }
        else cp->set_HPF();
        
        cp->setWetLevel(getWetLevel());
        
        if(!isOn()){
            cp->m_turnOff();
        }
    
        return cp;
    }
    
    /************************************
     *  ButterWorth - 2nd Order
     ************************************/
    
    AelButterWorth::AelButterWorth(float G, float Coff, float BW, int n_ch, int Sr): AelFilter(G, Coff, n_ch, Sr), out_1(n_ch), out_2(n_ch),
    in_1(n_ch), in_2(n_ch), BandWidth(BW)
    {
        coef_a.assign(3, 0.0);
        coef_b.assign(2, 0.0);
        set_LPF();    //default
    }
    
    void AelButterWorth::set_LPF(){
        
        ON = lowpass;
        
        float y = 1 / tan( (M_PI * cutoff) / getSampleRate() );
        float pow_2 = pow(y, 2);
        
        coef_a[0] = 1 / ( 1 + (2 * y) + pow_2 ) ;
        coef_a[1] = 2 * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = coef_a[1] * (1 - pow_2);
        coef_b[1] = coef_a[0] * (1 - (2 * y) + pow_2);
        
    }
    
    void AelButterWorth::set_HPF(){
        
        ON = highpass;
        
        float y = tan( (M_PI * cutoff) / getSampleRate() );
        float pow_2 = pow(y, 2);
        
        coef_a[0] = 1 / ( 1 + (2 * y) + pow_2) ;
        coef_a[1] = -2 * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = -coef_a[1] * ( pow_2 - 1 );
        coef_b[1] = coef_a[0] * ( 1 - ( 2 * y ) + pow_2 );
        
    }
    
    void AelButterWorth::set_NOTCH(){
        
        ON = rejectband;
        
        BandWidth = 10000;
        
        float y = tan( (M_PI * BandWidth) / getSampleRate() );
        float Q = 2 * cos( (2 * M_PI * cutoff) / getSampleRate());
        
        coef_a[0] = 1/ (1 + y);
        coef_a[1] = - Q * coef_a[0];
        coef_a[2] = coef_a[0];
        coef_b[0] = coef_a[1];
        coef_b[1] = coef_a[0] * (y -1);
        
    }
    
    void AelButterWorth::set_BPF(){
        
        ON = bandpass;
        
        float y = 1 / tan( (M_PI * BandWidth) / getSampleRate() );
        float Q = 2 * cos( (2 * M_PI * cutoff) / getSampleRate());
        
        coef_a[0] = 1/ (1 + y);
        coef_a[1] = 0;
        coef_a[2] = - coef_a[0];
        coef_b[0] = - y * Q * coef_a[0];
        coef_b[1] = coef_a[0] * (y -1);
        
    }
    
    void AelButterWorth::set_bandwidth(float BandWidth_){
        
        if(MORETHAN(BandWidth_, (getSampleRate()/2)))
            BandWidth = getSampleRate()/2 ;
        
        else if (LESSTHAN(BandWidth_, 0.0))
            BandWidth = 0.0;
        
        else
            BandWidth = BandWidth_;
        
        if(ON == rejectband)
            set_BPF();

    }
    
    void AelButterWorth::set_cutoff(float cutoff_){
        
        if(MORETHAN(cutoff_, (getSampleRate()/2)))
            cutoff = getSampleRate()/2 ;
        
        else if (LESSTHAN(cutoff_, 0.0))
            cutoff = 0.0;
        
        else
            this->cutoff = cutoff_;
        
        switch (ON) {
            case lowpass:
                set_LPF();
                break;
            case highpass:
                set_HPF();
                break;
            case rejectband:
                set_NOTCH();
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
    
    AelEffect* AelButterWorth::getCopy(){
        AelButterWorth *cp = new AelButterWorth(gain, cutoff, BandWidth ,getNChannels(), getSampleRate());
        
        cp->out_1 = out_1;
        cp->in_1 = in_1;
        cp->out_2 = in_2;
        cp->in_2 = in_2;
        
        cp->ON = ON;

        switch (ON) {
            case lowpass:
                cp->set_LPF();
                break;
            case highpass:
                cp->set_HPF();
                break;
            case rejectband:
                cp->set_NOTCH();
                break;
            case bandpass:
                cp->set_BPF();
                break;
            default:
                break;
        }
        
        cp->setWetLevel(getWetLevel());
        
        if(!isOn()){
            cp->m_turnOff();
        }
        
        
        return cp;
        
    }
}