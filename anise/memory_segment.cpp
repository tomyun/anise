#include "memory_segment.h"


MemorySegment::MemorySegment(word entry, word size)
{
	list[0] = new MemoryBlock(entry, size);
	table[0][SEGMENT_OFFSET_START] = entry;
	table[0][SEGMENT_OFFSET_END] = entry + size;

	count = 1;
}


MemorySegment::~MemorySegment()
{
}


void MemorySegment::add(MemoryBlock *memory_block)
{
	if (count < MAX_BLOCK) {
		list[count] = memory_block;
		table[count][SEGMENT_OFFSET_START] = memory_block->getEntry();
		table[count][SEGMENT_OFFSET_END] = table[count][SEGMENT_OFFSET_START] + memory_block->getSize();

		count++;
	}
	else {
		//TODO: process error
		PRINT("[MemorySegment::add()] segment space is full (%d of %d)\n", count, MAX_BLOCK);
		exit(1);
	}
}


MemoryBlock* MemorySegment::get(word *entry)
{
	MemoryBlock *memory_block = NULL;

	for (int i = 0; i < count; i++) {
		word start_offset = table[i][SEGMENT_OFFSET_START];
		word end_offset = table[i][SEGMENT_OFFSET_END];

		if ((*entry >= start_offset) && (*entry < end_offset)) {
			memory_block = list[i];
		}
	}

	//assert(memory_block != NULL);

	*entry = *entry - memory_block->getEntry();

	return memory_block;
}


word MemorySegment::getEntry()
{
	return table[0][SEGMENT_OFFSET_START];
}


word MemorySegment::getSize()
{
	return (table[0][SEGMENT_OFFSET_END] - table[0][SEGMENT_OFFSET_START]);
}