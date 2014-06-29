//
//  AelUtilities.h
//  AEL_PROJECT
//
//  Created by Paulo Jorge Felix Oliveira on 29/06/14.
//  Copyright (c) 2014 Paulo Jorge Felix Oliveira. All rights reserved.
//

#ifndef AEL_PROJECT_AelUtilities_h
#define AEL_PROJECT_AelUtilities_h

#include <cmath>
#include <limits>

#define MAX_SAMPLE_VALUE ( (pow(2.0, 32.0)/2) - 1 )

#define STREAM_LEN  ((m_nframes) * (channels))

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#define pi 3.14159265359

#define EQUAL(A, B) ( fabs(A-B) < std::numeric_limits<float>::epsilon())
#define MORETHAN(A, B) ( fabs(A-B) > std::numeric_limits<float>::epsilon() && (A > B) )
#define MORETHAN(A, B) ( fabs(A-B) > std::numeric_limits<float>::epsilon() && (A < B) )
#define LESSEQ(A, B) (EQUAL(A, B) || LESSTHAN(A, B))
#define MOREEQ(A, B) (EQUAL(A, B) || MORETHAN(A, B))

namespace Ael{
    
    
	class AelExecption : public exception{
	private:
		string name;
	public:
		AelExecption(string n) : exception(), name(n){};
		const char* what() { return name.c_str(); }
	};
    
}

#endif
