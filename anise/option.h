#ifndef OPTION_H
#define OPTION_H

#include "anise.h"

using std::string;

#define TITLE			"ANISE beta 8"
#define TITLE_EXTRA		" - A Newly Implemented Scripting Engine for AI5V\n"
#define FILE_NAME_LENGTH	32

enum GameType {
	GAME_NANPA1,
	GAME_NANPA2,
	GAME_ISAKU,
	GAME_KAKYUSEI,
	GAME_AISIMAI,
	GAME_KAWA,
	GAME_NONO,
	GAME_ISLE,
	GAME_CRESCENT,
	GAME_JACK,
	GAME_UNKNOWN
};


enum FontType {
	FONT_JIS,
	FONT_JISHAN,
	FONT_GAMEBOX
};


class Option {
public:
	const char *title;
	const char *usage;

	GameType game_type;
	FontType font_type;

	bool is_unpacked;

	bool is_fullscreen;
	bool is_filter;
	bool is_scanline;

	string path_name;
	string packed_file_name;
	string packed_file_extension;
	string script_file_name;
	string sound_file_name;

	word variable_size;
	word selection_item_entry;
	word procedure_entry;
	word animation_slot_entry;
	word animation_script_entry;

public:
	Option();
	~Option();

	bool initialize(int argc, char *argv[]);
};

#endif
