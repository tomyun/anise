#ifndef FILE_H
#define FILE_H

#include "anise.h"
#include "memory.h"
#include "option.h"

using std::string;

#define FILE_READ		"rb"
#define FILE_WRITE		"wb"
#define FILE_NAME_LENGTH	32

#define M_FILE_EXTENSION	".m"
#define M_FILE_EXTENSION_LENGTH	2

#define WAV_FILE_EXTENSION	".wav"

class File {
private:
	Memory *memory;
	Option *option;

	FILE *handle;
	string name;
	word size;

public:
	File(Memory *memory, Option *option);
	~File();

	void open(const char *filename, const char *mode);
	void close();
	word tell();
	void seek(word offset, int mode);
	bool load(MemoryBlock *memory_block, word offset);
	bool store(MemoryBlock *memory_block);
};

#endif
