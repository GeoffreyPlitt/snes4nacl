/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#include "snes9x.h"
#include "memmap.h"
#include "Emulator.h"
#include "Instance.h"
#include "apu/apu.h"
#include "display.h"

 
 namespace snes4nacl {
 
 Emulator::Emulator() : windowSize(SNES_WIDTH, SNES_HEIGHT), playing(false), romLoaded(false)
 {
	downloader = NULL;
 }
 
 Emulator::~Emulator()
 {
 }
 
 
 void Emulator::init(const ScriptingBridge& bridge, const MethodParameter& parameters)
 {
	printf("Emulator::init() -- Starting the show");
	
	memset(&Settings, 0, sizeof(Settings));
	Settings.MouseMaster = false;
	Settings.SuperScopeMaster = false;
	Settings.JustifierMaster = false;
	Settings.MultiPlayer5Master = false;
	Settings.FrameTimePAL = 20000;
	Settings.FrameTimeNTSC = 16667;
	Settings.SixteenBitSound = true;
	Settings.Stereo = true;
	Settings.SoundPlaybackRate = 44100;
	Settings.SoundInputRate = 32000;//32000;
	Settings.SoundSync = true;
	Settings.SupportHiRes = true;
	Settings.Transparency = true;
	Settings.AutoDisplayMessages = true;
	Settings.InitialInfoStringTimeout = 120;
	Settings.HDMATimingHack = 100;
	Settings.BlockInvalidVRAMAccessMaster = true;
	Settings.OpenGLEnable = true;
	Settings.DisplayFrameRate = true;
	
	fs.init();
	graphics.init();
	graphics.size(windowSize.width(), windowSize.height());
	sound.init();
	input.init();
	
	Instance::get()->sendStringMessage("initFinished");
 }
 
 Emulator::ROMDownloader::ROMDownloader(Emulator* _emu, const std::string& _url) : URLDownloader(Instance::get(), _url), emu(_emu), url(_url)
 {
 }
 
 void Emulator::ROMDownloader::onFinish(int httpResponseCode, const unsigned char* data, int numBytes)
 {
	if(httpResponseCode != 200)
		return;
	printf("Emulator::ROMDownloader::onFinish() -- ROM download finished, executing");	
	emu->romDownloadFinished(data, numBytes);
 }
 
 void Emulator::downloadThenLoadRom(const std::string& url)
 {
	if(downloader)
		return;

	downloader = new ROMDownloader(this, url);
	downloader->start();
 }
 
 void Emulator::downloadThenLoadRom(const ScriptingBridge& bridge, const MethodParameter& parameters)
 {
	MethodParameter::const_iterator it(parameters.find("url"));
	if(it != parameters.end())
		downloadThenLoadRom(it->second);
 }
 
 void Emulator::romDownloadFinished(const unsigned char* data, int numBytes)
 {
	if(downloader)
	{	
		FS::Handle* fp = fs.fopen(downloader->getURL().c_str(), "wb");
		fp->fwrite(data, 1, numBytes);
		fp->fclose();
		loadRom(downloader->getURL());
		fs.unlink(downloader->getURL().c_str());
		delete downloader;
		downloader = NULL;
	}
 }
 
 void Emulator::loadRom(const std::string& path)
 {
	printf("Emulator::loadRom() -- Starting load of %s", path.c_str());
	Memory.Init();
	S9xDeinitAPU();
	S9xInitAPU();
	if (!S9xInitSound(64, 0))
	{
		printf("Emulator::loadRom() -- S9xInitSound() failed");
		return;
	}
	S9xSetRenderPixelFormat(RGB565);
	if(!Memory.LoadROM(path.c_str()))
	{
		printf("Emulator::loadRom() -- Error loading %s", path.c_str());
		return;
	}
	romLoaded = true;
	Memory.LoadSRAM(S9xGetFilename(".srm", SRAM_DIR));
	
	if (!S9xGraphicsInit())
	{
		printf("Emulator::loadRom() -- S9xGraphicsInit() failed");
		return;
	}
	graphics.sourceSize(SNES_WIDTH, SNES_HEIGHT);
	printf("Emulator::loadRom() -- Successfully loaded %s", path.c_str());
	Instance::get()->sendStringMessage("loadFinished");
 }
 
 void Emulator::frame(const ScriptingBridge& bridge, const MethodParameter& parameters)
 {
	if(S9xSyncSound())
		S9xMainLoop();
	graphics.draw();
 }
 
 void Emulator::DidChangeView(const pp::View& view)
 {
	windowSize = view.GetRect();
	if(graphics.isInited())
		graphics.size(windowSize.width(), windowSize.height());
 }
 
 void Emulator::play(const ScriptingBridge& bridge, const MethodParameter& parameters)
 {
	if(!playing)
	{
		S9xSetSoundMute(false);
		sound.start();
		playing = true;
	}
 }
 
 void Emulator::pause(const ScriptingBridge& bridge, const MethodParameter& parameters)
 {
	if(playing)
	{	
		S9xSetSoundMute(true);
		sound.stop();
		playing = false;
	}
 }
 
 bool Emulator::HandleInputEvent(const pp::InputEvent& event)
 {
	switch (event.GetType()) {
      case PP_INPUTEVENT_TYPE_IME_COMPOSITION_START:
      case PP_INPUTEVENT_TYPE_IME_COMPOSITION_UPDATE:
      case PP_INPUTEVENT_TYPE_IME_COMPOSITION_END:
      case PP_INPUTEVENT_TYPE_IME_TEXT:
      case PP_INPUTEVENT_TYPE_UNDEFINED:
      case PP_INPUTEVENT_TYPE_MOUSEDOWN:
      case PP_INPUTEVENT_TYPE_MOUSEUP:
      case PP_INPUTEVENT_TYPE_MOUSEMOVE:
      case PP_INPUTEVENT_TYPE_MOUSEENTER:
      case PP_INPUTEVENT_TYPE_MOUSELEAVE:
	  case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
      case PP_INPUTEVENT_TYPE_KEYDOWN:
      case PP_INPUTEVENT_TYPE_CHAR:
      case PP_INPUTEVENT_TYPE_CONTEXTMENU:
	  default:
		break;
	   case PP_INPUTEVENT_TYPE_KEYUP:
        {
			pp::KeyboardInputEvent key_event(event);
			
			if(key_event.GetKeyCode() == 120) //F9
			{
				if(graphics.isInited())
					graphics.toggleFullScreen();
				return true;
			}
        }
    }
	return false;
 }
 
 }