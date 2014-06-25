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

    AelCompressor::AelCompressor(double rt, double th, double atck, double rls)
        : ratio(rt), threshold(log(th/MAX_SAMPLE_VALUE)), attack(1-exp(2.2/(sampleRate*atck))), release(1-exp(2.2/(sampleRate*rls))), prev_sample(0){
        
        //proteçoes de range para ratio, threshold, ...
        
        }

    void AelCompressor::setRatio(double ratio_){ }
    void AelCompressor::setThreshold(double thresholddb){ }
    void AelCompressor::setAttack(double attack_){ }
    void AelCompressor::setRelease(double release_){ }
    
    AelFrame& AelCompressor::processFrame(AelFrame& frame){
        
        int sample = abs(frame.maxSample());
        int peak = 0;
        
        if(sample > prev_sample){
            peak = attack * prev_sample + (1-attack) * sample;
        } else {
            peak = release * prev_sample;
        }
        
        
        
        return frame;
    }
    
    
    
}
