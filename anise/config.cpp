#include "config.h"

Config::Config()
{
	static const char title[] = "ANISE beta 3";
	static const char usage[] =
		" - A Newly Implemented Scripting Engine for ai5v\n"
		"Usage: anise [GAME] [DIRECTORY]\n";

	this->title = title;
	this->usage = usage;

	game_type = GAME_UNKNOWN;
	font_type = FONT_JISHAN;

	for (int i = 0; i < FILE_NAME_LENGTH; i++) {
		script_file_name[i] = NULL;
		font_file_name[i] = NULL;
	}

	//TODO: move this to proper position (make '~h' options work)
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
		printf(title);
		printf(usage);

		return false;
	}
	else {
		//TODO: need clean up
		//for (int i = argc - 1; i >= 0; i--) {
		for (int i = 1; i < argc; i++) {
			char *option = argv[i];

			if ((strcmp(option, "nanpa2") == 0) || (strcmp(option, "nanpa2h") == 0) || (strcmp(option, "nanpa2gb") == 0)) {
				game_type = GAME_NANPA2;

				sprintf(script_file_name, "start.mes");

				//TODO: make them exclusive
				if ((strcmp(option, "nanpa2h") == 0)) {
					sprintf(font_file_name, "jishan.fnt");
				}
				else if ((strcmp(option, "nanpa2gb") == 0)) {
					sprintf(font_file_name, "jishan.fnt");
				}

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
		}

		if (game_type == GAME_UNKNOWN) {
			//TODO: process error
			printf(title);
			printf(usage);

			return false;
		}
		else {
			return true;
		}
	}
}