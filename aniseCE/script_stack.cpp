#include "script_stack.h"

ScriptStack::ScriptStack()
{
	for (int i = 0; i <= STACK_SIZE; i++) {
		data[i] = 0;
	}

	index = 0;
}


ScriptStack::~ScriptStack()
{
}


void ScriptStack::push(word value)
{
	if (index <= STACK_SIZE) {
		data[++index] = value;
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Stack::push()] stack overflow: STACK_SIZE = %d, index = %d\n", STACK_SIZE, index);
		exit(1);
	}
}


word ScriptStack::pop()
{
	if (index > 0) {
		return data[index--];
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Stack::pop()] stack underflow: STACK_SIZE = %d, index = %d\n", STACK_SIZE, index);
		exit(1);
		return -1;
	}
}
