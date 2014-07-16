//
//  AelFilter.h
//  AEL
//
//  Created by Paulo Jorge Felix Oliveira on 01/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#ifndef __AEL__AelFilter__
#define __AEL__AelFilter__

#include "AelEffects.h"

namespace Ael {
    
    ///////////////////////////////////////////////////////////////////////////////
    // Classe AelFilter
    //  Classe base com as características comuns aos filtros
    // Variáveis-Membro:
    //   vector<float> (coef_a, coef_b), float (gain, cutoff)
    // Funções-Membro:
    //   1 Construtores
    //   Métodos set e get das variáveis membro
    //   Métodos puramente virtual: getCopy, processFrame
    /////////////////////////////////////////////////////////////////////////////
    class AelFilter : public AelEffect {
    
    protected:
        vector<float> coef_a, coef_b;
        float gain, cutoff;
        
    public:
        AelFilter(float =1.0, float =4410, int n_ch=2, int =44100);
        virtual ~AelFilter() {    }
        virtual void set_gain(float);
        virtual void set_cutoff(float) = 0;
        float get_gain() { return gain; }
        float get_cutoff() { return cutoff; }
        virtual AelEffect* getCopy() = 0;
        virtual AelFrame& processFrame(AelFrame&) = 0;
    };
    
    ///////////////////////////////////////////////////////////////////////////////
    // Classe AelIIR
    //  Classe que implementa um filtro de 1ª ordem IIR
    // Variáveis-Membro:
    //   AelFrame (out_1, in_1), enum iir (ON)
    // Funções-Membro:
    //   1 Construtores
    //   Implementação de métodos virtuais da classe base (getCopy e processframe)
    //   Métodos que definem comporatmento do filtro (LPF ou HPF)
    /////////////////////////////////////////////////////////////////////////////
    class AelIIR: public AelFilter{
        
        enum iir {lowpass, highpass} ON;
        AelFrame out_1, in_1;
        
    public:
        AelIIR(float G=1.0, float Coff=4410 ,int n_ch=2, int Sr=44100);
        virtual ~AelIIR() {  }
        void set_LPF();
        void set_HPF();
        enum iir get_state() { return ON; }
        virtual void set_cutoff(float);
        virtual AelFrame& processFrame(AelFrame&);
        virtual AelEffect* getCopy();
    };
    
    ///////////////////////////////////////////////////////////////////////////////
    // Classe AelButterWorth
    //  Classe que implementa um filtro genérico de 2ª ordem buttherWorth
    // Variáveis-Membro:
    //   AelFrame (out_1, in_1, out_2, in_2), enum butterworth(ON), float (bandwith)
    // Funções-Membro:
    //   1 Construtores
    //   Implementação de métodos virtuais da classe base (getCopy e processframe)
    //   Métodos que definem comporatmento do filtro (LPF, HPF, BPF, NOTCH)
    /////////////////////////////////////////////////////////////////////////////
    class AelButterWorth: public AelFilter{
        
        enum butterworth {lowpass, highpass, bandpass, rejectband} ON;
        AelFrame out_1, out_2, in_1, in_2;
        float BandWidth;
        
    public:
        AelButterWorth(float G=1.0, float Coff=4410, float BW =970, int n_ch=2, int Sr=44100);
        virtual ~AelButterWorth(){ }
        void set_LPF();
        void set_HPF();
        void set_NOTCH();
        void set_BPF();
        void set_bandwidth(float);
        enum butterworth get_state() { return ON; }
        virtual void set_cutoff(float);
        virtual AelFrame& processFrame(AelFrame&);
        virtual AelEffect* getCopy();
    };
    
}
#endif /* defined(__AEL__AelFilter__) */
