#ifndef FILE_H
#define FILE_H

#include "anise.h"
#include "memory.h"
#include "option.h"

using std::string;

#define FILE_READ			"rb"
#define FILE_WRITE			"wb"
#define FILE_READ_WRITE			"r+b"

#define FILE_NAME_LENGTH		32

#define FILE_PACKED_SLOTS		2
#define FILE_PACKED_DAT_TABLE_SIZE	291	// 6984 / 24
#define FILE_PACKED_DAT_KEY		0xAA

#define HMS_FILE_EXTENSION		".hms"
#define MES_FILE_EXTENSION		".mes"
#define MES_FILE_EXTENSION_LENGTH	4

#define M_FILE_EXTENSION		".m"
#define M_FILE_EXTENSION_LENGTH		2

#define WAV_FILE_EXTENSION		".wav"

#define SAGWA_DAT_SLOTS 3

class File {
private:
	Memory *memory;
	Option *option;

	FILE *handle;
	string name;
	word size;

	bool is_hms;

	int packed_slot_index;
	FILE *packed_handle[FILE_PACKED_SLOTS];
	int packed_entry_offset[FILE_PACKED_SLOTS];
	int packed_table_size[FILE_PACKED_SLOTS];

	//TODO: make it dynamic so that it can support various formats
	struct DATFile {
		char filename[16];
		int offset;
		int size;
	} packed_dat_table[FILE_PACKED_SLOTS][FILE_PACKED_DAT_TABLE_SIZE];

	bool is_sagwa_dat;
	bool is_sagwa_dat_load;
	int sagwa_dat_slot_index;
	FILE *sagwa_dat_handle[SAGWA_DAT_SLOTS];
	int sagwa_dat_size[SAGWA_DAT_SLOTS];
	DATFile *sagwa_dat_table[SAGWA_DAT_SLOTS];

	void openDirect(const char *filename, const char *mode);
	void openFromDAT(const char *filename);
	void openFromSagwaDAT(const char *filename);
	void initializeSagwaDAT(void);

	string concatenatePath(const char *filename);

public:
	File(Memory *memory, Option *option);
	~File();

	void open(const char *filename, bool is_flag = false);
	void close();
	word tell();
	void seek(word offset, int mode);
	bool load(MemoryBlock *memory_block, word offset, bool is_flag = false);
	bool store(MemoryBlock *memory_block);

	void initializeDAT(int slot_index, int packed_index);

	word getSize(){return size;};
};

#endif
