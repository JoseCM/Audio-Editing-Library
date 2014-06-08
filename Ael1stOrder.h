//
//  Ael1stOrder.h
//  AEL_PROJECT
//
//  Created by Paulo Jorge Felix Oliveira on 03/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#ifndef __AEL_PROJECT__Ael1stOrder__
#define __AEL_PROJECT__Ael1stOrder__


#include "AelFilter.h"
#include <cmath>


namespace Ael{
    
    class AelIIR: public AelFilter{
        AelFrame out_1;
        

    public:
        AelIIR(int Sr=44100, float G=1.0, float Coff=1.0 ,int n_ch=1, int n_coefa=1, int n_coefb=1) : AelFilter(Sr, G, Coff, n_coefa, n_coefb) , out_1(n_ch) { }
        
        virtual void set_LPF();
        virtual void set_HPF();
        virtual void set_NOTCH(float) { }
        virtual AelFrame& processFrame(AelFrame&);
        virtual AelAudioStream& processStream(AelAudioStream &);
        
        
        //void CalcCoef(int hp = 0);
        
    };
    
    
    
}








#endif /* defined(__AEL_PROJECT__Ael1stOrder__) */
