#include "file.h"

File::File(Memory *memory, Option *option)
{
	this->memory = memory;
	this->option = option;

	handle = NULL;
	size = 0;

	is_hms = false;

	packed_slot_index = 0;

	for (int i = 0; i < FILE_PACKED_SLOTS; i++) {
		packed_handle[i] = NULL;
		packed_entry_offset[i] = 0;
		packed_table_size[i] = 0;
	}

	if (!option->is_unpacked) {
		initializeDAT(0, 0);
		initializeDAT(1, 1);
	}

	is_sagwa_dat_load = false;
	if (option->font_type == FONT_SAGWA) {
		initializeSagwaDAT();
	}
}


File::~File()
{
	close();
	int i;

	for (i = 0; i < FILE_PACKED_SLOTS; i++) {
		if (packed_handle[i] != NULL) {
			fclose(packed_handle[i]);
		}
	}

	if(is_sagwa_dat_load){
		for (i=0; i < SAGWA_DAT_SLOTS; i++) {
			if (sagwa_dat_handle[i]) {
				delete sagwa_dat_table[i];
				fclose(sagwa_dat_handle[i]);
			}
		}
	}
}


void File::open(const char *filename, bool is_flag)
{
	bool is_mes = false;
	is_sagwa_dat = false;

	if (is_flag) {
		openDirect(filename, FILE_READ_WRITE);
	}
	else {
		//HACK: support .hms files
		string hms_file_name;
		if ((option->font_type == FONT_JISHAN) || (option->font_type == FONT_SAGWA)){
			string file_name = concatenatePath(filename);
			string file_extension = file_name.substr(file_name.size() - MES_FILE_EXTENSION_LENGTH);
			if (file_extension == MES_FILE_EXTENSION) {
				is_mes = true;

				if(option->font_type == FONT_JISHAN){
					hms_file_name = file_name.substr(option->path_name.size(), (file_name.size() - option->path_name.size()) - MES_FILE_EXTENSION_LENGTH);
					hms_file_name.append(HMS_FILE_EXTENSION);

					string hms_file_name_with_path = option->path_name + hms_file_name;

					FILE *hms_file_handle = fopen(hms_file_name_with_path.c_str(), FILE_READ);
					if (hms_file_handle) {
						is_hms = true;
						fclose(hms_file_handle);
					}
				}
			}
		}

		//HACK: support .wav files
		//HACK: script files in crescent have .m extension
		if (option->game_type != GAME_CRESCENT) {
			string file_name = concatenatePath(filename);
			string file_extension = file_name.substr(file_name.size() - M_FILE_EXTENSION_LENGTH);
			if (file_extension == M_FILE_EXTENSION) {
				option->sound_file_name = file_name.substr(0, file_name.size() - M_FILE_EXTENSION_LENGTH);

				string wav_file_name = option->sound_file_name;
				wav_file_name.append(WAV_FILE_EXTENSION);

				FILE *wav_file_handle = fopen(wav_file_name.c_str(), FILE_READ);
				if (wav_file_handle) {
					option->sound_file_extension = WAV_FILE_EXTENSION;
					fclose(wav_file_handle);
				}
				else {
					option->sound_file_extension = M_FILE_EXTENSION;
				}
			}
		}

		if (is_hms) {
			openDirect(hms_file_name.c_str(), FILE_READ);
		}
		else if (option->is_unpacked) {
			openDirect(filename, FILE_READ);
		}
		else {
			if((is_mes)&&(is_sagwa_dat_load))
				openFromSagwaDAT(filename);
			if(!is_sagwa_dat)
				openFromDAT(filename);
		}
	}
}


void File::openDirect(const char *filename, const char *mode)
{
	PRINT("[File::openDirect()] %s\n", filename);

	if (handle != NULL) {
		PRINT_ERROR("[File::open()] %s is already opened\n", filename);
		close();
	}

	// concatenate path name with file name
	name = concatenatePath(filename);

	handle = fopen(name.c_str(), mode);
	if (handle == NULL) {
		PRINT_ERROR("[File::openDirect()] fopen() failed: %s\n", name.c_str());
		exit(1);
	}

	seek(0, SEEK_END);
	size = tell();
	seek(0, SEEK_SET);

	memory->b_SystemVariable->writeWord(iw_File_Size, size);
}


