#include "file.h"

File::File(Memory *memory, Option *option)
{
	this->memory = memory;
	this->option = option;

	handle = NULL;
	size = 0;
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
		PRINT_ERROR("[File::open()] %s is already opened\n", filename);
		close();
	}

	// concatenate path name with file name
	name = option->path_name;
	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		if (filename[i] != '\0') {
			name += tolower(filename[i]);
		}
		else {
			break;
		}
	}

	handle = fopen(name.c_str(), mode);
	if (handle == NULL) {
		//TODO: process error
		PRINT_ERROR("[File::open()] fopen() failed: %s\n", name.c_str());
		exit(1);
	}

	seek(0, SEEK_END);
	size = tell();
	seek(0, SEEK_SET);

	memory->b_SystemVariable->writeWord(iw_File_Size, size);

	//HACK: script files in crescent have .m extension
	if (option->game_type != GAME_CRESCENT) {
		// check if it is a music file
		string extension = name.substr(name.size() - M_FILE_EXTENSION_LENGTH);
		if (extension == M_FILE_EXTENSION) {
			string wav_file_name = name.substr(0, name.size() - M_FILE_EXTENSION_LENGTH);
			wav_file_name.append(WAV_FILE_EXTENSION);

			FILE *wav_file_handle = fopen(wav_file_name.c_str(), FILE_READ);
			if (wav_file_handle) {
				option->sound_file_name = wav_file_name;

				fclose(wav_file_handle);
			}
			else {
				option->sound_file_name = name;
			}
		}
	}
}


void File::close()
{
	//PRINT("[File::close()]\n");

	if (handle != NULL) {
		fclose(handle);
		handle = NULL;
	}
}


word File::tell()
{
	int file_size = ftell(handle);
	if (file_size > 0xFFFF) {
		return 0;
	}
	else {
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

	//PRINT("[File::load()]\n");

	byte *raw = memory_block->getRaw();

	word read_length = (word) (fread((byte*) (raw + offset), sizeof(byte), size, handle));
	if (read_length == size) {
		return true;
	}
	else {
		//TODO: process error
		PRINT_ERROR("[File::open()] fread() failed: %s (read %d of %d)\n", name.c_str(), read_length, size);
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
		PRINT_ERROR("[File::open()] fwrite() failed: %s (written %d of %d)\n", name.c_str(), write_length, size);
		return false;
	}
}
