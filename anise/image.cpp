#include "image.h"

Image::Image(Memory *memory, Video *video, File *file)
{
	this->memory = memory;
	this->video = video;
	this->file = file;

	b_Image = NULL;

	coord_x = 0;
	coord_y = 0;

	width = 0;
	height = 0;

	surface_type = SURFACE_SCREEN;

	for (int i = 0; i < (VIDEO_COLOR + 1); i++) {
		for (int j = 0; j < VIDEO_COLOR; j++) {
			table[i][j] = 0;
		}
	}
}


Image::~Image()
{
}


word Image::scan(byte length)
{
	return b_Image->readBits(length);
}


void Image::initializeHeader()
{
	coord_x = b_Image->readWordBE();
	coord_y = b_Image->readWordBE();
	width = b_Image->readWordBE() + 1;
	height = b_Image->readWordBE() + 1;

	memory->b_SystemVariable->writeWord(iw_GP4File_CoordXb, coord_x >> 3);
	memory->b_SystemVariable->writeWord(iw_GP4File_CoordY, coord_y);
	memory->b_SystemVariable->writeWord(iw_GP4File_Widthb, width >> 3);
	memory->b_SystemVariable->writeWord(iw_GP4File_Height, height);
}


void Image::initializeTable()
{
	for (int i = 0; i < (VIDEO_COLOR + 1); i++) {
		for (int j = 0; j < VIDEO_COLOR; j++) {
			table[i][j] = (i + j) & COLOR_MASK;
		}
	}
}


void Image::decode(word destination_x, word destination_y)
{
	int table_index = VIDEO_COLOR;
	int remain_height = (int) height;
	while (remain_height > 0) {
		switch (scan()) {
			case DECODE_PIXEL:
				{
					for(word x = 0; x < 4; x++) {
						byte color = table[table_index][0];
						int color_index = 0;

						while (scan() == 1) {
							color_index++;

							table[table_index][0] = table[table_index][color_index];
							table[table_index][color_index] = color;
							color = table[table_index][0];
						}

						table_index = color;
						video->putPoint((coord_x + destination_x) + x, coord_y + destination_y, color, surface_type);
					}

					destination_y++;
					remain_height--;
				}
				break;

			case DECODE_RLE:
				{
					int horizontal = 1;
					int vertical = 0;

					if (scan() == 0) {					// horizontal (7)
						vertical = (int) (scan(4) - 8);
					}
					else {
						if (scan() == 0) {				// vetical
							horizontal = 0;

							vertical = (int) (scan(3) - 8);
							if (vertical <= -7) {
								if (vertical == -7) {		// -8(?)
									vertical = 0;
								}
								vertical = vertical - 8;	// -16(?)
							}
						}
						else {						// horizontal (6 ~ 4)
							do {
								horizontal++;
							} while (scan() == 1);

							vertical = (int) (scan(4) - 8);
						}
					}

					int replica_x = destination_x - (horizontal * 4);
					int replica_y = destination_y + vertical;
					word length = 0;

					if (scan() == 0) {
						length = scan() + 2;
					}
					else {
						if (scan() == 0) {
							length = scan(2) + 4;
						}
						else {
							if (scan() == 0) {
								length = scan(3) + 8;
							}
							else {
								length = scan(6) + 16;
								if (length >= 79) {
									length = scan(10) + 79;
								}
							}
						}
					}

					remain_height -= length;

					for (word y = 0; y < length; y++) {
						for (word x = 0; x < 4; x++) {
							byte color = video->getPoint((coord_x + replica_x) + x, coord_y + replica_y, surface_type);
							video->putPoint((coord_x + destination_x) + x, coord_y + destination_y, color, surface_type);
						}

						replica_y++;
						destination_y++;
					}
				}
				break;
		}
	}
}


void Image::load(const char *filename)
{
	word image_offset = memory->b_SystemVariable->queryWord(iwpo_GP4File);
	MemorySegment *segment = memory->getCurrentSegment();
	b_Image = segment->get(&image_offset);

	if (filename != NULL) {
		file->open(filename, FILE_READ);
		file->load(b_Image, image_offset);
		file->close();
	}

	b_Image->set(image_offset);

	initializeHeader();
	initializeTable();

	surface_type = video->getDrawSurface();

	b_Image->set(image_offset + GP4_RAW_OFFSET, BIT_FLOW_LEFT);

	word destination_x = 0;
	word destination_y = 0;
	for (word x = 0; x < (width / 4); x++) {
		decode(destination_x, destination_y);

		destination_x += 4;
		destination_y = 0;
	}

	for (int i = 0; i < VIDEO_COLOR; i++) {
		word image_color = b_Image->readWordBE(image_offset + GP4_PALETTE_OFFSET + (i * 2));
		word gp4_color = (((image_color >> 12) & COLOR_MASK) << 8) | (((image_color >> 7) & COLOR_MASK) << 4) | ((image_color >> 2) & COLOR_MASK);

		if (memory->b_SystemVariable->testByte(ibf_DisabledStatus, DISABLE_PRESERVEIMAGEPALETTE) == false) {
			memory->b_SystemVariable->writeWord(iw_Video_Palette0 + (i * 2), gp4_color);
		}

		//HACK: ensure correct palette for screenshot buffers
		SDL_Color sdl_color;
		video->splitColor(&sdl_color, gp4_color);
		video->updateColor(&sdl_color, i, surface_type);
	}

	if (video->isScreen(surface_type)) {
		video->updateScreen(coord_x, coord_y, width, height);
	}
}
