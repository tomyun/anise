#ifndef MEMORY_H
#define MEMORY_H

#include "memory_segment.h"
#include "memory_block.h"
#include "memory_index.h"
#include "option.h"

#define SEGMENT_SIZE			0xFFFF

#define SCRIPT_ENTRY			0
#define SCRIPT_SIZE				0xE000
#define SCRIPT_NAME_SIZE		32
#define BASE_VARIABLE_SIZE		54		// 27 * 2
#define SYSTEM_VARIABLE_SIZE	200		// 100 * 2
#define SELECTION_ITEM_SIZE		44		// 11 * (2 * 2)
#define PROCEDURE_SIZE			122		// 61 * 2
#define ANIMATION_SLOT_SIZE		162		// 10 * 16 + 2
#define FLAG_SIZE				3072

class Memory {
private:
	Option *option;

public:
	// segments and blocks
	MemorySegment *s_Core;
	MemorySegment *s_Resources;
	MemorySegment *s_FieldData;
	MemorySegment *s_FieldMap;

	MemoryBlock *b_Script;
	MemoryBlock *b_ScriptName;
	MemoryBlock *b_Variable;
	MemoryBlock *b_BaseVariable;
	MemoryBlock *b_SystemVariable;
	MemoryBlock *b_Heap;
	MemoryBlock *b_SelectionItem;
	MemoryBlock *b_Procedure;
	MemoryBlock *b_AnimationSlot;

	// information about segments and memory blocks
	word variable_size;

	word script_entry;
	word script_name_entry;
	word variable_entry;
	word base_variable_entry;
	word system_variable_entry;
	word heap_entry;
	word selection_item_entry;
	word procedure_entry;
	word animation_slot_entry;
	word animation_script_entry;
	word flag_entry;

	// constructor & destructor
	Memory(Option *option);
	~Memory();

	// methods
	bool initialize();
	MemorySegment* getCurrentSegment();
	void loadFlag(MemoryBlock *b_Flag);
	byte loadVariable(word index);
	void saveVariable(word index, word data);
};

#endif