void File::initializeDAT(int slot_index, int packed_index)
{
	if (packed_handle[slot_index] != NULL) {
		fclose(packed_handle[slot_index]);
	}

	// concatenate path name with file name
	char packed_index_c_str[2];
	sprintf(packed_index_c_str, "%1d", packed_index);
	string filename = option->packed_file_name + packed_index_c_str + option->packed_file_extension;
	string packed_name = concatenatePath(filename.c_str());

	packed_handle[slot_index] = fopen(packed_name.c_str(), FILE_READ);
	packed_entry_offset[slot_index] = 0;

	fseek(packed_handle[slot_index], 500, SEEK_SET);

	int table_size;
	fread(&table_size, sizeof(int), 1, packed_handle[slot_index]);
	packed_table_size[slot_index] = table_size / sizeof(DATFile);

	/*
	byte packed_table[FILE_PACKED_TABLE_SIZE];
	//TODO: is it necessary?
	//for (int i = 0; i < (table_size + 16); i++) {
	for (int i = 0; i < table_size; i++) {
		packed_table[slot_index][i] = 0;
	}
	*/

	fread(packed_dat_table[slot_index], sizeof(DATFile), packed_table_size[slot_index], packed_handle[slot_index]);

	byte *packed_table = (byte*) &packed_dat_table[slot_index];
	for (int i = (table_size - 1); i > 0; i--) {
		byte data = packed_table[i - 1];
		packed_table[i] ^= data;
	}
	packed_table[0] ^= FILE_PACKED_DAT_KEY;

	//TODO: isn't it possible?
	//packed_entry_offset[slot_index] = ftell(packed_handle[slot_index]);
	packed_entry_offset[slot_index] = table_size + 500 + 4;
}

const char *sagwa_dat_filename[3] = {"adult.dat", "patch.dat", "sagwa.dat"};

void File::initializeSagwaDAT(void)
{
	int slot_index, table_index, i;
	int seek, offset;
	char len;

	for(slot_index=0;slot_index<SAGWA_DAT_SLOTS;slot_index++){
		string packed_name = concatenatePath(sagwa_dat_filename[slot_index]);
		sagwa_dat_handle[slot_index] = fopen(packed_name.c_str(), FILE_READ);
		if(sagwa_dat_handle[slot_index]){
			fseek(sagwa_dat_handle[slot_index], -4, SEEK_END);
			fread(&sagwa_dat_size[slot_index], 1, 4, sagwa_dat_handle[slot_index]);
			seek = -((sagwa_dat_size[slot_index] & 0xFFFF) ^ 0xAAAA);
			sagwa_dat_size[slot_index] = ((sagwa_dat_size[slot_index] >> 16) & 0xFFFF) ^ 0xAAAA;
			sagwa_dat_table[slot_index] = new DATFile[sagwa_dat_size[slot_index]];

			offset = 0;
			fseek(sagwa_dat_handle[slot_index], seek, SEEK_END);
			for(table_index=0;table_index<sagwa_dat_size[slot_index];table_index++){
				fread(&len, 1, 1, sagwa_dat_handle[slot_index]);
				len ^= 0xAA;
				fread(sagwa_dat_table[slot_index][table_index].filename, 1, len, sagwa_dat_handle[slot_index]);
				fread(&sagwa_dat_table[slot_index][table_index].size, 1, 2, sagwa_dat_handle[slot_index]);
				for(i=0; i<len; i++)
					sagwa_dat_table[slot_index][table_index].filename[i] = toupper((char)(sagwa_dat_table[slot_index][table_index].filename[i] ^ 0xAA));
				sagwa_dat_table[slot_index][table_index].filename[len] = 0;
				sagwa_dat_table[slot_index][table_index].size = (sagwa_dat_table[slot_index][table_index].size & 0xFFFF) ^ 0xAAAA;
				sagwa_dat_table[slot_index][table_index].offset = offset;
				offset += sagwa_dat_table[slot_index][table_index].size;
			}

			is_sagwa_dat_load = true;
		}
		else {
			sagwa_dat_size[slot_index] = 0;
			sagwa_dat_table[slot_index] = NULL;
		}
	}
}

