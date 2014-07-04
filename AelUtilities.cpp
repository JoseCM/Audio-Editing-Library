//
//  AelUtilities.cpp
//  AudioEditingLibrary
//
//  Created by José Martins on 03/07/14.
//  Copyright (c) 2014 José Martins. All rights reserved.
//

#include "AelUtilities.h"

namespace Ael{

AelExecption::AelExecption(string n) : exception(), name(n){}
AelExecption::~AelExecption() { }
const char* AelExecption::what() { return name.c_str(); }

}