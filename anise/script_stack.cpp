#include "script_stack.h"

ScriptStack::ScriptStack()
{
	for (int i = 0; i < STACK_SIZE; i++) {
		data[i] = 0;
	}

	index = 0;
}


ScriptStack::~ScriptStack()
{
}


void ScriptStack::push(word value)
{
	if (index < STACK_SIZE) {
		data[index] = value;
		index++;
	}
	else {
		//TODO: process error
		PRINT("[Stack::push()] stack overflow\n");
		exit(1);
	}
}


word ScriptStack::pop()
{
	if (index > 0) {
		index--;
		return data[index];
	}
	else {
		//TODO: process error
		PRINT("[Stack::pop()] stack underflow\n");
		exit(1);
	}
}