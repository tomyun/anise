#include "script.h"

enum OpreResult {
	OPRE_FALSE = 0x0000,
	OPRE_TRUE = 0x0001
};


SCRIPTCALL Script::opre_add()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result = (word) (first_word + second_word);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_subtract()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result;
	if (first_word > second_word) {
		result = first_word - second_word;
	}
	else {
		result = 0;
	}
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_multiply()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result = (word)(first_word * second_word);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_divide()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result = (word) (first_word / second_word);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_modular()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result = first_word % second_word;
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_or()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result = first_word | second_word;
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_and()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word result = first_word & second_word;
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_isEqual()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	if (first_word == second_word) {
		stack->push(OPRE_TRUE);
	}
	else {
		stack->push(OPRE_FALSE);
	}

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_isNotEqual()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	if (first_word != second_word) {
		stack->push(OPRE_TRUE);
	}
	else {
		stack->push(OPRE_FALSE);
	}

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_isAbove()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	if (first_word > second_word) {
		stack->push(OPRE_TRUE);
	}
	else {
		stack->push(OPRE_FALSE);
	}

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_isBelow()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	if (first_word < second_word) {
		stack->push(OPRE_TRUE);
	}
	else {
		stack->push(OPRE_FALSE);
	}

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_getWordFromMemory()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word index = first_word + (second_word * 2);

	MemorySegment *segment;
	if (first_word == memory->system_variable_entry) {
		segment = memory->s_Core;
	}
	else {
		segment = memory->getCurrentSegment();
	}

	MemoryBlock *memory_block = segment->get(&index);

	word result = memory_block->queryWord(index);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_getByteFromMemory()
{
	word second_word = stack->pop();
	word first_word = stack->pop();

	word index = first_word + second_word;

	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *memory_block = segment->get(&index);

	word result = (word) memory_block->queryByte(index);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_getConstantFromOperand()
{
	word index = readConstant();
	word result = (word) memory->loadVariable(index);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_getConstantFromStack()
{
	word index = stack->pop();
	word result = (word) memory->loadVariable(index);
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_random()
{
	word range = readConstant();
	random_seed = (word) ((random_seed * 5) + 1);

	word result = random_seed % range;
	stack->push(result);

	return RETURN_NORMAL;
}


SCRIPTCALL Script::opre_null()
{
	return RETURN_NORMAL;
}