#include "option.h"

Option::Option()
{
	static const char title[] = TITLE TITLE_EXTRA;
	static const char usage[] =
		"\n"
		"Usage: anise [OPTIONS] ... [GAME]\n"
		"\n"
		"Options:\n"
		"  -p		Path to the game\n"
		"  -l		Select language (j: Japanese, k: *Korean, K: Korean (gamebox))\n"
		"\n"
		"Supported Games:\n"
		"  nanpa2	Dokyusei 2 (including special version)\n"
		"  nanpa1	Dokyusei 1\n"
		"  aisi		Aisi\n"
		"  crescent	Crescent\n"
		"\n"
		"Example:\n"
		"  anise -pC:\\NANPA2 -lK nanpa2\n";

	this->title = title;
	this->usage = usage;

	game_type = GAME_UNKNOWN;
	font_type = FONT_JISHAN;

	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		script_file_name[i] = NULL;
		font_file_name[i] = NULL;
	}

	//TODO: move this to proper position
	sprintf(font_file_name, "jishan.fnt");

	variable_size = NULL;
	selection_item_entry = NULL;
	procedure_entry = NULL;
	animation_slot_entry = NULL;
	animation_script_entry = NULL;
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
							if (path_name.at(path_name.length() - 1) != '\\') {
								path_name += '\\';
							}
						}
						break;

					case 'l':
						{
							//TODO: need clean up
							switch (option[2]) {
								case 'j':
									{
										font_type = FONT_JIS;
										sprintf(font_file_name, "jis.fnt");
									}
									break;

								case 'k':
									{
										font_type = FONT_JISHAN;
										sprintf(font_file_name, "jishan.fnt");
									}
									break;

								case 'K':
									{
										font_type = FONT_GAMEBOX;
										sprintf(font_file_name, "jishan.fnt");
									}
									break;
							}
						}
						break;
				}
			}
			else if (strcmp(option, "nanpa2") == 0) {
				game_type = GAME_NANPA2;

				sprintf(script_file_name, "start.mes");

				variable_size = 1024;
				selection_item_entry = 0xED7C;
				procedure_entry = 0xEDA8;
				animation_slot_entry = 0xF102;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "nanpa1") == 0) {
				game_type = GAME_NANPA1;

				sprintf(script_file_name, "start.mes");

				variable_size = 512;
				selection_item_entry = 0xED76;
				procedure_entry = 0xEDA2;
				animation_slot_entry = 0xF682;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "aisi") == 0) {
				game_type = GAME_AISI;

				sprintf(script_file_name, "main.mes");

				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
			else if (strcmp(option, "crescent") == 0) {
				game_type = GAME_CRESCENT;

				sprintf(script_file_name, "start.m");

				//HACK: same as aisi
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