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
     *  AelIIR
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
        
        AelFrame aux = Frame;
        for(int i = 0; i < Frame.getChannels() ; i++){
            aux[i] = Frame[i];
            Frame[i] = gain * ((Frame[i] * coef_a[0]) + (in_1[i] * coef_a[1]) + (out_1[i] * coef_b[0]));
            out_1[i] = Frame[i];
            in_1[i] = aux[i];
        }
        
        return Frame;
    }
    
    AelAudioStream& AelIIR::processStream(AelAudioStream &Stream){
        
        AelAudioStream *temp = new AelAudioStream(Stream.getchannels());
        
        for (int i=0; i< Stream.getnframes(); i++){
            AelFrame aux = Stream.getNextFrame();
            aux = this->processFrame(aux);
            temp->AddFrames(aux);
        }
        
        return *temp;
        
    }
    
    
    /************************************
     *  ButterWorth
     ************************************/
    
    AelFrame& ButterWorth::processFrame(AelFrame& Frame){
        //y(n) = a0x(n) + a1x(n-1) + a2x(n-2) -  b0y(n - 1) - b1y(n-2).
        
        AelFrame temp = Frame;
        
        for(int i = 0; i < Frame.getChannels() ; i++){
            temp[i] *= gain;
            Frame[i] = ( (coef_a[0] * temp[i]) + (coef_a[1] * in_1[i]) + (coef_a[2] * in_2[i]));
            Frame[i] -= ( (coef_b[0] * out_1[i]) + (coef_b[1] * out_2[i]) );
            in_2[i] = in_1[i];
            in_1[i] = temp[i];
            out_2[i] = out_1[i];
            out_1[i] = Frame[i];
            
        }
        return Frame;
    }
    
    AelAudioStream& ButterWorth::processStream(AelAudioStream &Stream){
        
        AelAudioStream *temp = new AelAudioStream(Stream.getchannels());
        
        for (int i=0; i< Stream.getnframes(); i++){
            AelFrame aux = Stream.getNextFrame();
            aux = this->processFrame(aux);
            temp->AddFrames(aux);
        }
        return *temp;
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