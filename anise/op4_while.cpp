#include "script.h"

SCRIPTCALL Script::op4_while()
{
	while (true) {
		word while_offset = getOffset();
		advance();

		word while_condition = readExpression();
		if (while_condition) {
			word condition = parseNested();

			setOffset(while_offset);

			if (condition == RETURN_BREAK) {
				skipScriptBlock();

				return RETURN_NORMAL;
			}
		}
		else {
			skipScriptBlock();

			return RETURN_NORMAL;
		}
	}
}