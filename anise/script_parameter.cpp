#include "script_parameter.h"

ScriptParameter::ScriptParameter(Memory *memory)
{
	this->memory = memory;

	initialize();
}


ScriptParameter::~ScriptParameter()
{
}


void ScriptParameter::initialize()
{
	count = 0;

	for (int i = 0; i < MAX_PARAMETER; i++) {
		type_list[i] = 0;
		value_list[i] = 0;
	}
}


int ScriptParameter::add(byte type, word value)
{
	//assert(count < MAX_PARAMETER);

	type_list[count] = type;
	value_list[count] = value;
	count++;

	return count;
}


word ScriptParameter::get(int index)
{
	//assert(index >= 0);
	//assert(index < count);

	return value_list[index];
}


byte ScriptParameter::getType(int index)
{
	//assert(index >= 0);
	//assert(index < count);

	return type_list[index];
}


int ScriptParameter::getCount()
{
	return count;
}


void ScriptParameter::load(int index, char *name)
{
	//assert(index >= 0);
	//assert(index < count);

	byte data;

	word source_index = value_list[index];
	MemoryBlock *source = memory->s_Core->get(&source_index);
	word destination_index = 0;

	if (type_list[index] == PARAMETER_TYPE_CONSTANT) {
		do {
			data = source->queryByte(source_index++);
			name[destination_index++] = (char) data;
		} while (data != CODE_NULL);
	}
	else if (type_list[index] == PARAMETER_TYPE_STRING) {
		source_index++;
		while (true) {
			data = source->queryByte(source_index++);
			if (data == CODE_PARAMETER_STRING) {
				name[destination_index++] = CODE_NULL;
				break;
			}
			else {
				name[destination_index++] = (char) data;
			}
		}
	}
}