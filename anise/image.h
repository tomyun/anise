#ifndef IMAGE_H
#define IMAGE_H

#include "memory.h"
#include "video.h"
#include "file.h"

enum GP4FileOffset {
	GP4_PALETTE_OFFSET = 8,
	GP4_RAW_OFFSET = 40
};


enum DecodeType {
	DECODE_PIXEL = 0,
	DECODE_RLE = 1
};


class Image {
private:
	Memory *memory;
	Video *video;
	File *file;

	MemoryBlock *b_Image;

	word coord_x;
	word coord_y;
	word width;
	word height;

	byte table[VIDEO_COLOR + 1][VIDEO_COLOR];
	byte *surface;

	word scan(byte length);

	void initializeHeader();
	void initializeTable();
	void decode(word destination_x, word destination_y);

public:
	Image(Memory *memory, Video *video, File *file);
	~Image();

	void load(const char *filename);
};

#endif
