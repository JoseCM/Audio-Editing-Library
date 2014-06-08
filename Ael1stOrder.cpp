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
       
        float cos_ = cos(2*pi*cutoff/getSampleRate());
        coef_b[0] =  sqrt( pow( (2 - cos_) , 2) - 1 ) - 2 + cos_;
        coef_a[0] = 1 + coef_b[0];
        std::cout << coef_a[0] << " " << coef_b[0] << endl;
    }
    
    void AelIIR::set_HPF(){
        
        float cos_ = cos(2*pi*cutoff/getSampleRate());
        coef_b[0] = 2 - cos_ - sqrt( pow( (2 - cos_) , 2) - 1 );
        coef_a[0] = 1 - coef_b[0];
        std::cout << coef_a[0] << " " << coef_b[0] << endl;
    
    }
    
    AelFrame& AelIIR::processFrame(AelFrame& Frame){
    
        //y(n) = ax(n) - b y(n - 1).
        
        for(int i = 0; i < Frame.getChannels() ; i++){
            Frame[i] = gain * ( (coef_a[0] * Frame[i]) - (coef_b[0] * out_1[i]) );
            out_1[i] = Frame[i];
        }
    
        return Frame;
    }
    
    AelAudioStream& AelIIR::processStream(AelAudioStream &Stream){
        out_1[0] = out_1[1] = 0;
        
        AelAudioStream *temp = new AelAudioStream(Stream.getchannels());
        
        for (int i=0; i< Stream.getnframes(); i++){
            AelFrame aux = Stream.getNextFrame();
            aux = this->processFrame(aux);
            temp->AddFrames(aux);
        }
        
        return *temp;
    
    }
    
    /*
    void AelIIR::CalcCoef(int hp)
	{
		double x = exp(-2*pi * (cutoff/getSampleRate()));
		if (hp)
		{
			coef_a[0]= x;
			coef_b[0] = 1.0 - x;
		}
		else
		{
			coef_a[0] = 1.0 - x;
			coef_b[0] = -x;
		}
	}
     */
    
     
    
}