#ifndef VIDEO_H
#define VIDEO_H

#include "anise.h"
#include "memory.h"
#include "timer.h"
#include "option.h"
#include "SDL_video.h"
#include "SDL_endian.h"
#include "SDL_error.h"

#define FADE_LEVEL		8
#define FONT_BPB		8
#define CHARACTER_TOTAL		8
#define SPRITE_SIZE		16
#define SPRITE_LAYER_MASK	0x0FFF
#define SPRITE_COLOR_MASK	0x07
#define FILTER_RADIUS		1
#define FILTER_PRESERVE		3

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
#ifdef FIELD_EXPERIMENT
	, SURFACE_MAP = 4
#endif
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


#ifdef FIELD_EXPERIMENT
enum CharacterSpriteType {
	SPRITE_PREVIOUS = 0,
	SPRITE_CURRENT = 1
};


enum CharacterSpriteNeighbor {
	SPRITE_LEFT = 0,
	SPRITE_RIGHT = 1
};
#endif


struct BlitStruct {
	struct BlitPlane {		
		word x, y;
		SurfaceType surface_type;
	} source, destination;

	word w, h;
};


#ifdef FIELD_EXPERIMENT
class CharacterSprite {
public:
	word coord_xw;
	word coord_yw;

	word widthw;
	word heightw;

	word *background_layer;
	word *foreground_layer_1st;
	word *foreground_layer_2nd;
	word *foreground_layer_3rd;

	//HACK: only appliable when FILTER_RADIUS is 1
	word *neighbour_background_layer;
	word *neighbour_foreground_layer_1st;
	word *neighbour_foreground_layer_2nd;
	word *neighbour_foreground_layer_3rd;

	CharacterSprite()
	{
		coord_xw = 0;
		coord_yw = 0;

		widthw = 0;
		heightw = 0;

		background_layer = NULL;
		foreground_layer_1st = NULL;
		foreground_layer_2nd = NULL;
		foreground_layer_3rd = NULL;

		neighbour_background_layer = NULL;
		neighbour_foreground_layer_1st = NULL;
		neighbour_foreground_layer_2nd = NULL;
		neighbour_foreground_layer_3rd = NULL;
	}
	
	
	~CharacterSprite()
	{
		if (background_layer != NULL) {
			delete[] background_layer;
		}
		if (foreground_layer_1st != NULL) {
			delete[] foreground_layer_1st;
		}
		if (foreground_layer_2nd != NULL) {
			delete[] foreground_layer_2nd;
		}
		if (foreground_layer_3rd != NULL) {
			delete[] foreground_layer_3rd;
		}

		if (neighbour_background_layer != NULL) {
			delete [] neighbour_background_layer;
		}
		if (neighbour_foreground_layer_1st != NULL) {
			delete [] neighbour_foreground_layer_1st;
		}
		if (neighbour_foreground_layer_2nd != NULL) {
			delete [] neighbour_foreground_layer_2nd;
		}
		if (neighbour_foreground_layer_3rd != NULL) {
			delete [] neighbour_foreground_layer_3rd;
		}
	}


	bool compare(CharacterSprite *character)
	{
		bool return_value = true;
		return_value &= ((coord_xw == character->coord_xw) && (coord_yw == character->coord_yw));
		return_value &= ((widthw == character->widthw) && (heightw == character->heightw));
		
		for (int i = 0; i < (widthw * heightw); i++) {
			return_value &= (background_layer[i] == character->background_layer[i]);
			return_value &= (foreground_layer_1st[i] == character->foreground_layer_1st[i]);
			return_value &= (foreground_layer_2nd[i] == character->foreground_layer_2nd[i]);
			return_value &= (foreground_layer_3rd[i] == character->foreground_layer_3rd[i]);
		}

		return return_value;
	}
};
#endif


class Video {
private:
	Memory *memory;
	Timer *timer;
	Option *option;

	SDL_Surface *sdl_screen;
	byte screen[VIDEO_WIDTH * VIDEO_HEIGHT];
	byte buffer[VIDEO_BUFFER][VIDEO_WIDTH * VIDEO_HEIGHT];

	Uint32 color_red_mask;
	Uint32 color_blue_mask;
	Uint32 color_green_mask;
	Uint32 color_alpha_mask;

#ifdef FIELD_EXPERIMENT
	SDL_Surface *sdl_map;
	byte *map;
	int map_width;
	int map_height;

	CharacterSprite *character[CHARACTER_TOTAL][2];
	bool has_character_moved;
#endif

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

	byte* getSurface(SurfaceType surface_type);
	SurfaceType getDrawSurface();

	bool isScreen(byte *surface);
	bool isScreen(SurfaceType surface_type);

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

#ifdef FIELD_EXPERIMENT
	void initializeMap(word width, word height);
	void createMap();
	void drawMap(SDL_Rect *map_region, SDL_Rect *screen_region);
	void updateCharacter(int index, CharacterSprite *new_character);
	void drawCharacter(word view_coord_xw, word view_coord_yw, word view_margin_xw, word view_margin_y, bool is_forced = false);
#endif
	void putSprite(word coord_x, word coord_y, word background_layer, word foreground_layer_1st, word foreground_layer_2nd, word foreground_layer_3rd, SurfaceType surface_type = SURFACE_SCREEN);

	void putPoint(word coord_x, word coord_y, byte color_index, SurfaceType surface_type = SURFACE_SCREEN);
	byte getPoint(word coord_x, word coord_y, SurfaceType surface_type = SURFACE_SCREEN);

	Uint32 getFilteredColor(word coord_x, word coord_y, SurfaceType surface_type = SURFACE_SCREEN);

	void drawFont(word coord_x, word coord_y, const byte *font, long int offset, word width, word height);

	void capture();
};

#endif
