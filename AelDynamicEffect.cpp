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
    
    
////////////////AELCOMPRESSOR

    AelCompressor::AelCompressor(double rt, double thdb, double atck, double rls, int samplerate)
        : AelEffect(samplerate){
        
            setRatio(rt);
            setThreshold(thdb);
            setAttack(atck);
            setRelease(rls);
        
    }

    void AelCompressor::setRatio(double ratio_){
    
        if(ratio_ < 1.0)
            ratio = 1.0;
        
        else if (ratio_ > 70.0)
            ratio = 70.0;
        
        else
            ratio = ratio_;
    
    }
    
    void AelCompressor::setThreshold(double thresholddb){
    
        if(thresholddb > -1.0)
            thresholdDB = -1.0;
        
        else if(thresholddb < -70.0)
            thresholdDB = -70.0;
        
        else
          thresholdDB = thresholddb;
        
        
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholdDB/20);
    
    }
    
    void AelCompressor::setAttack(double attack_) {
    
        if(attack_ > 2.6)
            attack = exp(-1.0/(sampleRate*2.6));
        
        else if(attack_ < 0.00001)
            attack = exp(-1.0/(sampleRate*0.00001));
        
        else
            attack = exp(-1.0/(sampleRate*attack_));

    
    }
    
    void AelCompressor::setRelease(double release_){
    
        if(release_ > 5.0)
            release = exp(-1.0/(sampleRate*5.0));
        
        else if(release_ < 0.130)
            release = exp(-1.0/(sampleRate*0.130));
        
        else
            release = exp(-1.0/(sampleRate*release_));
        
    
    }
    
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

            float logx = 20.0 * log10(peak/MAX_SAMPLE_VALUE);
                
            frame = frame *  pow(10.0, (thresholdDB - logx) * (1.0-(1.0/ratio)) / 20.0);
            
        }
        
        return frame;
    }
    

    
////////////AELGATE
    
    AelGate::AelGate(double thdb,  double att, double atck, double rls, int samplerate) : AelEffect(samplerate){
        
        setThreshold(thdb);
        setAttenuation(att);
        setAttack(atck);
        setRelease(rls);
        
    }
    
    
    void AelGate::setThreshold(double thresholddb){
        
        if(thresholddb > -1.0)
            thresholdDB = -1.0;
        
        else if(thresholddb < -70.0)
            thresholdDB = -70.0;
        
        else
            thresholdDB = thresholddb;
        
        
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholdDB/20);
        
    }
    
    void AelGate::setAttenuation(double att){
        
        
        if(att > 0.0)
            attenuationDB  = 0.0;
        
        else if(att < -70.0)
            attenuationDB = -70.0;
        
        else
            attenuationDB = att;
        
        
        attenuation = pow(10.0, attenuationDB/20);
        
        
    }
    
    void AelGate::setAttack(double attack_) {
        
        if(attack_ > 2.6)
            attack = exp(-1.0/(sampleRate*2.6));
        
        else if(attack_ < 0.00001)
            attack = exp(-1.0/(sampleRate*0.00001));
        
        else
            attack = exp(-1.0/(sampleRate*attack_));
        
        
    }
    
    void AelGate::setRelease(double release_){
        
        if(release_ > 5.0)
            release = exp(-1.0/(sampleRate*5.0));
        
        else if(release_ < 0.130)
            release = exp(-1.0/(sampleRate*0.130));
        
        else
            release = exp(-1.0/(sampleRate*release_));
        
        
    }
    
    AelFrame& AelGate::processFrame(AelFrame& frame){
        
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
        
        if(peak < threshold)
            frame = frame * attenuation;

        return frame;
    }


    
}
