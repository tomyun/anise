#ifndef SCRIPT_STACK_H
#define SCRIPT_STACK_H

#include "anise.h"

#define STACK_SIZE	64

class ScriptStack {
private:
	word data[STACK_SIZE];
	int index;

public:
	ScriptStack();
	~ScriptStack();

	void push(word value);
	word pop();
};

#endif
