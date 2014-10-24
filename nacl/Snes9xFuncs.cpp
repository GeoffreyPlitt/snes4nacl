/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#include "snes9x.h"
#include "memmap.h"
#include "debug.h"
#include "cpuexec.h"
#include "ppu.h"
#include "snapshot.h"
#include "apu/apu.h"
#include "display.h"
#include "gfx.h"
#include "movie.h"
#include "netplay.h"
#include "conffile.h"
#include "cheats.h"
#include "movie.h"
#include "logger.h"
#include "controls.h"

#include "nacl/Graphics.h"
#include "Instance.h"

void S9xTextMode()
{
}

void S9xGraphicsMode()
{
}

void S9xExit()
{

}

void S9xMessage(int type, int number, char const *message)
{
	printf("S9xMessage() -- %s", message);
}

const char *S9xGetFilename (const char *e, enum s9x_getdirtype dirtype)
{
	static char filename [PATH_MAX + 1];
    char dir [_MAX_DIR + 1];
    char drive [_MAX_DRIVE + 1];
    char fname [_MAX_FNAME + 1];
    char ext [_MAX_EXT + 1];
    unsigned int i=0;
    const char *d;

    _splitpath (Memory.ROMFilename, drive, dir, fname, ext);
    d=S9xGetDirectory(dirtype);
    snprintf(filename, sizeof(filename), "%s\\%s%03d%s", d, fname, i, e);

    return (filename);
}

const char *S9xGetDirectory (enum s9x_getdirtype dirtype)
{
	static char s[PATH_MAX + 1];

	switch (dirtype)
	{
		case SNAPSHOT_DIR:
			return "/states/";
			break;
		case CHEAT_DIR:
			return "/cheats/";
			break;
		case SRAM_DIR:
		case ROMFILENAME_DIR:

			strncpy(s, Memory.ROMFilename, PATH_MAX + 1);
			s[PATH_MAX] = 0;

			for (int i = strlen(s); i >= 0; --i)
			{
				if (s[i] == SLASH_CHAR)
				{
					s[i] = 0;
					break;
				}
			}

			return s;
			break;
		default:
			return "/";
	}
}

const char *S9xGetFilenameInc(const char *e, enum s9x_getdirtype dirtype)
{
	return NULL;
}

bool8 S9xOpenSnapshotFile(const char *fname, bool8 read_only, STREAM *file)
{
	return NULL;
}

void S9xCloseSnapshotFile(STREAM file)
{
}

void S9xSyncSpeed()
{
}

const char *S9xBasename(const char *f)
{
	return NULL;
}

bool8 S9xReadMousePosition(int which, int &x, int &y, uint32 &buttons)
{
	return false;
}

void S9xAutoSaveSRAM()
{
}

void S9xSetPause(uint32 mask)
{
}

void S9xClearPause(uint32 mask)
{
}

const char* S9xChooseFilename(bool8 read_only)
{
	return NULL;
}

void S9xToggleSoundChannel(int c)
{
}

void S9xExtraUsage()
{
}

const char* S9xChooseMovieFilename(bool8 read_only)
{
	return NULL;
}

void S9xSetPalette()
{
}

void S9xParsePortConfig(ConfigFile &conf, int pass)
{
}

void _splitpath (const char *path, char *drive, char *dir, char *fname, char *ext)
{
	*drive = 0;

	const char	*slash = strrchr(path, SLASH_CHAR);
	const char	*dot   = strrchr(path, '.');

	if (dot && slash && dot < slash)
	{
		dot = NULL;
	}

	if (!slash)
	{
		*dir = 0;

		strcpy(fname, path);

		if (dot)
		{
			fname[dot - path] = 0;
			strcpy(ext, dot + 1);
		}
		else
		{
			*ext = 0;
		}
	}
	else
	{
		strcpy(dir, path);
		dir[slash - path] = 0;

		strcpy(fname, slash + 1);

		if (dot)
		{
			fname[dot - slash - 1] = 0;
			strcpy(ext, dot + 1);
		}
		else
		{
			*ext = 0;
		}
	}
}

void _makepath (char *path, const char *, const char *dir, const char *fname, const char *ext)
{
	if (dir && *dir)
	{
		strcpy(path, dir);
		strcat(path, SLASH_STR);
	}
	else
	{
		*path = 0;
	}

	strcat(path, fname);

	if (ext && *ext)
	{
		strcat(path, ".");
		strcat(path, ext);
	}
}

bool8 S9xInitUpdate()
{
	return true;
}

bool8 S9xContinueUpdate(int width, int height)
{
	return true;
}

static int lastWidth = 0;
static int lastHeight = 0;
bool8 S9xDeinitUpdate(int width, int height)
{
	snes4nacl::Graphics& graphics = snes4nacl::Instance::get()->getEmulator().getGraphics();
	if(width != lastWidth || lastHeight != height)
	{
		graphics.sourceSize(width, height);
		lastWidth = width;
		lastHeight = height;
	}
	graphics.source((unsigned char*)(GFX.Screen));
	return true;
}

const char* S9xStringInput(const char *message)
{
	return NULL;
}

bool S9xPollAxis(uint32 id, int16_t *value)
{
	return false;
}


bool S9xPollButton(uint32 id, bool *pressed)
{
	return false;
}


bool S9xPollPointer(uint32 id, int16_t *x, int16_t *y)
{
	return false;
}

bool8 S9xOpenSoundDevice()
{
	return true;
}

void S9xHandlePortCommand(s9xcommand_t cmd, int16_t data1, int16_t data2)
{
}

void S9xParseArg(char** argv, int& i, int argc)
{
}