#include "config.h"

Config::Config()
{
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


Config::~Config()
{
}


bool Config::initialize(int argc, char *argv[])
{
	if (argc < 2) {
		//TODO: display usage screen
		return false;
	}
	else {
		for (int i = argc - 1; i >= 0; i--) {
			char *option = argv[i];

			if (_stricmp(option, "nanpa2") == 0) {
				game_type = GAME_NANPA2;

				sprintf(script_file_name, "start.mes");

				variable_size = 1024;
				selection_item_entry = 0xED7C;
				procedure_entry = 0xEDA8;
				animation_slot_entry = 0xF102;
				animation_script_entry = 0x8000;
			}
			else if (_stricmp(option, "nanpa1") == 0) {
				game_type = GAME_NANPA1;

				sprintf(script_file_name, "start.mes");

				variable_size = 512;
				selection_item_entry = 0xED76;
				procedure_entry = 0xEDA2;
				animation_slot_entry = 0xF682;
				animation_script_entry = 0x8000;
			}
			else if (_stricmp(option, "aisi") == 0) {
				game_type = GAME_AISI;

				sprintf(script_file_name, "main.mes");

				variable_size = 512;
				selection_item_entry = 0xED78;
				procedure_entry = 0xEDA4;
				animation_slot_entry = 0xF302;
				animation_script_entry = 0x8000;
			}
		}

		return true;
	}
}