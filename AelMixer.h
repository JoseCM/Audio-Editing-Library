//
//  AelMixer.h
//  AudioEditingLibrary
//
//  Created by José Martins on 31/05/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#ifndef __AudioEditingLibrary__AelMixer__
#define __AudioEditingLibrary__AelMixer__

#include <iostream>
#include "AelAudioBuf.h"
#include "AelEffects.cpp"

namespace Ael {
    

    class AelVolume : public AelEffect  {
        
    private:
        double volume;
        
    public:
        AelVolume(double gain = 0.5) : volume(gain) { }
        double getVolume();
        void setVolume(double);
        double getVolumeDb();
        void setVolumeDb(double);
        
        AelFrame& processFrame(AelFrame&);
        AelAudioStream& processStream(AelAudioStream &);
    };

    class AelPanner : public AelEffect {
        
    private:
        double pan;
        double panright;
        double panleft;
        
    public:
        AelPanner(double pan = 0.0) : pan(pan), panright( (1 + pan) / 2.0 ), panleft((1 - pan) / 2.0 ) { }
        double getPan();
        void setPan(double);
        
        AelFrame& processFrame(AelFrame&);
        AelAudioStream& processStream(AelAudioStream &);
        
        
    };
    
    
}


#endif /* defined(__AudioEditingLibrary__AelMixer__) */
