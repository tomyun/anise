#include "script.h"

enum SelectionInUse {
	INUSE_FALSE = 0,
	INUSE_TRUE = 1
};


enum SelectionStatus {
	STATUS_RELEASED = 0x01,
	STATUS_OK = 0x02,
	STATUS_CANCEL = 0x04,
	STATUS_UP = 0x08,
	STATUS_DOWN = 0x10,
	STATUS_LEFT = 0x20,
	STATUS_RIGHT = 0x40
};


SCRIPTCALL Script::op4_displaySelection()
{
	parameter = getParameter();
	if (parameter->getType(0) == PARAMETER_TYPE_BLOCK) {
		memory->b_Procedure->writeWord(iwpo_Selection_List, parameter->get(0));
	}
	else {
		word procedure_index = parameter->get(0) * 2;
		word procedure_offset = memory->b_Procedure->queryWord(procedure_index);
		memory->b_Procedure->writeWord(iwpo_Selection_List, procedure_offset);
	}
	deleteParameter();

	memory->b_SystemVariable->writeWord(iwf_Selection_InUse, INUSE_TRUE);

	word script_offset = getOffset();

	setOffset(memory->b_Procedure->queryWord(iwpo_Selection_ShowMenu));
	parseNested();

	while (true) {
		//TODO: debugmode
		animation->show();
		if (input->refresh() == false) {
			break;
		}

		if (memory->b_SystemVariable->queryWord(iwf_Selection_InUse) == INUSE_FALSE) {
			setOffset(script_offset);
			break;
		}

		setOffset(memory->b_Procedure->queryWord(iwpo_Selection_CheckPosition));
		parseNested();

		if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_OK) && input->check(INPUT_OK)) {
			setOffset(memory->b_Procedure->queryWord(iwpo_Selection_OK));
			parseNested();

			if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RELEASED) == false) {
				while (input->check(INPUT_OK)) {
					//TODO: debugmode
					animation->show();
					input->refresh();
				}
			}
		}
		else if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_CANCEL) && input->check(INPUT_CANCEL)) {
			setOffset(memory->b_Procedure->queryWord(iwpo_Selection_Cancel));
			parseNested();

			if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RELEASED) == false) {
				while (input->check(INPUT_CANCEL)) {
					//TODO: debugmode
					animation->show();
					input->refresh();
				}
			}
		}
		else if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_UP) && input->check(INPUT_UP)) {
			setOffset(memory->b_Procedure->queryWord(iwpo_Selection_Up));
			parseNested();

			if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RELEASED) == false) {
				while (input->check(INPUT_UP)) {
					//TODO: debugmode
					animation->show();
					input->refresh();
				}
			}
		}
		else if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_DOWN) && input->check(INPUT_DOWN)) {
			setOffset(memory->b_Procedure->queryWord(iwpo_Selection_Down));
			parseNested();

			if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RELEASED) == false) {
				while (input->check(INPUT_DOWN)) {
					//TODO: debugmode
					animation->show();
					input->refresh();
				}
			}
		}
		else if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_LEFT) && input->check(INPUT_LEFT)) {
			setOffset(memory->b_Procedure->queryWord(iwpo_Selection_Left));
			parseNested();

			if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RELEASED) == false) {
				while (input->check(INPUT_LEFT)) {
					//TODO: debugmode
					animation->show();
					input->refresh();
				}
			}
		}
		else if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RIGHT) && input->check(INPUT_RIGHT)) {
			setOffset(memory->b_Procedure->queryWord(iwpo_Selection_Right));
			parseNested();

			if (memory->b_SystemVariable->testByte(ibf_Selection_Status, STATUS_RELEASED) == false) {
				while (input->check(INPUT_RIGHT)) {
					//TODO: debugmode
					animation->show();
					input->refresh();
				}
			}
		}
	}

	return RETURN_NORMAL;
}