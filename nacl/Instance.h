/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_NACL_H
#define _SNES4NACL_NACL_H

#include "scripting_bridge.h" 
#include "ppapi/cpp/view.h"
#include "ppapi/cpp/input_event.h"
#include "nacl_module.h"
#include "Emulator.h"


#define printf(...) snes4nacl::Instance::get()->log(__VA_ARGS__)

namespace snes4nacl {

//Each <embed> object in the document will be one of these
class Instance : public pp::Instance
{
public:
	virtual ~Instance();
	static Instance* get();
	
	bool Init(uint32_t argc, const char* argn[], const char* argv[]);
	
	//the main way we interact with the browser is through this pipe
	void HandleMessage(const pp::Var& var_message);
	
	//incoming input!
	bool HandleInputEvent(const pp::InputEvent& event);
	
	//Called when resize, etc.
	void DidChangeView(const pp::View& view);
	
	void log(const char* format, ...);
	void sendStringMessage(const char* message);
	
	Emulator& getEmulator() { return emulator; }
	
private:
	friend class Module;
	explicit Instance(PP_Instance instance);
	static void set(Instance* to);
	static Instance* inst;
	
	ScriptingBridge bridge;
	Emulator emulator;
}; 

}


#endif