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
#include <list>
#include "AelAudioBuf.h"
#include "AelEffects.h"

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

    };

    
    class AelChannel {
        
        friend class AelMixer;
        
        const int channel_ID;
        string name;
        AelVolume volume;
        AelPanner panner;
        AelAudioStream stream;
        list<AelEffect*> effectChain;
        bool onoff;
        bool eoc;
        
        
    public:
        
        AelChannel(const string &fileName, int ID) : stream(fileName), volume(0.5), panner(0), onoff(true), name(fileName),
        channel_ID(ID), eoc(false) { }
        void setVolumeDb(double volDb) { volume.setVolumeDb(volDb); }
        void setPan(double pan){ panner.setPan(pan); }
        double getVolumeDb() { return volume.getVolume(); }
        double getPan() { return panner.getPan(); }
        void turnOn() { onoff = true; }
        void turnOff() { onoff = false; }
        bool isOn() { return onoff; }
        int getID() { return channel_ID; }
        string getName() { return name; }
        void setName(string newname) { name = newname; }
        void addEffect(AelEffect &effect) { effectChain.push_back(&effect); }
        //void removeEffect(int ChannelId);
        AelFrame getNextFrame();
        bool isEOC(){ return eoc; }
        
    };
    
    
    class AelMixer{
        
    public:
        
        AelMixer() : m_nChannels(0), masterVolDb(1.0), masterPan(0.0) { }
        int addChannel(const string &filename);
        //outras formas de adicionar canais (vazios, associados a geradores de sinal, etc(?))
        AelChannel* getChannel(const int &channelID);
        bool removeChannel(const int &channelID);
        AelFrame getNextFrame();
        AelAudioStream* getFullMix();
        
        //void addEffect(int channelID, AelEffect &effect);
        
    private:
        
        int m_nChannels;
        list<AelChannel*> channel_list;
        AelVolume masterVolDb;
        AelPanner masterPan;
        list<AelChannel*>::iterator findChannel(const int &channelID);
    };
    
}


#endif /* defined(__AudioEditingLibrary__AelMixer__) */
