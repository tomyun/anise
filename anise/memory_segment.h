#ifndef MEMORY_SEGMENT_H
#define MEMORY_SEGMENT_H

#include "anise.h"
#include "memory_block.h"

#define MAX_BLOCK	10

enum SegmentOffset {
	SEGMENT_OFFSET_START = 0,
	SEGMENT_OFFSET_END = 1
};


class MemorySegment {
private:
	MemoryBlock* list[MAX_BLOCK];
	word table[MAX_BLOCK][2];
	int count;

public:
	// constructor & destructor
	MemorySegment(word entry, word size);
	~MemorySegment();

	// methods
	void add(MemoryBlock *memory_block);
	MemoryBlock* get(word *entry);
	word getEntry();
	word getSize();
};

#endif
