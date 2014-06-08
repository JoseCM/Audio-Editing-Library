//
//  AelFilter.cpp
//  AEL
//
//  Created by Paulo Jorge Felix Oliveira on 01/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#include "AelFilter.h"


namespace Ael {

    AelFilter::AelFilter(int sampleR, float gain_, float cutoff_, int n_coefa, int n_coefb) : AelEffect(sampleR), gain(gain_), cutoff(cutoff_) {
    
    coef_a.assign(n_coefa, 0.0);
    coef_b.assign(n_coefb, 0.0);
    
    }
    
    


}