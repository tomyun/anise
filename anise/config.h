#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <string.h>
#include "memory_block.h"

#define FILE_NAME_LENGTH	32

enum GameType {
	GAME_NANPA1,
	GAME_NANPA2,
	GAME_ISAKU,
	GAME_KAKYUSEI,
	GAME_AISI,
	GAME_KAWA,
	GAME_NONO,
	GAME_ISLE,
	GAME_JACK,
	GAME_UNKNOWN
};


enum FontType {
	FONT_JIS,
	FONT_JISHAN,
	FONT_GAMEBOX
};


class Config {
public:
	GameType game_type;
	FontType font_type;

	char script_file_name[FILE_NAME_LENGTH];
	char font_file_name[FILE_NAME_LENGTH];

	word variable_size;
	word selection_item_entry;
	word procedure_entry;
	word animation_slot_entry;
	word animation_script_entry;

public:
	Config();
	~Config();

	bool initialize(int argc, char *argv[]);
};

#endif CONFIG_H