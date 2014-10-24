/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_INPUT_H
#define _SNES4NACL_INPUT_H

#include "callback.h"
#include <map>

namespace snes4nacl {

class Input
{
public:
	Input();
	~Input();
	
	void init();
	
	void keyEvent(const ScriptingBridge& bridge, const MethodParameter& parameters);

private:
	std::map<const std::string, int> keyToId;
};

}

#endif