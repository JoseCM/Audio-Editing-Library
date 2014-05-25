#ifndef __AEL_EFFECTS__
#define __AEL_EFFECTS__

#include <string>

using namespace std;


//////////////////////////////////////////////
// AUDIO EFFECTS

namespace Ael {

	class AelEffects{
		string effectName;
	public:
		virtual string getEffectName() = 0;
		// int m_turnOn();
	};


}

#endif