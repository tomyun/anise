#include "memory_block.h"
//#include <assert.h>

/* constructor & destructor */
MemoryBlock::MemoryBlock(word entry, word size)
{
	this->entry = entry;
	this->size = size;
	this->current_byte_index = 0;
	this->current_bit_index = 0;
	this->current_bit_flow = BIT_FLOW_RIGHT;

	raw = new byte[size];

	clear();
}


MemoryBlock::~MemoryBlock()
{
	delete[] raw;
}


/* inform methods */
byte* MemoryBlock::getRaw()
{
	return raw;
}


word MemoryBlock::getEntry()
{
	return entry;
}


word MemoryBlock::getSize()
{
	return size;
}


word MemoryBlock::getCurrentIndex()
{
	return current_byte_index;
}


/* move methods */
void MemoryBlock::set(word byte_index, int bit_flow)
{
	//assert((bit_flow == BIT_FLOW_LEFT) || (bit_flow == BIT_FLOW_RIGHT));

	current_byte_index = byte_index;
	current_bit_flow = bit_flow;
	if (bit_flow == BIT_FLOW_RIGHT) {
		current_bit_index = 0;
	}
	else {
		current_bit_index = 7;
	}
}


void MemoryBlock::advance()
{
	current_byte_index++;
}


/* read methods */
bit MemoryBlock::readBit()
{
	byte current_byte = queryByte();
	bit current_bit = (bit) ((current_byte >> current_bit_index) & 1);

	if (current_bit_flow == BIT_FLOW_RIGHT) {
		current_bit_index++;
		if (current_bit_index > 7) {
			current_byte_index++;
			current_bit_index = 0;
		}
	}
	else {
		current_bit_index--;
		if (current_bit_index < 0) {
			current_byte_index++;
			current_bit_index = 7;
		}
	}

	return current_bit;
}


bit MemoryBlock::readBit(word byte_index, int bit_index)
{
	//assert(byte_index < size);
	//assert(bit_index < 8);

	current_byte_index = byte_index;
	current_bit_index = bit_index;

	return readBit();
}


word MemoryBlock::readBits(byte length)
{
	//assert(length <= 16);
	//assert(length > 0);

	if (length == 1) {
		return readBit();
	}

	word current_bits = 0;

	for (int i = 0; i < length; i++) {
		word current_bit = (word) readBit();

		if (current_bit_flow == BIT_FLOW_RIGHT) {
			current_bits = current_bits | (current_bit << i);
		}
		else {
			current_bits = (current_bits << 1) | current_bit;
		}
	}

	return current_bits;
}


word MemoryBlock::readBits(word byte_index, int bit_index, byte length)
{
	//assert(byte_index < size);
	//assert(bit_index < 8);
	//assert(length <= 16);
	//assert(length > 0);

	current_byte_index = byte_index;
	current_bit_index = bit_index;

	return readBits(length);
}


byte MemoryBlock::readByte()
{
	byte current_byte = *(raw + current_byte_index);
	current_byte_index++;

	return current_byte;
}


byte MemoryBlock::readByte(word index)
{
	//assert(index < size);

	current_byte_index = index;

	return readByte();
}


byte MemoryBlock::queryByte()
{
	return queryByte(current_byte_index);
}


byte MemoryBlock::queryByte(word index)
{
	//assert(index < size);

	byte current_byte = *(raw + index);

	return current_byte;
}


word MemoryBlock::readWord()
{
	word first_byte = (word) readByte();
	word second_byte = (word) readByte();

	return (first_byte | (second_byte << 8));
}


word MemoryBlock::readWord(word index)
{
	//assert(index < size);

	current_byte_index = index;

	return readWord();
}


word MemoryBlock::readWordBE()
{
	byte first_byte = readByte();
	byte second_byte = readByte();

	return ((first_byte << 8) | second_byte);
}


word MemoryBlock::readWordBE(word index)
{
	//assert(index < size);

	current_byte_index = index;

	return readWordBE();
}


word MemoryBlock::queryWord()
{
	return queryWord(current_byte_index);
}


word MemoryBlock::queryWord(word index)
{
	//assert(index < size);

	byte first_byte = queryByte(index);
	byte second_byte = queryByte(index + 1);

	return (first_byte | (second_byte << 8));
}


char* MemoryBlock::queryString(word index, int length)
{
	//assert(index < size);
	//assert(length > 0);

	char *data = new char[length];
	for (int i = 0; i < length; i++) {
		data[i] = (char) queryByte(index + i);
	}

	return data;
}


/* write methods */
void MemoryBlock::writeByte(word index, byte data)
{
	//assert(index < size);

	*(raw + index) = data;
}


