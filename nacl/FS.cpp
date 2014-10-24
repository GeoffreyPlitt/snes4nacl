/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */
 
#include <string.h>
#include <stdio.h>
#include "ppapi/cpp/file_system.h"
#include "FS.h"
#include "Instance.h"

namespace snes4nacl {

FS::FS() 
{

}

FS::~FS()
{

}

void FS::init()
{
	printf("FS::init() -- Empty virtual file system created");
}

FS::Handle* FS::fopen(const char* filename, const char* mode, EPRESIST_MODE persistMode)
{
	printf("FS::fopen() -- Open request for %s with mode %s", filename, mode);
	switch(persistMode)
	{
		case PERSIST_MODE_Memory:
			if(strcmp(mode, "wb") == 0)
				return new InMemoryWriteHandle(this, filename);
			else if(strcmp(mode, "rb") == 0)
			{
				fs_t::iterator it(fs_data.find(filename));
				if(it != fs_data.end())
					return new InMemoryReadHandle(this, it);
			}
			break;
		case PERSIST_MODE_Disk:
			break;
	}
	return NULL;
}

FS::Handle* FS::fdopen(int fileno, const char* mode)
{
	return NULL;
}

int FS::unlink(const char* filename)
{
	return -1;
}


FS::Handle::Handle(FS* _parent, const char* _path) : parent(_parent), path(_path)
{
}

FS::Handle::~Handle()
{
}

FS::InMemoryWriteHandle::InMemoryWriteHandle(FS* _parent, const char* _path) : Handle(_parent, _path)
{
	printf("FS::InMemoryWriteHandle::InMemoryWriteHandle() -- Opened %s", _path);
	pending = new std::vector<unsigned char>();
	pos = pending->end();
}

FS::InMemoryWriteHandle::~InMemoryWriteHandle()
{
	apply();
}

size_t FS::InMemoryWriteHandle::fread(void* ptr, size_t size, size_t count)
{
	return 0;
}
	
size_t FS::InMemoryWriteHandle::fwrite(const void* data, size_t size, size_t count)
{
	const unsigned char* ptr = static_cast<const unsigned char*>(data);
	size_t bytes = size * count;
	if(bytes <= 0)
		return 0;
	std::vector<unsigned char>::iterator end(pending->end());
	size_t overwriteAvailable = end - pos;
	if(overwriteAvailable > 0)
	{
		size_t toOverwrite = std::min(overwriteAvailable, bytes);
		memcpy(&(*pos), ptr, toOverwrite);
		ptr += bytes;
		bytes -= toOverwrite;
		pos += bytes;
	}
	if(bytes > 0)
	{
		pending->insert(pos, ptr, ptr + bytes);
		pos += bytes;
	}
	return size * count;
}
	
void FS::InMemoryWriteHandle::apply()
{
	if(pending)
	{
		FS::fs_t::iterator it(parent->fs_data.find(path));
		if(it != parent->fs_data.end())
			delete it->second;
		parent->fs_data[path] = pending;
		printf("FS::InMemoryWriteHandle::apply() -- Put %d bytes into virtual filesystem for %s", pending->size(), path.c_str());
		pending = NULL;
	}
}

int FS::InMemoryWriteHandle::fclose()
{
	apply();
	delete this;
	return 0;
}

int FS::InMemoryWriteHandle::fgetc()
{
	return 0;
}

char* FS::InMemoryWriteHandle::fgets(char* buff, size_t len)
{
	return NULL;
}

int FS::InMemoryWriteHandle::fseek(int offset, int mode)
{
	switch(mode)
	{
		case SEEK_SET:
			pos = pending->begin() + offset;
			break;
		case SEEK_CUR:
			pos += offset;
			break;
		case SEEK_END:
			pos = pending->end() - offset;
			break;
		default:
			return -1;
	}
	return 0;
}

int FS::InMemoryWriteHandle::ftell()
{
	return pos - pending->begin();
}

FS::InMemoryReadHandle::InMemoryReadHandle(FS* _parent, const fs_t::iterator& file) : Handle(_parent, file->first.c_str())
{
	begin = &file->second->front();
	pos = begin;
	end = begin + file->second->size();
	printf("FS::InMemoryReadHandle::InMemoryReadHandle() -- Opened %s", file->first.c_str());
}

FS::InMemoryReadHandle::~InMemoryReadHandle()
{
}

size_t FS::InMemoryReadHandle::fread(void* ptr, size_t size, size_t count)
{
	printf("FS::InMemoryReadHandle::fread() -- Requesting read of %d items of length %d", count, size);
	int available = (end - begin) / size;
	printf("FS::InMemoryReadHandle::fread() -- %d items available", available);
	int toRead = std::min(available, (int)count);
	size_t toReadInBytes = toRead * size;
	if(toReadInBytes > 0)
	{
		memcpy(ptr, pos, toReadInBytes);
		pos += toReadInBytes;
		printf("FS::InMemoryReadHandle::fread() -- Read %d items (%d bytes)", toRead, toReadInBytes);
	}
	return toRead;	
}

size_t FS::InMemoryReadHandle::fwrite(const void* ptr, size_t size, size_t count)
{
	return 0;
}

int FS::InMemoryReadHandle::fclose()
{
	return 0;
}

int FS::InMemoryReadHandle::fgetc()
{
	return pos < end ? *pos++ : -1;
}

char* FS::InMemoryReadHandle::fgets(char* buff, size_t len)
{
	return NULL;
}

int FS::InMemoryReadHandle::fseek(int offset, int mode)
{
	switch(mode)
	{
		case SEEK_SET:
			pos = begin + offset;
			break;
		case SEEK_CUR:
			pos += offset;
			break;
		case SEEK_END:
			pos = end - offset;
			break;
		default:
			return -1;
	}
	return 0;
}

int FS::InMemoryReadHandle::ftell()
{
	return pos - begin;
}


}