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
    
    /*******************************************************************
     *Método getVolume
     *Parâmetros: void
     *retorna double que representa o estado da variável correspondente 
     * ao volume
     *******************************************************************/
    double AelVolume::getVolume() { return volume; }
    
    
    /*******************************************************************
     *Método setVolume
     *Parâmetros: double
     *actualiza o estado da variavel define o volume dentro dos limites
     *establecido
     *retorna void
     *******************************************************************/
    void AelVolume::setVolume(double vol) {
        
        if(MORETHAN(vol, 2.0))
            volume = 2.0;
        
        else if (LESSTHAN(vol, 0.0))
            volume = 0.0;
        
        else
            volume = vol;
    }
    
    
    /*******************************************************************
     *Método setVolumeDb
     *Parâmetros: void
     *retorna double que representa o estado da variável correspondente
     *ao volume traduzida em decibeis
     *******************************************************************/
    double AelVolume::getVolumeDb(){
        
        if(EQUAL(volume, 0)) return -70.0;
        
        else return 20.0 * log(volume);
    }
    
    /*******************************************************************
     *Método setVolumedB
     *Parâmetros: double
     *actualiza o estado da variavel define o volume dentro dos limites 
     *establecidos, fazendo a tradução de decibeis para valor absoluto
     *retorna void
     *******************************************************************/
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
    
    /*******************************************************************
     *Função membro getCopy
     *Parâmetros: void
     *Retorna Cópia do efeito, no estado actual em que se encontra
     *******************************************************************/
    AelEffect* AelVolume::getCopy() {
        
        AelEffect *temp = new AelVolume(volume, n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    
    /*******************************************************************
     *Função membro processFrame
     *Parâmetros: AelFrame& iframe
     *Responsável por processar a frame recebida pelo efeito volume
     *mutiplicando a frame por o valor absoluto do volume
     *******************************************************************/
    AelFrame& AelVolume::processFrame(AelFrame& frame){
        
        frame= frame * volume;
        
        return frame;
    }
    
    
//////////////AELPANNER
    
    //SÓ TEM SUPORTE PARA MONO E STEREO
    
    /*******************************************************************
     *Método getPan
     *Parâmetros: void
     *retorna double que representa o estado da variável correspondente
     *ao parametro do panner
     *******************************************************************/
    double AelPanner::getPan(){
        return pan;
    }
    
    /*******************************************************************
     *Método setPan
     *Parâmetros: double
     *actualiza o estado da variavel define o vpan dentro dos limites 
     *establecidos calculando os níveis do canal direito e esquerdo correspondente
     *retorna void
     *******************************************************************/
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
    
    /*******************************************************************
     *Função membro getCopy
     *Parâmetros: void
     *Retorna Cópia do efeito, no estado actual em que se encontra
     *******************************************************************/
    AelEffect* AelPanner::getCopy() {
        
        AelEffect *temp = new AelPanner(pan, n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    
    /*******************************************************************
     *Função membro processFrame
     *Parâmetros: AelFrame& iframe
     *Responsável por processar a frame recebida pelo efeito panner
     *mutiplicando a frame direita e esquerda pelos valores correspondentes
     *******************************************************************/
    AelFrame& AelPanner::processFrame(AelFrame &frame){
        
        frame.toStereo(); // traduz a frame para stereo, já que este efeito
                          //apenas opera em streams de dois canais
        
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

    /*******************************************************************
     *Método setRatio
     *Parâmetros: double
     *actualiza o estado da variavel define o ratio de compressão,
     *dentro dos limites establecidos
     *retorna void
     *******************************************************************/
    void AelCompressor::setRatio(double ratio_){
    
        if(LESSTHAN(ratio_, 1.0))
            ratio = 1.0;
        
        else if (MORETHAN(ratio_, 70.0))
            ratio = 70.0;
        
        else
            ratio = ratio_;
    
    }
    
    /*******************************************************************
     *Método setThreshold
     *Parâmetros: double
     *actualiza o estado da variavel define o nível threshold do compressor
     *dentro dos limites establecidos, convertendo o parametro recebido
     *em decibeis para um valor absoluto
     *retorna void
     *******************************************************************/
    void AelCompressor::setThreshold(double thresholddb){
    
        if(MORETHAN(thresholddb, -1.0))
            thresholdDB = -1.0;
        
        else if(LESSTHAN(thresholddb, -70.0))
            thresholdDB = -70.0;
        
        else
          thresholdDB = thresholddb;
        
        
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholdDB/20);
    
    }
    
    /*******************************************************************
     *Método setAttack
     *Parâmetros: double
     *actualiza o estado da variavel define o tempo de subida do simulador
     *de detector de pico que produz o envelope do sinal
     *retorna void
     *******************************************************************/
    void AelCompressor::setAttack(double attack_) {
    
        if(MORETHAN(attack_, 2.6))
            attack = exp(-1.0/(sampleRate*2.6));
        
        else if(LESSTHAN(attack_, 0.00001))
            attack = exp(-1.0/(sampleRate*0.00001));
        
        else
            attack = exp(-1.0/(sampleRate*attack_));

    
    }
    
    /*******************************************************************
     *Método setRelease
     *Parâmetros: double
     *actualiza o estado da variavel define o tempo de decaimento  do simulador
     *de detector de pico que produz o envelope do sinal
     *retorna void
     *******************************************************************/
    void AelCompressor::setRelease(double release_){
    
        if(MORETHAN(release_, 5.0))
            release = exp(-1.0/(sampleRate*5.0));
        
        else if(LESSTHAN(release_, 0.130))
            release = exp(-1.0/(sampleRate*0.130));
        
        else
            release = exp(-1.0/(sampleRate*release_));
        
    
    }
    
    /*******************************************************************
     *Função membro getCopy
     *Parâmetros: void
     *Retorna Cópia do efeito, no estado actual em que se encontra
     *******************************************************************/
    AelEffect* AelCompressor::getCopy() {
        
        AelEffect *temp =  new AelCompressor(ratio, thresholdDB, getAttackTime(), getReleaseTime(), n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    
    /*******************************************************************
     *Função membro processFrame
     *Parâmetros: AelFrame& iframe
     *Responsável por processar a frame recebida pelo efeito compressor
     *******************************************************************/
    AelFrame& AelCompressor::processFrame(AelFrame& frame){
        
        static int prev_sample = 0;
        int peak;
        int sample = abs(frame.maxSample());
        
        //Gera o envelope do efeito simulando um circuito detector de pico
        //de acordo com os parametros de attack e release time
        if(sample >= prev_sample){
            peak = attack * prev_sample + (1-attack) * sample;
        } else {
            peak = release * prev_sample + (1-release) * sample;
        }
        
        prev_sample = peak;
        
        //envelope check test
        //frame[0] = frame[1] = peak;
        
        //Caso o envelope passe do limite threshold
        //calcula o ganho corresponde de acordo com o ratio
        //e faz o ajuste do sinal para o nível necessário
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
    
    /*******************************************************************
     *Método setThreshold
     *Parâmetros: double
     *actualiza o estado da variavel define o nível threshold do gate
     *dentro dos limites establecidos, convertendo o parametro recebido
     *em decibeis para um valor absoluto
     *retorna void
     *******************************************************************/
    void AelGate::setThreshold(double thresholddb){
        
        if(MORETHAN(thresholddb, -1.0))
            thresholdDB = -1.0;
        
        else if(LESSTHAN(thresholddb, -70.0))
            thresholdDB = -70.0;
        
        else
            thresholdDB = thresholddb;
        
        
        threshold = MAX_SAMPLE_VALUE * pow(10.0, thresholdDB/20);
        
        
    }
    
    /*******************************************************************
     *Método setAttenuation
     *Parâmetros: double
     *actualiza o estado da variavel define a atenuação do gate
     *dentro dos limites establecidos, convertendo o valor recebido em 
     *decibeis para um valor absoluto
     *retorna void
     *******************************************************************/
    void AelGate::setAttenuation(double att){
        
        if(MORETHAN(att, 0.0))
            attenuationDB = 0.0;
        
        else if(LESSTHAN(att, -70.0))
            attenuationDB = -70.0;
        
        else
            attenuationDB = att;
        
        
        attenuation = pow(10.0, attenuationDB/20);
        
        
    }
    
    /*******************************************************************
     *Método setAttack
     *Parâmetros: double
     *actualiza o estado da variavel define o tempo de subida do simulador
     *de detector de pico que produz o envelope do sinal
     *retorna void
     *******************************************************************/
    void AelGate::setAttack(double attack_) {
        
        
        if(MORETHAN(attack_, 2.6))
            attack = exp(-1.0/(sampleRate*2.6));
        
        else if(LESSTHAN(attack_, 0.00001))
            attack = exp(-1.0/(sampleRate*0.00001));
        
        else
            attack = exp(-1.0/(sampleRate*attack_));
        
        
    }
    
    /*******************************************************************
     *Método setRelease
     *Parâmetros: double
     *actualiza o estado da variavel define o tempo de decaimento do simulador
     *de detector de pico que produz o envelope do sinal
     *retorna void
     *******************************************************************/
    void AelGate::setRelease(double release_){
        
        if(MORETHAN(release_, 5.0))
            release = exp(-1.0/(sampleRate*5.0));
        
        else if(LESSTHAN(release_, 0.130))
            release = exp(-1.0/(sampleRate*0.130));
        
        else
            release = exp(-1.0/(sampleRate*release_));
        
        
    }
    
    /*******************************************************************
     *Função membro getCopy
     *Parâmetros: void
     *Retorna Cópia do efeito, no estado actual em que se encontra
     *******************************************************************/
    AelEffect* AelGate::getCopy() {
        
        AelEffect *temp =  new AelGate(thresholdDB, attenuationDB, getAttackTime(), getReleaseTime(), n_channels, sampleRate);
        temp->setWetLevel(wet_level);
        if(!onoff)
            temp->m_turnOff();
        
        return temp;
    }
    

    /*******************************************************************
     *Função membro processFrame
     *Parâmetros: AelFrame& iframe
     *Responsável por processar a frame recebida pelo efeito compressor
     *******************************************************************/
    AelFrame& AelGate::processFrame(AelFrame& frame){
        
        static int prev_sample = 0;
        int peak;
        int sample = abs(frame.maxSample());

        //Gera o envelope do efeito simulando um circuito detector de pico
        //de acordo com os parametros de attack e release time
        if(sample >= prev_sample){
            peak = attack * prev_sample + (1-attack) * sample;
        } else {
            peak = release * prev_sample + (1-release) * sample;
        }
        
        prev_sample = peak;
        
        //Caso o envelope seja abaixo limite threshold
        //multiplica o sinal pela atenuação definida
        if(peak < threshold)
            frame = frame * attenuation * getWetLevel() + frame * (1 - getWetLevel());

        return frame;
    }


    
}
