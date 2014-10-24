/**
 *  snes4nacl
 *  (c) 2012 Jeffrey Quesnelle <jquesnelle@gmail.com>
 *  This code is licensed under GPLv3, see LICENSE 
 */

#ifndef _SNES4NACL_FS_H
#define _SNES4NACL_FS_H
 
#include <map>
#include <vector>
#include <string>

namespace snes4nacl {

class FS
{
public:
	FS();
	~FS();
	void init();
	
	
	class Handle
	{
	public:
		Handle(FS* parent, const char* path);
		virtual ~Handle();
		virtual size_t fread(void* ptr, size_t size, size_t count) = 0;
		virtual size_t fwrite(const void* ptr, size_t size, size_t count) = 0;
		virtual int fclose() = 0;
		virtual int fgetc() = 0;
		virtual char* fgets(char* buff, size_t len) = 0;
		virtual int fseek(int offset, int mode) = 0;
		virtual int ftell() = 0;
	protected:
		FS* parent;
		const std::string path;
	};
	
	enum EPRESIST_MODE
	{
		PERSIST_MODE_Memory,
		PERSIST_MODE_Disk
	};
	
	Handle* fopen(const char* filename, const char* mode, EPRESIST_MODE persistMode = PERSIST_MODE_Memory);
	Handle* fdopen(int fileno, const char* mode);
	int unlink(const char* filename);
	
private:

	typedef std::map< std::string, std::vector<unsigned char>* > fs_t;
	fs_t fs_data;
	
	class InMemoryReadHandle : public Handle
	{
	public:
		InMemoryReadHandle(FS* parent, const fs_t::iterator& file );
		~InMemoryReadHandle();
		size_t fread(void* ptr, size_t size, size_t count);
		size_t fwrite(const void* ptr, size_t size, size_t count);
		int fclose();
		int fgetc();
		char* fgets(char* buff, size_t len);
		int fseek(int offset, int mode);
		int ftell();
	protected:
		const unsigned char* begin;
		const unsigned char* end;
		const unsigned char* pos;
	};
	
	class InMemoryWriteHandle : public Handle
	{
	public:
		InMemoryWriteHandle(FS* parent, const char* path);
		~InMemoryWriteHandle();
		size_t fread(void* ptr, size_t size, size_t count);
		size_t fwrite(const void* ptr, size_t size, size_t count);
		int fclose();
		int fgetc();
		char* fgets(char* buff, size_t len);
		int fseek(int offset, int mode);
		int ftell();
	protected:
		void apply();
		std::vector<unsigned char>* pending;
		std::vector<unsigned char>::iterator pos;
	};

};

}

#endif