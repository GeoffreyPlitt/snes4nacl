/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#include "snes9x.h"
#include "controls.h"
#include "Input.h"
#include "Instance.h"

namespace snes4nacl {

Input::Input()
{
}

Input::~Input()
{
}

void Input::init()
{
	#define MAP_KEY(NAME, ID) S9xMapButton(keyToId[NAME] = runner++, S9xGetCommandT(ID), false);
	int runner = 0;
	MAP_KEY("J1_UP", "Joypad1 Up");
	MAP_KEY("J1_DOWN", "Joypad1 Down");
	MAP_KEY("J1_LEFT", "Joypad1 Left");
	MAP_KEY("J1_RIGHT", "Joypad1 Right");
	MAP_KEY("J1_START", "Joypad1 Start");
	MAP_KEY("J1_SELECT", "Joypad1 Select");
	MAP_KEY("J1_A", "Joypad1 A");
	MAP_KEY("J1_B", "Joypad1 B");
	MAP_KEY("J1_X", "Joypad1 X");
	MAP_KEY("J1_Y", "Joypad1 Y");
	MAP_KEY("J1_L", "Joypad1 L");
	MAP_KEY("J1_R", "Joypad1 R");
	printf("Input::init() -- Created key mappings");
}

void Input::keyEvent(const ScriptingBridge& bridge, const MethodParameter& parameters)
{
	MethodParameter::const_iterator direction(parameters.find("d"));
	MethodParameter::const_iterator control(parameters.find("c"));
	if(direction != parameters.end() && control != parameters.end())
	{
		std::map<const std::string, int>::iterator id(keyToId.find(control->second));
		if(id != keyToId.end())
		{
			//printf("Input::keyEvent() -- Control %s, direction %s", control->second.c_str(), direction->second.c_str());
			if( direction->second == "up" )
				S9xReportButton(id->second, false);
			else
				S9xReportButton(id->second, true);
		}
	}
}


}