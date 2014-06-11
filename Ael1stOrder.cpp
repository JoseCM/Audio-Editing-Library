//
//  Ael1stOrder.cpp
//  AEL_PROJECT
//
//  Created by Paulo Jorge Felix Oliveira on 03/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#include "Ael1stOrder.h"
//AelFilter::AelFilter(int sampleR, float gain_, float cutoff_)

namespace Ael{
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
}