#include "file.h"

File::File(Memory *memory)
{
	this->memory = memory;

	handle = NULL;
	is_huge = false;
}


File::~File()
{
	if (handle != NULL) {
		close();
	}
}


void File::open(const char *filename, const char *mode)
{
	PRINT("[File::open()] %s\n", filename);

	if (handle != NULL) {
		//TODO: process error
		PRINT("[File::open()] file is already opened\n");
		close();
	}

	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		name[i] = tolower(filename[i]);
	}

	handle = fopen(name, mode);
	if (handle == NULL) {
		//TODO: process error
		PRINT("[File::open()] fopen() failed\n");
		exit(1);
	}

	//TODO: put filename into name

	seek(0, SEEK_END);
	size = tell();
	seek(0, SEEK_SET);

	memory->b_SystemVariable->writeWord(iw_File_Size, size);
}


void File::close()
{
	PRINT("[File::close()]\n");

	if (handle != NULL) {
		fclose(handle);
		handle = NULL;
	}
}


word File::tell()
{
	int file_size = ftell(handle);
	if (file_size > 0xFFFF) {
		is_huge = true;

		return 0;
	}
	else {
		is_huge = false;

		return (word) file_size;
	}
}


void File::seek(word offset, int mode)
{
	fseek(handle, offset, mode);
}


bool File::load(MemoryBlock *memory_block, word offset)
{
	//assert(offset < memory_block->getSize());

	PRINT("[File::load()]\n");

	byte *raw = memory_block->getRaw();

	word read_length = (word) (fread((byte*) (raw + offset), sizeof(byte), size, handle));
	if (read_length == size) {
		return true;
	}
	else {
		//TODO: process error
		PRINT("[File::open()] fread() failed (read %d of %d)\n", read_length, size);
		return false;
	}
}


bool File::store(MemoryBlock *memory_block)
{
	byte *raw = memory_block->getRaw();
	word block_size = memory_block->getSize();

	word write_length = (word) (fwrite((byte*) raw, sizeof(byte), block_size, handle));
	if (write_length == block_size) {
		return true;
	}
	else {
		//TODO: process error
		PRINT("[File::open()] fwrite() failed (written %d of %d)\n", write_length, size);
		return false;
	}
}