void File::openFromDAT(const char *filename)
{
	char uppercase_filename[FILE_NAME_LENGTH];
	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		if (filename[i] != '\0') {
			uppercase_filename[i] = toupper(filename[i]);
		}
		else {
			uppercase_filename[i] = '\0';
		}
	}
	
	bool is_found = false;
	int slot_index;
	int table_index;
	for (slot_index = 0; slot_index < FILE_PACKED_SLOTS; slot_index++) {
		if (packed_handle[slot_index] == NULL) {
			continue;
		}

		for (table_index = 0; table_index < packed_table_size[slot_index]; table_index++) {
			if (strcmp(uppercase_filename, packed_dat_table[slot_index][table_index].filename) == 0) {
				is_found = true;
				break;
			}
		}

		if (is_found) {
			break;
		}
	}

	if (is_found) {
		packed_slot_index = slot_index;

		size = packed_dat_table[slot_index][table_index].size;
		memory->b_SystemVariable->writeWord(iw_File_Size, size);

		int offset = packed_entry_offset[slot_index] + packed_dat_table[slot_index][table_index].offset;
		fseek(packed_handle[slot_index], offset, SEEK_SET);
	}
	else {
		PRINT_ERROR("[File::openFromDAT()] cannot find file: %s\n", filename);
	}
}

void File::openFromSagwaDAT(const char *filename)
{
	char uppercase_filename[FILE_NAME_LENGTH];
	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		if ((filename[i] != '\0') && (filename[i] != '.')) {
			uppercase_filename[i] = toupper(filename[i]);
		}
		else {
			uppercase_filename[i] = '\0';
			break;
		}
	}
	
	bool is_found = false;
	int slot_index;
	int table_index;
	for (slot_index = 0; slot_index < SAGWA_DAT_SLOTS; slot_index++) {
		if (sagwa_dat_handle[slot_index] == NULL) {
			continue;
		}

		for (table_index = 0; table_index < sagwa_dat_size[slot_index]; table_index++) {
			if (strcmp(uppercase_filename, sagwa_dat_table[slot_index][table_index].filename) == 0) {
				is_found = true;
				break;
			}
		}

		if (is_found) {
			break;
		}
	}

	if (is_found) {
		is_sagwa_dat = true;
		sagwa_dat_slot_index = slot_index;

		size = sagwa_dat_table[slot_index][table_index].size;
		memory->b_SystemVariable->writeWord(iw_File_Size, size);

		int offset = sagwa_dat_table[slot_index][table_index].offset;
		fseek(sagwa_dat_handle[slot_index], offset, SEEK_SET);
	}
	else {
		PRINT("[File::openFromSagwaDAT()] cannot find file: %s\n", filename);
	}
}

string File::concatenatePath(const char *filename)
{
	string concatenated_name = option->path_name;
	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		if (filename[i] != '\0') {
			concatenated_name += tolower(filename[i]);
		}
		else {
			break;
		}
	}

	return concatenated_name;
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


bool File::load(MemoryBlock *memory_block, word offset, bool is_flag)
{
	PRINT("[File::load()]\n");

	if (offset + size > memory_block->getSize()) {
		PRINT_ERROR("[File::load()] out of bound: offset = %d, size = %d, memory_block->size = %d\n", offset, size, memory_block->getSize());
		return false;
	}

	byte *raw = memory_block->getRaw();

	FILE *file_handle;
	if (is_hms) {
		is_hms = false;
		file_handle = handle;
	}
	else if (is_sagwa_dat) {
		file_handle = sagwa_dat_handle[sagwa_dat_slot_index];
	}
	else if (is_flag || option->is_unpacked) {
		file_handle = handle;
	}
	else {
		file_handle = packed_handle[packed_slot_index];	
	}

	word read_length = (word) (fread((byte*) (raw + offset), sizeof(byte), size, file_handle));
	if (read_length == size) {
		if(is_sagwa_dat){
			byte mask = (read_length & 0xFF) ^ (read_length >> 8);
			byte *read = (byte*)(raw + offset);
			byte next;
			for(int i=0;i<size;i++){
				next = *read;
				*read++ = ~(next ^ mask);
				mask = next;
			}
		}
		return true;
	}
	else {
		PRINT_ERROR("[File::load()] fread() failed: %s (read %d of %d)\n", name.c_str(), read_length, size);
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
		PRINT_ERROR("[File::store()] fwrite() failed: %s (written %d of %d)\n", name.c_str(), write_length, size);
		return false;
	}
}
