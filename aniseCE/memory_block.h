#ifndef MEMORY_BLOCK_H
#define MEMORY_BLOCK_H

#include "anise.h"

class MemoryBlock {
private:
	byte *raw;
	word entry;
	word size;
	word current_byte_index;
	int current_bit_index;
	int current_bit_flow;

public:
	/* constructor & destructor */
	MemoryBlock(word entry, word size);
	~MemoryBlock();

	/* inform methods */
	byte* getRaw();
	word getEntry();
	word getSize();
	word getCurrentIndex();

	/* move methods */
	void advance();
	void set(word byte_index, int bit_flow = BIT_FLOW_RIGHT);

	/* read methods */
	bit readBit();
	bit readBit(word byte_index, int bit_index);
	word readBits(byte length);
	word readBits(word byte_index, int bit_index, byte length);

	byte readByte();
	byte readByte(word index);
	byte queryByte();
	byte queryByte(word index);

	word readWord();
	word readWord(word index);
	word readWordBE();
	word readWordBE(word index);
	word queryWord();
	word queryWord(word index);

	char* queryString(word index, int length);

	/* write methods */
	void writeByte(word index, byte data);
	void writeWord(word index, word data);
	void writeString(word index, char *data, int length) const;
	void writeRaw(byte *data, int length) const;
	void writeBlock(MemoryBlock *memory_block, word entry);

	/* manipulate methods */
	byte andByte(word index, byte mask);
	word andWord(word index, word mask);
	
	byte orByte(word index, byte mask);
	word orWord(word index, word mask);

	byte xorByte(word index, byte mask);
	word xorWord(word index, word mask);

	byte notByte(word index);
	word notWord(word index);

	bool testByte(word index, byte mask);
	bool testWord(word index, word mask);

	byte increaseByte(word index);
	word increaseWord(word index);

	byte decreaseByte(word index);
	word decreaseWord(word index);

	byte shiftRightByte(word index, int count);
	word shiftRightWord(word index, int count);

	byte shiftLeftByte(word index, int count);
	word shiftLeftWord(word index, int count);

	/* miscellaneous methods */
	void clear();

#ifdef DEBUG
	void dump(word index);
#endif
};

#endif
