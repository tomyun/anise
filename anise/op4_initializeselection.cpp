#include "script.h"

#define CHECKITEM_NONE	0xFF

enum InitializeSelectionFunction {
	SELECTION_SET = 0,
	SELECTION_COUNT = 1
};

byte checkItem(Script *script, word *item_offset);

SCRIPTCALL Script::op4_initializeSelection()
{
	parameter = getParameter();
	word function = parameter->get(0);
	deleteParameter();

	word script_offset = getOffset();

	switch (function) {
		case SELECTION_SET:
		default:
			{
				byte total_item_count = (byte) memory->b_SystemVariable->queryWord(iw_Selection_TotalItemCount);
				byte skip_item_count = (byte) memory->b_SystemVariable->queryWord(iw_Selection_SkipItemCount);

				word source_offset = memory->b_Procedure->queryWord(iwpo_Selection_List);
				word destination_offset = memory->b_SystemVariable->queryWord(iwpo_Selection_Item);

				setOffset(source_offset);
				MemoryBlock *destination = memory->s_Core->get(&destination_offset);

				for (int i = 0; i < (total_item_count * 2); i++) {
					destination->writeWord(destination_offset + (i * 2), 0);
				}

				word item_type = 0;
				while (true) {
					item_type++;

					word item_offset;
					byte return_value = checkItem(this, &item_offset);
					if (return_value == CHECKITEM_NONE) {
						break;
					}
					else if (return_value == 0) {
						continue;
					}
					else {
						if (skip_item_count == 0) {
							destination->writeWord(destination_offset, item_offset);
							destination->writeWord(destination_offset + 2, item_type);

							destination_offset += 4;
							total_item_count--;
						}
						else {
							skip_item_count--;
						}
					}
				}
			}
			break;

		case SELECTION_COUNT:
			{
				word source_offset = memory->b_Procedure->queryWord(iwpo_Selection_List);
				setOffset(source_offset);

				word current_item_count = 0;
				while (true) {
					word item_offset;
					byte return_value = checkItem(this, &item_offset);
					if (return_value == CHECKITEM_NONE) {
						memory->b_SystemVariable->writeWord(iw_Selection_CurrentItemCount, current_item_count);
						break;
					}
					else if (return_value == 0) {
						continue;
					}
					else {
						current_item_count++;
					}
				}
			}
			break;
	}

	setOffset(script_offset);

	return RETURN_NORMAL;
}


byte checkItem(Script *script, word *item_offset)
{
	byte return_value = CHECKITEM_NONE;

	byte code = script->fetch();
	if (code == CODE_NULL) {
		return return_value;
	}

	script->advance();
	return_value = 1;

	code = script->fetch();
	if (code == CODE_EXPRESSION) {
		script->advance();
		return_value = (byte) script->readExpression();
	}

	*item_offset = script->getOffset();

	while (true) {
		code = script->fetch();
		if (code == CODE_NULL || code == CODE_CONTINUE) {	//TODO: does it mean right?
			return return_value;
		}

		script->advance();
	}
}
