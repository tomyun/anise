#include "option.h"

Option::Option()
{
	static const char title[] = TITLE TITLE_EXTRA;
	static const char usage[] =
		"\n"
		"Usage: anise [OPTIONS] ... [GAME]\n"
		"\n"
		"Options:\n"
		"  -p\t\tPath to the game\n"
		"  -l[j,k,K]\tLanguage (j: Japanese, *k: Korean, K: Korean (gamebox))\n"
		"  -u\t\tUse unpacked game files\n"
		"  -f\t\tFullscreen mode\n"
		"  -b[s]\t\tBlurring filter mode (s: scanline)\n"
		"\n"
		"Supported Games:\n"
		"  nanpa2\tDokyusei 2 (including special version)\n"
		"  nanpa1\tDokyusei 1\n"
		"  aisimai\tAisimai\n"
		"  crescent\tCrescent\n"
		"\n"
		"Example:\n"
		"  anise -pC:\\NANPA2 -lK nanpa2\n"
		"  anise -pC:\\NANPA1 -lj -bs nanpa1\n";

	this->title = title;
	this->usage = usage;

	game_type = GAME_UNKNOWN;
	font_type = FONT_JISHAN;

	is_unpacked = false;

	is_fullscreen = false;
	is_filter = false;
	is_scanline = false;

	variable_size = 0;
	selection_item_entry = 0;
	procedure_entry = 0;
	animation_slot_entry = 0;
	animation_script_entry = 0;
}


Option::~Option()
{
}


bool Option::initialize(int argc, char *argv[])
{
	printf(title);

	if (argc < 2) {
		printf(usage);

		return false;
	}
	else {
		//TODO: need clean up
		//for (int i = argc - 1; i >= 0; i--) {
		for (int i = 1; i < argc; i++) {
			char *option = argv[i];

			if (option[0] == '-') {
				switch (option[1]) {
					case 'p':
						{
							path_name = option + 2;

							//HACK: should be platform independent
#ifdef _WIN32
							if (path_name.at(path_name.length() - 1) != '\\') {
								path_name += '\\';
							}
#else
							if (path_name.at(path_name.length() - 1) != '/') {
								path_name += '/';
							}
#endif
						}
						break;

					case 'l':
						{
							switch (option[2]) {
								case 'j':
									font_type = FONT_JIS;
									break;

								case 'k':
									font_type = FONT_JISHAN;
									break;

								case 'K':
									font_type = FONT_GAMEBOX;
									break;
							}
						}
						break;

					case 'u':
						{
							is_unpacked = true;
						}
						break;

					case 'f':
						{
							is_fullscreen = true;
						}
						break;

					case 'b':
						{
							is_filter = true;
							if ((option[2]) == 's') {
								is_scanline = true;
							}
						}
						break;
				}
			}
			else if (strcmp(option, "nanpa2") == 0) {
				game_type = GAME_NANPA2;

				is_unpacked = true;

				script_file_name = "start.mes";

				variable_size = 1024;
				selection_item_entry = 0xED7C;
				procedure_entry = 0xEDA8;
				animation_slot_entry = 0xF102;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "nanpa1") == 0) {
				game_type = GAME_NANPA1;

				packed_file_name = "elf";
				packed_file_extension = ".dat";

				script_file_name = "start.mes";

				variable_size = 512;
				selection_item_entry = 0xED76;
				procedure_entry = 0xEDA2;
				animation_slot_entry = 0xF682;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "aisimai") == 0) {
				game_type = GAME_AISIMAI;

				//HACK: it's probably a packed game
				is_unpacked = true;

				script_file_name = "main.mes";

				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "crescent") == 0) {
				game_type = GAME_CRESCENT;

				is_unpacked = true;

				script_file_name = "start.m";

				//HACK: same as aisimai
				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "isle") == 0) {
				game_type = GAME_ISLE;

				is_unpacked = true;

				script_file_name = "main.mes";

				//HACK: same as aisimai
				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "kawa") == 0) {
				game_type = GAME_KAWA;

				packed_file_name = "silk";
				packed_file_extension = ".dat";

				script_file_name = "main.mes";

				//HACK: same as aisimai
				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "nono") == 0) {
				game_type = GAME_NONO;

				packed_file_name = "silk";
				packed_file_extension = ".dat";

				script_file_name = "main.mes";

				//HACK: same as aisimai
				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
		}

		if (game_type == GAME_UNKNOWN) {
			//TODO: process error
			printf(usage);

			return false;
		}
		else {
			return true;
		}
	}
}
