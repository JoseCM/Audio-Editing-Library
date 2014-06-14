//
//  Filters.h
//  AEL_PROJECT
//
//  Created by Paulo Jorge Felix Oliveira on 14/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#ifndef __AEL_PROJECT__Filters__
#define __AEL_PROJECT__Filters__

#include "AelFilter.h"
#include <cmath>

namespace Ael{
    
    class AelIIR: public AelFilter{
        AelFrame out_1, in_1;
        
        
    public:
        AelIIR(int Sr=44100, float G=1.0, float Coff=1.0 ,int n_ch=2, int n_coefa=2, int n_coefb=1) : AelFilter(Sr, G, Coff, n_coefa, n_coefb) , out_1(n_ch), in_1(n_ch) { }
        
        virtual void set_LPF();
        virtual void set_HPF();
        virtual AelFrame& processFrame(AelFrame&);
        virtual AelAudioStream& processStream(AelAudioStream &);
    };
    
    
    class ButterWorth: public AelFilter{
        AelFrame out_1, out_2, in_1, in_2;
        
    public:
        ButterWorth(int Sr=44100, float G=1.0, float Coff=1.0, int n_ch=2 ,int n_coefa=3, int n_coefb=2): AelFilter(Sr, G, Coff, n_coefa, n_coefb), out_1(n_ch), out_2(n_ch), in_1(n_ch), in_2(n_ch) { }
        
        virtual AelFrame& processFrame(AelFrame&);
        virtual AelAudioStream& processStream(AelAudioStream &);
        virtual void set_LPF();
        virtual void set_HPF();
        void set_NOTCH(float);
    };
    
    
}



#endif /* defined(__AEL_PROJECT__Filters__) */
