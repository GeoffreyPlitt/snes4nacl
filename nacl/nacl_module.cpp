/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */

#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "gles2/gl2.h"


#include "Instance.h"

namespace snes4nacl {

bool Module::Init()
{
	return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
}

pp::Instance* Module::CreateInstance(PP_Instance instance) 
{
	Instance* inst = new snes4nacl::Instance(instance);
	Instance::set(inst);
	return inst;
}

}

namespace pp {

//essentially main()
Module* CreateModule()
{
	return new snes4nacl::Module();
}

}