#ifndef SCRIPT_PARAMETER_H
#define SCRIPT_PARAMETER_H

#include "memory.h"
#include "script_code.h"

#define MAX_PARAMETER		15
#define PARAMETER_LENGTH	32

enum ScriptParameterType {
	PARAMETER_TYPE_CONSTANT = 0x01,
	PARAMETER_TYPE_STRING = 0x02,
	PARAMETER_TYPE_BLOCK = 0x03
};


class ScriptParameter {
private:
	Memory *memory;

	int count;
	byte type_list[MAX_PARAMETER];
	word value_list[MAX_PARAMETER];

public:
	ScriptParameter(Memory *memory);
	~ScriptParameter();

	void initialize();
	int add(byte type, word value);
	word get(int index);
	byte getType(int index);
	int getCount();
	void load(int index, char *name);
};

#endif
