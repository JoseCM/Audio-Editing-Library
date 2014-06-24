#ifndef __AEL_EXECPTION__
#define __AEL_EXECPTION__

#include <exception>
#include <string>

//////////////////////////////////////////////
// AEL EXCEPTION

using namespace std;

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