void MemoryBlock::writeWord(word index, word data)
{
	//assert(index < size);

	*(raw + index) = (byte) (data & MASK_LOWER_WORD);
	*(raw + index + 1) = (byte) ((data & MASK_UPPER_WORD) >> 8);
}


void MemoryBlock::writeString(word index, char *data, int length) const
{
	//assert(index < size);
	//assert(data != NULL);
	//assert(length > 0);

	for (int i = 0; i < length; i++) {
		*(raw + index + i) = (byte) data[i];
	}
}


void MemoryBlock::writeRaw(byte *data, int length) const
{
	//assert(data != NULL);
	//assert(length > 0);

	for (int i = 0; i < length; i++) {
		*(raw + i) = *(data + i);
	}
}


void MemoryBlock::writeBlock(MemoryBlock *memory_block, word entry)
{
	//assert(memory_block != NULL);
	//assert(entry < memory_block->getSize());

	for (word i = 0; i < size; i++) {
		writeByte(i, memory_block->queryByte(entry + i));
	}
}


/* manipulate methods */
byte MemoryBlock::andByte(word index, byte mask)
{
	//assert(index < size);

	byte data = queryByte(index) & mask;
	writeByte(index, data);

	return data;
}


word MemoryBlock::andWord(word index, word mask)
{
	//assert(index < size);

	word data = queryWord(index) & mask;
	writeWord(index, data);

	return data;
}


byte MemoryBlock::orByte(word index, byte mask)
{
	//assert(index < size);

	byte data = queryByte(index) | mask;
	writeByte(index, data);

	return data;
}


word MemoryBlock::orWord(word index, word mask)
{
	//assert(index < size);

	word data = queryWord(index) | mask;
	writeWord(index, data);

	return data;
}


byte MemoryBlock::xorByte(word index, byte mask)
{
	//assert(index < size);

	byte data = queryByte(index) ^ mask;
	writeByte(index, data);

	return data;
}


word MemoryBlock::xorWord(word index, word mask)
{
	//assert(index < size);

	word data = queryWord(index) ^ mask;
	writeWord(index, data);

	return data;
}


byte MemoryBlock::notByte(word index)
{
	//assert(index < size);

	byte data = ~(queryByte(index));
	writeByte(index, data);

	return data;
}


word MemoryBlock::notWord(word index)
{
	//assert(index < size);

	word data = ~(queryWord(index));
	writeWord(index, data);

	return data;
}


bool MemoryBlock::testByte(word index, byte mask)
{
	//assert(index < size);

	return ((queryByte(index) & mask) == mask);
}


bool MemoryBlock::testWord(word index, word mask)
{
	//assert(index < size);

	return ((queryWord(index) & mask) == mask);
}


byte MemoryBlock::increaseByte(word index)
{
	//assert(index < size);

	byte data = queryByte(index) + 1;
	writeByte(index, data);

	return data;
}


word MemoryBlock::increaseWord(word index)
{
	//assert(index < size);

	word data = queryWord(index) + 1;
	writeWord(index, data);

	return data;
}


byte MemoryBlock::decreaseByte(word index)
{
	//assert(index < size);

	byte data = queryByte(index) - 1;
	writeByte(index, data);

	return data;
}


word MemoryBlock::decreaseWord(word index)
{
	//assert(index < size);

	word data = queryWord(index) - 1;
	writeWord(index, data);

	return data;
}


byte MemoryBlock::shiftRightByte(word index, int count)
{
	//assert(index < size);
	//assert(count > 0);

	byte data = queryByte(index) >> count;
	writeByte(index, data);

	return data;
}


word MemoryBlock::shiftRightWord(word index, int count)
{
	//assert(index < size);
	//assert(count > 0);

	word data = queryWord(index) >> count;
	writeWord(index, data);

	return data;
}


byte MemoryBlock::shiftLeftByte(word index, int count)
{
	//assert(index < size);
	//assert(count > 0);

	byte data = queryByte(index) << count;
	writeByte(index, data);

	return data;
}


word MemoryBlock::shiftLeftWord(word index, int count)
{
	//assert(index < size);
	//assert(count > 0);

	word data = queryWord(index) << count;
	writeWord(index, data);

	return data;
}


/* miscellaneous methods */
void MemoryBlock::clear()
{
	for (word i = 0; i < size; i++) {
		raw[i] = 0;
	}
}


#ifdef DEBUG
void MemoryBlock::dump(word index)
{
	FILE *file = fopen("dump.txt", "wb");

	while (index < getSize()) {
		fprintf(file, "%2x ", queryByte(index));
		if ((index % 16) == 0) {
			fprintf(file, "\n");
		}

		index++;
	}

	fclose(file);
}
#endif
