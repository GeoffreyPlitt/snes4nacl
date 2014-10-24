/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_SOUND_H
#define _SNES4NACL_SOUND_H

#include "ppapi/cpp/audio.h"

namespace snes4nacl {

class Sound
{
public:
	Sound();
	~Sound();
	
	void init();
	void start();
	void stop();
	
private:
	pp::Audio audio;
	unsigned int sampleFrameCount;
	
	static void getSound(void* samples, uint32_t bufferSize, void* data);
};

}

#endif