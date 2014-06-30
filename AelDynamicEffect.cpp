//
//  AelDynamicEffect.cpp
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelDynamicEffect.h"

namespace Ael {
    
////////////////AEL VOLUME
    
    double AelVolume::getVolume() { return volume; }
    
    void AelVolume::setVolume(double vol) {
        
        if(MORETHAN(vol, 2.0))
            volume = 2.0;
        
        else if (LESSTHAN(vol, 0.0))
            volume = 0.0;
        
        else
            volume = vol;
    }
    
    double AelVolume::getVolumeDb(){
        
        if(EQUAL(volume, 0)) return -70.0;
        
        else return 20.0 * log(volume);
    }
    
    void AelVolume::setVolumeDb(double voldb){
        
        double dbvalue = 0.0;
        
        if(MORETHAN(voldb, 2.0))
            dbvalue = 6.0;
        
        else if (LESSTHAN(voldb, -70.0))
            dbvalue = 0.0;
        
        else
            dbvalue = voldb;
        
        volume = pow(10, dbvalue/20);
        
    }
    
    AelEffect* AelVolume::getCopy() {
        
        AelEffect *temp = new AelVolume(volume, n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    
    AelFrame& AelVolume::processFrame(AelFrame& frame){
        
        frame= frame * volume;
        
        return frame;
    }
    
    
//////////////AELPANNER
    
    //SÓ TEM SUPORTE PARA MONO E STEREO
    
    double AelPanner::getPan(){
        return pan;
    }
    
    void AelPanner::setPan(double pan){
        
        if(LESSTHAN(pan, -1.0))
            this->pan = -1.0;
        
        else if(MORETHAN(pan, 1.0))
            this->pan = 1.0;
        
        else
            this->pan = pan;
        
        
        panright = (sin((1 + this->pan) / 2.0 * M_PI / 2.0));
        panleft =  (sin((1 - this->pan) / 2.0 * M_PI / 2.0));
        
    }
    
    AelEffect* AelPanner::getCopy() {
        
        AelEffect *temp = new AelPanner(pan, n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    
    AelFrame& AelPanner::processFrame(AelFrame &frame){
        
        if(frame.getChannels() > 2)
            return frame;
        
        frame.toStereo();
        
        frame[0] *= panleft;
        frame[1] *= panright;
        
        return frame;
    }

    
    
////////////////AELCOMPRESSOR

    AelCompressor::AelCompressor(double rt, double thdb, double atck, double rls, int n_chn , int samplerate)
        : AelEffect(n_chn, samplerate){
        
            setRatio(rt);
            setThreshold(thdb);
            setAttack(atck);
            setRelease(rls);
        
    }

    void AelCompressor::setRatio(double ratio_){
    
        if(LESSTHAN(ratio_, 1.0))
            ratio = 1.0;
        
        else if (MORETHAN(ratio_, 70.0))
            ratio = 70.0;
        
        else
            ratio = ratio_;
    
    }
    
    void AelCompressor::setThreshold(double thresholddb){
    
        if(MORETHAN(thresholddb, -1.0))
            thresholdDB = -1.0;
        
        else if(LESSTHAN(thresholddb, -70.0))
            thresholdDB = -70.0;
        
        else
          thresholdDB = thresholddb;
        
        
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholdDB/20);
    
    }
    
    void AelCompressor::setAttack(double attack_) {
    
        if(MORETHAN(attack_, 2.6))
            attack = exp(-1.0/(sampleRate*2.6));
        
        else if(LESSTHAN(attack_, 0.00001))
            attack = exp(-1.0/(sampleRate*0.00001));
        
        else
            attack = exp(-1.0/(sampleRate*attack_));

    
    }
    
    void AelCompressor::setRelease(double release_){
    
        if(MORETHAN(release_, 5.0))
            release = exp(-1.0/(sampleRate*5.0));
        
        else if(LESSTHAN(release_, 0.130))
            release = exp(-1.0/(sampleRate*0.130));
        
        else
            release = exp(-1.0/(sampleRate*release_));
        
    
    }
    
    AelEffect* AelCompressor::getCopy() {
        
        AelEffect *temp =  new AelCompressor(ratio, thresholdDB, getAttackTime(), getReleaseTime(), n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
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
                
            frame = (frame *  pow(10.0, (thresholdDB - logx) * (1.0-(1.0/ratio)) / 20.0)) * getWetLevel() + frame * (1-getWetLevel());
            
        }
        
        return frame;
    }
    

    
////////////AELGATE
    
    AelGate::AelGate(double thdb,  double att, double atck, double rls, int n_chn ,int samplerate) : AelEffect(n_chn, samplerate){
        
        setThreshold(thdb);
        setAttenuation(att);
        setAttack(atck);
        setRelease(rls);
        
    }
    
    void AelGate::setThreshold(double thresholddb){
        
        if(MORETHAN(thresholddb, -1.0))
            thresholdDB = -1.0;
        
        else if(LESSTHAN(thresholddb, -70.0))
            thresholdDB = -70.0;
        
        else
            thresholdDB = thresholddb;
        
        
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholdDB/20);
        
        
    }
    
    void AelGate::setAttenuation(double att){
        
        if(MORETHAN(att, 0.0))
            attenuationDB = 0.0;
        
        else if(LESSTHAN(att, -70.0))
            attenuationDB = -70.0;
        
        else
            attenuationDB = att;
        
        
        attenuation = pow(10.0, attenuationDB/20);
        
        
    }
    
    void AelGate::setAttack(double attack_) {
        
        
        if(MORETHAN(attack_, 2.6))
            attack = exp(-1.0/(sampleRate*2.6));
        
        else if(LESSTHAN(attack_, 0.00001))
            attack = exp(-1.0/(sampleRate*0.00001));
        
        else
            attack = exp(-1.0/(sampleRate*attack_));
        
        
    }
    
    void AelGate::setRelease(double release_){
        
        if(MORETHAN(release_, 5.0))
            release = exp(-1.0/(sampleRate*5.0));
        
        else if(LESSTHAN(release_, 0.130))
            release = exp(-1.0/(sampleRate*0.130));
        
        else
            release = exp(-1.0/(sampleRate*release_));
        
        
    }
    
    AelEffect* AelGate::getCopy() {
        
        AelEffect *temp =  new AelGate(thresholdDB, attenuationDB, getAttackTime(), getReleaseTime(), n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    
    AelFrame& AelGate::processFrame(AelFrame& frame){
        
        static int prev_sample = 0;
        int peak;
        int sample = abs(frame.maxSample());

        if(sample >= prev_sample){
            peak = attack * prev_sample + (1-attack) * sample;
        } else {
            peak = release * prev_sample + (1-release) * sample;
        }
        
        prev_sample = peak;
        
        if(peak < threshold)
            frame = frame * attenuation * getWetLevel() + frame * (1 - getWetLevel());

        return frame;
    }


    
}
