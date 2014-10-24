/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */

#ifndef _SNES4NACL_NACL_MODULE_H
#define _SNES4NACL_NACL_MODULE_H 
 
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"

namespace snes4nacl {

//The one and only module we provide, the snes4nacl module.
//From here, we can create many instances of the same type (if we wanted to)
class Module : public pp::Module
{
public:

	//This is called when the snes4nacl module is loaded
	virtual bool Init();
	
	//For each <embed> object, we need to return a new instace
	virtual pp::Instance* CreateInstance(PP_Instance instance);

};

}


#endif