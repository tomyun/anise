#include "script.h"

#define OPCODE_IF	0x0F

SCRIPTCALL Script::op_if()
{
	word condition;
	word if_condition;

	while (true) {
		if_condition = readExpression();
		if (if_condition) {
			condition = parseNested();

			while (fetch() == CODE_CONTINUE) {
				condition = skipScriptBlock();
			}
			if (condition == RETURN_SUCCESS) {
				return RETURN_NORMAL;
			}
			else {
				return condition;
			}
		}
		else {
			condition = skipScriptBlock();

			if (fetch() == CODE_CONTINUE) {
				advance();

				if (fetch() == OPCODE_IF) {
					advance();
				}
				else {
					condition = parseNested();

					if (condition == RETURN_SUCCESS) {
						return RETURN_NORMAL;
					}
					else {
						return condition;
					}
				}
			}
			else {
				return RETURN_NORMAL;
			}
		}
	}
}