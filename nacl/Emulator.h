/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#ifndef _SNES4NACL_EMULATOR_H
#define _SNES4NACL_EMULATOR_H

#include "Graphics.h"
#include "Sound.h"
#include "Input.h"
#include "URLDownloader.h"
#include "FS.h"
#include "scripting_bridge.h"
#include "ppapi/cpp/input_event.h"
#include <string>

namespace snes4nacl {

class Emulator
{
public:

	Emulator();
	~Emulator();
	
	void DidChangeView(const pp::View& view);
	bool HandleInputEvent(const pp::InputEvent& event);

	void init(const ScriptingBridge& bridge, const MethodParameter& parameters);
	void frame(const ScriptingBridge& bridge, const MethodParameter& parameters);
	void downloadThenLoadRom(const ScriptingBridge& bridge, const MethodParameter& parameters);
	void downloadThenLoadRom(const std::string& url);
	void loadRom(const std::string& path);
	void play(const ScriptingBridge& bridge, const MethodParameter& parameters);
	void pause(const ScriptingBridge& bridge, const MethodParameter& parameters);
	
	Graphics& getGraphics() { return graphics; }
	FS& getFS() { return fs; }
	Input& getInput() { return input; }
	
private:

	class ROMDownloader : public URLDownloader
	{
	public:
		ROMDownloader(Emulator* emu, const std::string& _url);
		const std::string& getURL() { return url; }
	protected:
		void onFinish(int httpResponseCode, const unsigned char* data, int numBytes);
		Emulator* emu;
		std::string url;
		
	};
	
	void romDownloadFinished(const unsigned char* data, int numBytes);

	Graphics graphics;
	FS fs;
	Sound sound;
	Input input;
	ROMDownloader* downloader;
	pp::Rect windowSize;
	
	bool playing;
	bool romLoaded;

};

}

#endif