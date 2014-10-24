/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#include <stdarg.h>
#include <stdio.h>
#include "Instance.h"
#include "ppapi/cpp/var.h"
#include "snes9x.h"

namespace snes4nacl {

Instance* Instance::inst = NULL;

Instance::Instance(PP_Instance instance) : pp::Instance(instance)
{
	log("Instance::Instance() -- NaCl instance successfully created");
}

Instance::~Instance()
{
}

void Instance::set(Instance* to)
{
	inst = to;
}

Instance* Instance::get()
{
	return inst;
}

void Instance::sendStringMessage(const char* message)
{
	PostMessage(pp::Var(message));
}

void Instance::HandleMessage(const pp::Var& var_message)
{
	if(var_message.is_string())
		bridge.InvokeMethod(var_message.AsString());
	else
		log("Got an unknown message type");
}

void Instance::DidChangeView(const pp::View& view)
{
	emulator.DidChangeView(view);
}

void Instance::log(const char* format, ...)
{
	char out[1024];
	char realFormat[1024];
	snprintf(realFormat, 1023, "msg:%s", format);
	va_list arg_ptr;
	va_start(arg_ptr, format);
	vsnprintf(out, 1023, realFormat, arg_ptr);
	va_end(arg_ptr);
	sendStringMessage(out);
}

bool Instance::Init(uint32_t argc, const char* argn[], const char* argv[])
{
    RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);

	ScriptingBridge::SharedMethodCallbackExecutor emulator_init(
      new MethodCallback<Emulator>(
          &emulator, &Emulator::init));	
	bridge.AddMethodNamed("init", emulator_init);
	
	ScriptingBridge::SharedMethodCallbackExecutor emulator_frame(
      new MethodCallback<Emulator>(
          &emulator, &Emulator::frame));	
	bridge.AddMethodNamed("frame", emulator_frame);
	
	ScriptingBridge::SharedMethodCallbackExecutor emulator_downloadThenLoadRom(
      new MethodCallback<Emulator>(
          &emulator, &Emulator::downloadThenLoadRom));	
	bridge.AddMethodNamed("downloadThenLoadRom", emulator_downloadThenLoadRom);
	
	ScriptingBridge::SharedMethodCallbackExecutor emulator_play(
      new MethodCallback<Emulator>(
          &emulator, &Emulator::play));	
	bridge.AddMethodNamed("play", emulator_play);
	
	ScriptingBridge::SharedMethodCallbackExecutor emulator_pause(
      new MethodCallback<Emulator>(
          &emulator, &Emulator::pause));	
	bridge.AddMethodNamed("pause", emulator_pause);
	
	ScriptingBridge::SharedMethodCallbackExecutor input_keyEvent(
      new MethodCallback<Input>(
          &emulator.getInput(), &Input::keyEvent));	
	bridge.AddMethodNamed("keyEvent", input_keyEvent);
	
	ScriptingBridge::SharedMethodCallbackExecutor graphics_setSmoothScale(
      new MethodCallback<Graphics>(
          &emulator.getGraphics(), &Graphics::setSmoothScale));	
	bridge.AddMethodNamed("setSmoothScale", graphics_setSmoothScale);
	
	
	return true;
}

bool Instance::HandleInputEvent(const pp::InputEvent& event)
{
	return emulator.HandleInputEvent(event);
}

}