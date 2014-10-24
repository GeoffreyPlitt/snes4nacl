/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 

#include "apu/apu.h"
#include "nacl/Sound.h"
#include "nacl/Instance.h"

#define DEFAULT_SAMPLE_RATE 1024

namespace snes4nacl {

Sound::Sound() : sampleFrameCount(DEFAULT_SAMPLE_RATE)
{
}

Sound::~Sound()
{
}

void Sound::init()
{
	sampleFrameCount = pp::AudioConfig::RecommendSampleFrameCount(Instance::get(), PP_AUDIOSAMPLERATE_44100, DEFAULT_SAMPLE_RATE);
	pp::AudioConfig audioConfig = pp::AudioConfig(Instance::get(), PP_AUDIOSAMPLERATE_44100, sampleFrameCount);
	audio = pp::Audio(Instance::get(), audioConfig, getSound, Instance::get());
	printf("Sound::init() -- Sound system created, sample frame count is %d", sampleFrameCount);
}

void Sound::getSound(void* samples, uint32_t bufferSize, void* data) 
{
	S9xMixSamples((unsigned char*)samples, bufferSize / 2);
}

void Sound::start()
{
	printf("Sound::start() -- Audio playback started");
	audio.StartPlayback();
	
}

void Sound::stop()
{
	audio.StopPlayback();
	printf("Sound::start() -- Audio playback stopped");
}

}