#include "script.h"
#include "script_code.h"

word Script::readConstant()
{
	byte code = fetchAdvance();
	word operand;
	if ((code >= CODE_ASCIINUMBER_FIRST) && (code <= CODE_ASCIINUMBER_LAST)) {
		operand = (word) (code - CODE_ASCIINUMBER_FIRST);
	}
	else if (code == CODE_CONSTANT_1OP) {
		operand = readConstant1Op();
	}
	else if (code == CODE_CONSTANT_2OP) {
		operand = readConstant2Op();
	}
	else if (code == CODE_CONSTANT_3OP) {
		operand = readConstant3Op();
	}
	else {
		operand = (word) code;
	}

	return operand;
}


word Script::readConstant1Op()
{
	word operand = (word) fetchAdvance();
	operand = operand >> 2;

	return operand;
}


word Script::readConstant2Op()
{
	word operand = readConstant1Op();
	byte second_byte = fetchAdvance();

	operand = ((operand << 8) | second_byte) >> 2;

	return operand;
}


word Script::readConstant3Op()
{
	word operand = readConstant2Op();
	byte third_byte = fetchAdvance();

	// shift right and rotate carry right twice
	for (int i = 0; i < 2; i++) {
		third_byte = (third_byte >> 1) | (0x80 * (operand & 1));
		operand = operand >> 1;
	}

	operand = ((operand & 0xFF) << 8) | third_byte;

	return operand;
}


word Script::readExpression()
{
	word operand;
	stack = new ScriptStack();

	while (true) {
		byte code = fetchAdvance();
		if (code == CODE_EXPRESSION_END) {
			operand = stack->pop();

			delete stack;

			return operand;
		}
		else if ((code >= CODE_ASCIINUMBER_FIRST) && (code <= CODE_ASCIINUMBER_LAST)) {
			operand = (word) (code - CODE_ASCIINUMBER_FIRST);
			stack->push(operand);
		}
		else if (code == CODE_CONSTANT_1OP) {
			operand = readConstant1Op();
			stack->push(operand);
		}
		else if (code == CODE_CONSTANT_2OP) {
			operand = readConstant2Op();
			stack->push(operand);
		}
		else if (code == CODE_CONSTANT_3OP) {
			operand = readConstant3Op();
			stack->push(operand);
		}
		else if ((code >= CODE_BASEVARIABLE_FIRST) && (code <= CODE_BASEVARIABLE_LAST)) {
			word index = (code - CODE_BASEVARIABLE_FIRST) * 2;
			operand = memory->b_BaseVariable->queryWord(index);
			stack->push(operand);
		}
		else if ((code >= CODE_CONSTANT_RE_FIRST) && (code <= CODE_CONSTANT_RE_LAST)) {
			code = code - CODE_CONSTANT_RE_FIRST;
			int condition = excuteOpcodeOpre(code);
		}
	}
}