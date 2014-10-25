
#
# Project information
#
# These variables store project specific settings for the project name
# build flags, files to copy or install. �In the examples it is typically
# only the list of sources and project name that will actually change and
# the rest of the makefile is boilerplate for defining build rules.
#
PROJECT := snes4nacl
LDFLAGS := -lppapi_gles2 -lppapi_cpp -lppapi -lnosys
CXX_SOURCES := 	bsx.cpp c4.cpp c4emu.cpp cheats.cpp cheats2.cpp clip.cpp conffile.cpp \
				controls.cpp cpu.cpp cpuexec.cpp cpuops.cpp crosshairs.cpp debug.cpp \
				dma.cpp dsp.cpp dsp1.cpp dsp2.cpp dsp3.cpp dsp4.cpp fxdbg.cpp \
				fxemu.cpp fxinst.cpp gfx.cpp globals.cpp loadzip.cpp logger.cpp \
				memmap.cpp movie.cpp netplay.cpp obc1.cpp ppu.cpp reader.cpp sa1.cpp \
				sa1cpu.cpp screenshot.cpp sdd1.cpp sdd1emu.cpp server.cpp seta.cpp \
				seta010.cpp seta011.cpp seta018.cpp snapshot.cpp snes9x.cpp \
				spc7110.cpp srtc.cpp tile.cpp apu/SNES_SPC.cpp apu/SNES_SPC_misc.cpp \
				apu/SNES_SPC_state.cpp apu/SPC_DSP.cpp apu/SPC_Filter.cpp apu/apu.cpp \
				nacl/nacl_module.cpp nacl/Snes9xFuncs.cpp nacl/Instance.cpp \
				nacl/Emulator.cpp nacl/Graphics.cpp nacl/OpenGL.cpp nacl/URLDownloader.cpp \
				nacl/FS.cpp nacl/scripting_bridge.cpp nacl/Sound.cpp nacl/Input.cpp

#
# Get pepper directory for toolchain and includes.
#
# If PEPPER_ROOT is not set, then assume it can be found a two directories up,
# from the default example directory location.
#
THIS_MAKEFILE:=$(abspath $(lastword $(MAKEFILE_LIST)))
INCLUDE_PATHS = $(abspath $(dir $(THIS_MAKEFILE)))

# Project Build flags
WARNINGS := -Wno-variadic-macros -Wno-long-long -Wall -pedantic -Werror
CXXFLAGS := -pthread -std=gnu++98 $(WARNINGS) \
						-I$(NACL_SDK_ROOT)/include \

#
# Compute tool paths
#
#
OSNAME:=$(shell python $(NACL_SDK_ROOT)/tools/getos.py)
TC_PATH:=$(abspath $(NACL_SDK_ROOT)/toolchain/$(OSNAME)_x86_newlib)
CXX:=$(TC_PATH)/bin/i686-nacl-g++

#
# Disable DOS PATH warning when using Cygwin based tools Windows
#
CYGWIN ?= nodosfilewarning
export CYGWIN

# Declare the ALL target first, to make the 'all' target the default build
all: $(PROJECT)_x86_32.nexe $(PROJECT)_x86_64.nexe

# Define 32 bit compile and link rules for C++ sources
x86_32_OBJS:=$(patsubst %.cpp,%_32.o,$(CXX_SOURCES))
$(x86_32_OBJS) : %_32.o : %.cpp $(THIS_MAKE)
	$(CXX) -I $(INCLUDE_PATHS) -o $@ -c $< -m32 -O0 -g $(CXXFLAGS) \
	-L$(NACL_SDK_ROOT)/lib/newlib_x86_32/Release

$(PROJECT)_x86_32.nexe : $(x86_32_OBJS)
	$(CXX) -I $(INCLUDE_PATHS) -o $@ $^ -m32 -O0 -g $(CXXFLAGS) $(LDFLAGS) \
	-L$(NACL_SDK_ROOT)/lib/newlib_x86_32/Release

# Define 64 bit compile and link rules for C++ sources
x86_64_OBJS:=$(patsubst %.cpp,%_64.o,$(CXX_SOURCES))
$(x86_64_OBJS) : %_64.o : %.cpp $(THIS_MAKE)
	$(CXX) -I $(INCLUDE_PATHS) -o $@ -c $< -m64 -O0 -g $(CXXFLAGS) \
	-L$(NACL_SDK_ROOT)/lib/newlib_x86_64/Release

$(PROJECT)_x86_64.nexe : $(x86_64_OBJS)
	$(CXX) -I $(INCLUDE_PATHS) -o $@ $^ -m64 -O0 -g $(CXXFLAGS) $(LDFLAGS) \
	-L$(NACL_SDK_ROOT)/lib/newlib_x86_64/Release


# Define a phony rule so it always runs, to build nexe and start up server.
.PHONY: RUN
RUN: all
	python httpd.py

clean:
	@rm -f $(x86_32_OBJS)
	@rm -f $(x86_64_OBJS)

serve:
	python -m SimpleHTTPServer 8080

package:
	tar zcvf package.tar.gz *.nexe
