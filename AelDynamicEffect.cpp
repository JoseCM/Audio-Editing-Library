//
//  AelDynamicEffect.cpp
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelDynamicEffect.h"
#include "defines.h"
#include <cmath>



namespace Ael {

    AelCompressor::AelCompressor(double rt, double thdb, double atck, double rls, int samplerate)
        : AelEffect(samplerate), ratio(rt), threshold(MAX_SAMPLE_VALUE * pow(10.0, thdb/20.0)), thresholdDB(thdb), attack(exp(-1.0/(sampleRate*atck))), release(exp(-1.0/(sampleRate*rls))){
        
            if(ratio < 1 || ratio > 70)
                ratio = 1;
            
            if(thdb > 6 || thdb < -70){
                threshold = MAX_SAMPLE_VALUE;
                thresholdDB = 0;
            }
            
            //OU LANÇAR EXCEPÇOES?!?!
        }

    void AelCompressor::setRatio(double ratio_){
    
        if(ratio < 1 || ratio > 70)
            return;
        
        ratio = ratio_;
    
    }
    
    void AelCompressor::setThreshold(double thresholddb){
    
        if(thresholddb > 6 || thresholddb < -70)
            return;

        thresholdDB = thresholddb;
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholddb/20);
    
    }
    
    void AelCompressor::setAttack(double attack_) { }
    
    void AelCompressor::setRelease(double release_){ }
    
    AelFrame& AelCompressor::processFrame(AelFrame& frame){
        
        static int prev_sample = 0;
        int peak;
        int sample = abs(frame.maxSample());
        
        //Envelope generation stage
        if(sample >= prev_sample){
            peak = attack * prev_sample + (1-attack) * sample;
        } else {
            peak = release * prev_sample + (1-release) * sample;
            //OR
            //peak = release * prev_sample;
        }
        
        prev_sample = peak;
        
        //envelope check test
        //frame[0] = frame[1] = peak;
        
        //Gain calculation and adjustment stage
        if(peak > threshold){
        
            for(int i = 0; i < frame.getChannels(); i++){
                
                int tempsample = frame[i];
                float logx;
                
                logx = 20.0 * log10(peak/MAX_SAMPLE_VALUE);
                
                frame[i] = tempsample * pow(10.0, -(logx - thresholdDB)/(ratio*20));
                
        
            }
            
        }
        
        return frame;
    }
    
    
    
}
