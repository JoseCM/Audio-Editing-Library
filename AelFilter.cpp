//
//  AelFilter.cpp
//  AEL
//
//  Created by Paulo Jorge Felix Oliveira on 01/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#include "AelFilter.h"


namespace Ael {

AelFilter::AelFilter(int sampleR, float gain_, float cutoff_)
    : AelEffect(sampleR), gain(gain_), cutoff(cutoff_) {
    a.assign(2, 0.0);
    b.assign(3, 0.0);
    a[0] = b[0] = 1.0;
    in_.assign(3, 0.0), out_.assign(3, 0.0);
}


}