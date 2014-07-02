//
//  AelDynamicEffect.h
//  Audio Editing Library
//
//  Created by José Martins on 23/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#ifndef __Audio_Editing_Library__AelDynamicEffect__
#define __Audio_Editing_Library__AelDynamicEffect__

#include "AelEffects.h"

namespace Ael {
    
    class AelVolume : public AelEffect  {
        
    private:
        double volume;
        
    public:
        AelVolume(double gain = 1.0, int n_chn = 2, int sampRate = 44100) : Ael::AelEffect(n_chn, sampRate), volume(gain) { }
        double getVolume();
        void setVolume(double);
        double getVolumeDb();
        void setVolumeDb(double);
        
        AelEffect* getCopy();
        AelFrame& processFrame(AelFrame&);
    };
    
    class AelPanner : public AelEffect {
        
    private:
        double pan;
        double panright;
        double panleft;
        
    public:
        AelPanner(double pan = 0.0, int n_chn = 2, int sampRate = 44100) : Ael::AelEffect(n_chn, sampRate), pan(pan), panright( (sin((1 + this->pan) / 2.0 * M_PI / 2.0)) ), panleft((sin((1 - this->pan) / 2.0 * M_PI / 2.0)) ) { }
        double getPan();
        void setPan(double);
        
        AelEffect* getCopy();
        AelFrame& processFrame(AelFrame&);
        
    };
    
    class AelCompressor : public AelEffect {

        double ratio;
        int threshold;
        double thresholdDB, attack, release;
        
        public:
        
        AelCompressor(double rt = 1.0, double thdb = -1.0, double atck = 0.00001, double rls = 0.130, int n_chn = 2, int samplerate = 44100);
        void setRatio(double ratio_);
        void setThreshold(double thresholddb);
        void setAttack(double attack_);
        void setRelease(double release_);
        
        double getRatio() { return ratio; }
        double getThreshold() { return thresholdDB; }
        double getAttackTime() { return -1.0/(log(attack)*sampleRate); }
        double getReleaseTime() { return -1.0/(log(release)*sampleRate); }
        
        virtual AelEffect* getCopy();
        virtual AelFrame& processFrame(AelFrame&);
        
    };

    
    class AelGate : public AelEffect {
        
        double attack, release, thresholdDB, attenuationDB, attenuation;

        int threshold;
        
    public:
        
        AelGate(double thdb = -70.0,  double att = -70.0, double atck = 0.00001,  double rls = 0.130, int n_chn = 2, int samplerate = 44100);
        
        void setThreshold(double thresholddb);
        void setAttenuation(double att);
        void setAttack(double attack_);
        void setRelease(double release_);
        
        double getAttenuation() { return attenuationDB; }
        double getThreshold() { return thresholdDB; }
        double getAttackTime() { return - 1.0/(log(attack)*sampleRate); }
        double getReleaseTime() { return - 1.0/(log(release)*sampleRate); }
        
        virtual AelEffect* getCopy();
        virtual AelFrame& processFrame(AelFrame&);
    };
    
}


#endif /* defined(__Audio_Editing_Library__AelDynamicEffect__) */
