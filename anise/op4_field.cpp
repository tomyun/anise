#include "script.h"

enum FieldFunction {
	FIELD_LoadMAPFile = 0,
	FIELD_LoadC5File = 1,
	FIELD_Initialize = 2,
	FIELD_SetCharactersOnMap = 3,
	FIELD_QuickDraw = 4,
	FIELD_Draw = 5,
	FIELD_Redraw = 6,
	FIELD_OperateCharacters = 7,
	FIELD_CheckEntrance = 8,
	FIELD_CheckDetectCollision = 9,
	FIELD_SetEntryInfo = 10,
	FIELD_ResetEntryInfo = 11,
	FIELD_CheckClick = 12,
	FIELD_SetCharacterOperationOffset = 13,
	FIELD_ActivateCharacter = 14,
	FIELD_UnpackMPFile = 15,
	FIELD_Function16 = 16,
	FIELD_MakeSetPath = 30,
	FIELD_ClearPathFoundStatus = 32
};

SCRIPTCALL Script::op4_field()
{
	parameter = getParameter();

	field->initialize();

	word function = parameter->get(0);
	word selection = function;
	switch (function) {
		case FIELD_LoadMAPFile:
			PRINT("[Script::op4_field()] LoadMAPFile\n");
			selection = field->loadMapFile();
			break;

		case FIELD_LoadC5File:
			PRINT("[Script::op4_field()] LoadC5File\n");
			field->loadC5File();
			break;

		case FIELD_Initialize:
			PRINT("[Script::op4_field()] InitializeMap\n");
			field->initializeMap();
			break;

		case FIELD_SetCharactersOnMap:
			PRINT("[Script::op4_field()] SetCharactersOnMap\n");
			field->setCharactersOnMap();
			break;

		case FIELD_QuickDraw:
			PRINT("[Script::op4_field()] QuickDraw\n");
			animation->clearSlot();
			field->quickDraw();
			break;

		case FIELD_Draw:
			PRINT("[Script::op4_field()] Draw\n");
			animation->clearSlot();
			field->draw();
			break;

		case FIELD_Redraw:
			field->initializeMap();
			field->setCharactersOnMap();
			field->quickDraw();
			break;

		case FIELD_OperateCharacters:
			PRINT("[Script::op4_field()] OperateCharacters\n");
			field->operateCharacters();
			break;

		case FIELD_CheckEntrance:
			PRINT("[Script::op4_field()] CheckEntrance\n");
			selection = field->checkEntrance(parameter->get(1));
			break;

		case FIELD_CheckDetectCollision:
			PRINT("[Script::op4_field()] CheckDetectCollision\n");
			selection = field->checkDetectCollision(parameter->get(1));
			break;

		case FIELD_SetEntryInfo:
			PRINT("[Script::op4_field()] SetEntryInfo\n");
			field->setEntryInfo(parameter->get(1));
			break;

		case FIELD_ResetEntryInfo:
			PRINT("[Script::op4_field()] ResetEntryInfo\n");
			field->resetEntryInfo(parameter->get(1));
			break;

		case FIELD_CheckClick:
			PRINT("[Script::op4_field()] CheckClick\n");
			selection = field->checkClick();
			break;

		case FIELD_SetCharacterOperationOffset:
			PRINT("[Script::op4_field()] SetCharacterOperationOffset\n");
			field->setCharacterOperationOffset(parameter->get(1), parameter->get(2));
			break;

		case FIELD_ActivateCharacter:
			PRINT("[Script::op4_field()] ActivateCharacter\n");
			field->activateCharacter(parameter->get(1), parameter->get(2));
			break;

		case FIELD_UnpackMPFile:
			PRINT("[Script::op4_field()] Function15\n");
			field->unpackMPFile(parameter->get(1));
			break;

		case FIELD_Function16:
			PRINT("[Script::op4_field()] Function16\n");
			field->function16();
			break;

		case FIELD_MakeSetPath:
			PRINT("[Script::op4_field()] MakeSetPath\n");
			selection = field->makeSetPath(parameter->get(1));
			break;

		case FIELD_ClearPathFoundStatus:
			PRINT("[Script::op4_field()] ClearPathFoundStatus\n");
			field->clearPathFoundStatus();
			break;
	}

	memory->b_BaseVariable->writeWord(iw_Selection, selection);

	deleteParameter();

	return RETURN_NORMAL;
}