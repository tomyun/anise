#ifndef VIDEO_H
#define VIDEO_H

#include "anise.h"
#include "memory.h"
#include "timer.h"
#include "option.h"
#include "SDL_video.h"
#include "SDL_endian.h"
#include "SDL_error.h"

#define FADE_LEVEL			8
#define FONT_BPB			8
#define SPRITE_SIZE			16
#define SPRITE_LAYER_MASK	0x0FFF
#define SPRITE_COLOR_MASK	0x07
#define FILTER_RADIUS		2

enum VideoSpecification {
	VIDEO_WIDTH = 640,
	VIDEO_HEIGHT = 400,
	VIDEO_COLOR_DEPTH = 32,
	VIDEO_COLOR = 16,
	VIDEO_BUFFER = 3
};


enum Color {
	COLOR_RED = 0x00F0,
	COLOR_GREEN = 0x0F00,
	COLOR_BLUE = 0x000F,
	COLOR_RGB = 3,
	COLOR_LEVEL = 16,
	COLOR_KEY = 0x08,
	COLOR_MASK = 0x0F,
	COLOR_NONE = 0xFF
};


enum SurfaceType {
	SURFACE_SCREEN = 0,
	SURFACE_BUFFER1 = 1,
	SURFACE_BUFFER2 = 2,
	SURFACE_BUFFER3 = 3
};


enum DrawBoxType {
	DRAWBOX_SOLID = 0,
	DRAWBOX_INVERSE = 1
};

enum BlitType {
	BLIT_DIRECT = 0,
	BLIT_SWAPPED = 1,
	BLIT_MASKED = 2,
	BLIT_ERROR = 3
};


enum FontColor {
	FONT_BACKGROUND = 0,
	FONT_FOREGROUND = 1
};


struct BlitStruct {
	struct BlitPlane {		
		word x, y;
		byte *s;
	} source, destination;

	word w, h;
};


class Video {
private:
	Memory *memory;
	Timer *timer;
	Option *option;

	SDL_Surface *sdl_screen;
	byte screen[VIDEO_WIDTH * VIDEO_HEIGHT];
	byte buffer[VIDEO_BUFFER][VIDEO_WIDTH * VIDEO_HEIGHT];

	Uint32 sdl_palette[VIDEO_COLOR];
	word intermediate_palette[VIDEO_COLOR];

	SDL_Surface *overlap_old_screen;
	SDL_Surface *overlap_new_screen;
public:
	bool overlap_inuse;
	word overlap_parameter;
	word overlap_level;
	word overlap_current_level;
	word overlap_delay;

private:
	// SDL related methods
	void lockScreen(SDL_Surface *surface);
	void lockScreen();

	void unlockScreen(SDL_Surface *surface);
	void unlockScreen();

	void drawPixel(SDL_Surface *sdl_surface, int x, int y, Uint32 sdl_color);

public:
	Video(Memory *memory, Timer *timer, Option *option);
	~Video();

	void setColor(byte color_index, word color);
	Uint32 getColor(byte color_index);
	Uint32 convertColor(word color);
	void splitColor(word color, Uint8 *color_red, Uint8 *color_green, Uint8 *color_blue);
	void setIntermediateColor(byte color_index, word color);
	word getIntermediateColor(byte color_index);
	void setIntermediatePalette();
	void setPalette();

	void drawBox(byte mode, word coord_x0b, word coord_y0, word coord_x1b, word coord_y1);
	void clearScreen();

	byte* getSurface(byte surface_type);
	byte* getDrawSurface();
	bool isScreen(byte *surface);
	bool isScreen(byte surface_type);

	void updateScreen(word coord_x, word coord_y, word width, word height);
	void updateScreen();
	void fadeScreen();
	void initializeOverlapScreen();
	void overlapScreen();

	void blit(byte mode, word source_coord_x0b, word source_coord_y0, word source_coord_x1b, word source_coord_y1, byte source_type, word destination_coord_xb, word destination_coord_y, byte destination_type);
	void blitDirect(BlitStruct *order);
	void blitSwapped(BlitStruct *order);
	void blitMasked(BlitStruct *order);

	void blitMerged(byte mode, word foreground_coord_x, word foreground_coord_y, word background_coord_x, word background_coord_y, word destination_coord_x, word destination_coord_y, word width, word height);
	void putSprite(word coord_x, word coord_y, word background_layer, word foreground_layer_1st, word foreground_layer_2nd, word foreground_layer_3rd);

	void putPoint(byte surface_type, word coord_x, word coord_y, byte color_index);
	void putPoint(word coord_x, word coord_y, byte color_index);
	byte getPoint(byte surface_type, word coord_x, word coord_y);
	byte getPoint(word coord_x, word coord_y);

	void drawFont(word coord_x, word coord_y, const byte *font, long int offset, word width, word height);

	void filter(word coord_x, word coord_y, word width, word height);

	void dump();
};

#endif
