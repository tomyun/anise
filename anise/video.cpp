#include "video.h"

Video::Video(Memory *memory, Option *option)
{
	this->memory = memory;
	this->option = option;

	sdl_screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, SDL_SWSURFACE);
	if (sdl_screen == NULL) {
		PRINT("[Video::Video()] unable to set %dx%dx%d video mode: %s\n", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, SDL_GetError());
	}

	overlap_old_screen = NULL;
	overlap_new_screen = NULL;

	overlap_inuse = false;
	overlap_parameter = 0;
	overlap_level = 0;
	overlap_current_level = 0;
	overlap_delay = 0;
}


Video::~Video()
{
	SDL_FreeSurface(sdl_screen);
}


inline void Video::setColor(byte color_index, word color)
{
	if (color_index < VIDEO_COLOR) {
		sdl_palette[color_index] = convertColor(color);
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::setColor()] out of bound\n");
		PAUSE;
	}

	//updateScreen();
}


inline Uint32 Video::getColor(byte color_index)
{
	if (color_index < VIDEO_COLOR) {
		return sdl_palette[color_index];
	}
	else {
		//HACK: improve it
		return 0x000000;

		//TODO: process error
		PRINT_ERROR("[Video::getColor()] out of bound\n");
		PAUSE;
		exit(1);
	}
}


Uint32 Video::convertColor(word color)
{
	Uint8 color_red, color_green, color_blue;
	splitColor(color, &color_red, &color_green, &color_blue);

	return SDL_MapRGB(sdl_screen->format, color_red, color_green, color_blue);
}


inline void Video::splitColor(word color, Uint8 *color_red, Uint8 *color_green, Uint8 *color_blue)
{
	*color_red = (Uint8) (((color >> 4) & COLOR_MASK) * 0x11);
	*color_green = (Uint8) (((color >> 8) & COLOR_MASK) * 0x11);
	*color_blue = (Uint8) (((color >> 0) & COLOR_MASK) * 0x11);
}


inline void Video::setIntermediateColor(byte color_index, word color)
{
	if (color_index < VIDEO_COLOR) {
		intermediate_palette[color_index] = color;
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::setIntermediateColor()] out of bound\n");
		PAUSE;
	}
}


word Video::getIntermediateColor(byte color_index)
{
	if (color_index < VIDEO_COLOR) {
		return intermediate_palette[color_index];
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::getIntermediateColor()] out of bound\n");
		PAUSE;
		exit(1);
	}
}


void Video::setIntermediatePalette()
{
	for (int i = 0; i < VIDEO_COLOR; i++) {
		word color = memory->b_SystemVariable->queryWord(iw_Video_Palette0 + (i * 2));
		setIntermediateColor(i, color);
	}
}


void Video::setPalette()
{
	for (int i = 0; i < VIDEO_COLOR; i++) {
		setColor(i, intermediate_palette[i]);
	}

	updateScreen();
}


inline byte* Video::getSurface(byte surface_type)
{
	byte *surface;

	switch (surface_type) {
		case SURFACE_SCREEN:
			surface = screen;
			break;
		case SURFACE_BUFFER1:
			surface = buffer[0];
			break;
		case SURFACE_BUFFER2:
			surface = buffer[1];
			break;
		case SURFACE_BUFFER3:
		default:
			surface = buffer[2];
			break;
	}

	return surface;
}


byte* Video::getDrawSurface()
{
	byte surface_type = (byte) (memory->b_SystemVariable->queryWord(iw_DisplayBuffer));

	return getSurface(surface_type);
}


inline bool Video::isScreen(byte *surface)
{
	return (surface == screen);
}


inline bool Video::isScreen(byte surface_type)
{
	return (surface_type == SURFACE_SCREEN);
}


void Video::updateScreen(word coord_x, word coord_y, word width, word height)
{
	lockScreen();

	//TODO: implement this
	//filter(coord_x, coord_y, width, height);

	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			Uint32 sdl_color = getColor(getPoint(coord_x + x, coord_y + y));
			drawPixel(sdl_screen, (coord_x + x), (coord_y + y), sdl_color);
		}
	}

	SDL_UpdateRect(sdl_screen, coord_x, coord_y, width, height);

	unlockScreen();
}


inline void Video::updateScreen()
{
	updateScreen(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
}


void Video::fadeScreen()
{
	for (int i = 0; i < VIDEO_COLOR; i++) {
		setColor(i, intermediate_palette[i]);
	}

	SDL_Surface *old_screen = SDL_CreateRGBSurface(SDL_HWSURFACE, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, 0, 0, 0, 0);
	SDL_Surface *new_screen = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, 0, 0, 0, 0);

	lockScreen();
	lockScreen(old_screen);
	lockScreen(new_screen);

	SDL_BlitSurface(sdl_screen, NULL, old_screen, NULL);

	for (word y = 0; y < VIDEO_HEIGHT; y++) {
		for (word x = 0; x < VIDEO_WIDTH; x++) {
			Uint32 sdl_color = getColor(getPoint(x, y));
			drawPixel(new_screen, x, y, sdl_color);
		}
	}

	int fade_interval = (int) (SDL_ALPHA_OPAQUE / FADE_LEVEL);
	for (int i = SDL_ALPHA_OPAQUE - (fade_interval * FADE_LEVEL); i <= SDL_ALPHA_OPAQUE; i += fade_interval) {
		SDL_SetAlpha(new_screen, SDL_SRCALPHA, i);

		SDL_BlitSurface(old_screen, NULL, sdl_screen, NULL);
		SDL_BlitSurface(new_screen, NULL, sdl_screen, NULL);

		SDL_UpdateRect(sdl_screen, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}

	unlockScreen();
	unlockScreen(old_screen);
	unlockScreen(new_screen);

	SDL_FreeSurface(old_screen);
	SDL_FreeSurface(new_screen);
}


void Video::initializeOverlapScreen()
{
	if (overlap_old_screen != NULL) {
		SDL_FreeSurface(overlap_old_screen);
	}
	overlap_old_screen = SDL_CreateRGBSurface(SDL_HWSURFACE, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, 0, 0, 0, 0);

	if (overlap_new_screen != NULL) {
		SDL_FreeSurface(overlap_new_screen);
	}
	overlap_new_screen = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, 0, 0, 0, 0);

	lockScreen();
	lockScreen(overlap_old_screen);
	lockScreen(overlap_new_screen);

	SDL_BlitSurface(sdl_screen, NULL, overlap_old_screen, NULL);

	setIntermediatePalette();
	for (int i = 0; i < VIDEO_COLOR; i++) {
		setColor(i, intermediate_palette[i]);
	}

	for (word y = 0; y < VIDEO_HEIGHT; y++) {
		for (word x = 0; x < VIDEO_WIDTH; x++) {
			Uint32 sdl_color = getColor(getPoint(x, y));
			drawPixel(overlap_new_screen, x, y, sdl_color);
		}
	}

	unlockScreen();
	unlockScreen(overlap_old_screen);
	unlockScreen(overlap_new_screen);

	overlap_inuse = true;
}


void Video::overlapScreen()
{
	int overlap_interval = (int) (SDL_ALPHA_OPAQUE / overlap_level);
	int overlap_initial = SDL_ALPHA_OPAQUE - (overlap_interval * overlap_level);
	SDL_SetAlpha(overlap_new_screen, SDL_SRCALPHA, overlap_initial + (overlap_interval * overlap_current_level));

	lockScreen();
	lockScreen(overlap_old_screen);
	lockScreen(overlap_new_screen);

	SDL_BlitSurface(overlap_old_screen, NULL, sdl_screen, NULL);
	SDL_BlitSurface(overlap_new_screen, NULL, sdl_screen, NULL);

	SDL_UpdateRect(sdl_screen, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);

	unlockScreen();
	unlockScreen(overlap_old_screen);
	unlockScreen(overlap_new_screen);

	overlap_current_level++;
	if (overlap_current_level == overlap_level) {
		overlap_inuse = false;

		SDL_FreeSurface(overlap_old_screen);
		SDL_FreeSurface(overlap_new_screen);

		overlap_old_screen = NULL;
		overlap_new_screen = NULL;

		updateScreen();
	}
}


void Video::drawBox(byte mode, word coord_x0b, word coord_y0, word coord_x1b, word coord_y1)
{
	word coord_x = coord_x0b << 3;
	word coord_y = coord_y0;
	word width = (coord_x1b - coord_x0b + 1) << 3;
	word height = coord_y1 - coord_y0 + 1;

	byte color = (byte) memory->b_SystemVariable->queryByte(iw_Video_Color);
	color = color >> 4;

	byte *draw_surface = getDrawSurface();

	switch (mode) {
		case DRAWBOX_SOLID:
			{
				for (word y = 0; y < height; y++) {
					for (word x = 0; x < width; x++) {
						draw_surface[((coord_y + y) * VIDEO_WIDTH) + (coord_x + x)] = color;
					}
				}

			}
			break;

		case DRAWBOX_INVERSE:
			{
				for (word y = 0; y < height; y++) {
					for (word x = 0; x < width; x++) {
						draw_surface[((coord_y + y) * VIDEO_WIDTH) + (coord_x + x)] ^= color;
					}
				}
			}
			break;
	}
	
	if (isScreen(draw_surface)) {
		updateScreen(coord_x, coord_y, width, height);
	}
}


void Video::clearScreen()
{
	word coord_x0b = memory->b_SystemVariable->queryWord(iw_Video_CoordXb);
	word coord_y0 = memory->b_SystemVariable->queryWord(iw_Video_CoordY);
	word coord_x1b = memory->b_SystemVariable->queryWord(iw_Video_MaxCoordXb);
	word coord_y1 = memory->b_SystemVariable->queryWord(iw_Video_MaxCoordY);

	drawBox(DRAWBOX_SOLID, coord_x0b, coord_y0, coord_x1b, coord_y1);
}


void Video::blit(byte mode, word source_coord_x0b, word source_coord_y0, word source_coord_x1b, word source_coord_y1, byte source_type, word destination_coord_xb, word destination_coord_y, byte destination_type)
{
	word width = (source_coord_x1b - source_coord_x0b + 1) << 3;
	word height = source_coord_y1 - source_coord_y0 + 1;

	BlitStruct order;
	order.source.x = source_coord_x0b << 3;
	order.source.y = source_coord_y0;
	order.source.s = getSurface(source_type);
	order.destination.x = destination_coord_xb << 3;
	order.destination.y = destination_coord_y;
	order.destination.s = getSurface(destination_type);
	order.w = width;
	order.h = height;

	switch (mode) {
		case BLIT_SWAPPED:
			blitSwapped(&order);
			break;
		case BLIT_MASKED:
			blitMasked(&order);
			break;
		case BLIT_ERROR:
			//TODO: process error
			PRINT_ERROR("[Video::blit()] unknown blitting function\n");
			break;
		case BLIT_DIRECT:
		default:
			blitDirect(&order);
	}
}


void Video::blitDirect(BlitStruct *order)
{
	for (word y = 0; y < order->h; y++) {
		for (word x = 0; x < order->w; x++) {
			byte color = order->source.s[((order->source.y + y) * VIDEO_WIDTH) + (order->source.x + x)];
			order->destination.s[((order->destination.y + y) * VIDEO_WIDTH) + (order->destination.x + x)] = color;
		}
	}

	if (isScreen(order->destination.s)) {
		updateScreen(order->destination.x, order->destination.y, order->w, order->h);
	}
}


void Video::blitSwapped(BlitStruct *order)
{
	if (isScreen(order->source.s) || isScreen(order->destination.s)) {
		for (word y = 0; y < order->h; y++) {
			//HACK: is a meaning of width different here?
			//for (word x = 0; x < order->w; x++) {
			for (word x = 0; x < order->w - 8; x++) {
				byte source_color = order->source.s[((order->source.y + y) * VIDEO_WIDTH) + (order->source.x + x)];
				byte destination_color = order->destination.s[((order->destination.y + y) * VIDEO_WIDTH) + (order->destination.x + x)];

				order->source.s[((order->source.y + y) * VIDEO_WIDTH) + (order->source.x + x)] = destination_color;
				order->destination.s[((order->destination.y + y) * VIDEO_WIDTH) + (order->destination.x + x)] = source_color;
			}
		}

		if (isScreen(order->source.s)) {
			updateScreen(order->source.x, order->source.y, order->w, order->h);
		}
		else if (isScreen(order->destination.s)) {
			updateScreen(order->destination.x, order->destination.y, order->w, order->h);
		}
	}
	else {
		blitDirect(order);
	}
}


void Video::blitMasked(BlitStruct *order)
{
	for (word y = 0; y < order->h; y++) {
		for (word x = 0; x < order->w; x++) {
			byte color = order->source.s[((order->source.y + y) * VIDEO_WIDTH) + (order->source.x + x)];

			if (color != COLOR_KEY) {
				order->destination.s[((order->destination.y + y) * VIDEO_WIDTH) + (order->destination.x + x)] = color;
			}
		}
	}

	if (isScreen(order->destination.s)) {
		updateScreen(order->destination.x, order->destination.y, order->w, order->h);
	}
}


void Video::blitMerged(byte mode, word foreground_coord_x, word foreground_coord_y, word background_coord_x, word background_coord_y, word destination_coord_x, word destination_coord_y, word width, word height)
{
	byte foreground_type = (mode >> 1) & 1;
	byte background_type = mode & 1;
	byte destination_type = (mode >> 2) & 1 ;

	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			byte foreground_color = getPoint(foreground_type, foreground_coord_x + x, foreground_coord_y + y);
			byte background_color = getPoint(background_type, background_coord_x + x, background_coord_y + y);
			byte color;

			if (foreground_color != COLOR_KEY) {
				color = foreground_color;
			}
			else {
				color = background_color;
			}

			putPoint(destination_type, destination_coord_x + x, destination_coord_y + y, color);
		}
	}

	if (isScreen(destination_type)) {
		updateScreen(destination_coord_x, destination_coord_y, width, height);
	}
}


void Video::putSprite(word coord_x, word coord_y, word background_layer, word foreground_layer_1st, word foreground_layer_2nd, word foreground_layer_3rd)
{
	word background_coord_x = (word) ((background_layer & SPRITE_LAYER_MASK) % (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word background_coord_y = (word) ((background_layer & SPRITE_LAYER_MASK) / (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word foreground1_coord_x = (word) ((foreground_layer_1st & SPRITE_LAYER_MASK) % (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word foreground1_coord_y = (word) ((foreground_layer_1st & SPRITE_LAYER_MASK) / (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word foreground2_coord_x = (word) ((foreground_layer_2nd & SPRITE_LAYER_MASK) % (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word foreground2_coord_y = (word) ((foreground_layer_2nd & SPRITE_LAYER_MASK) / (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word foreground3_coord_x = (word) ((foreground_layer_3rd & SPRITE_LAYER_MASK) % (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;
	word foreground3_coord_y = (word) ((foreground_layer_3rd & SPRITE_LAYER_MASK) / (VIDEO_WIDTH / SPRITE_SIZE)) * SPRITE_SIZE;

	for (word y = 0; y < SPRITE_SIZE; y++) {
		for (word x = 0; x < SPRITE_SIZE; x++) {
			byte background_color = getPoint(SURFACE_BUFFER2, background_coord_x + x, background_coord_y + y);

			byte foreground1_color;
			byte foreground2_color;
			byte foreground3_color;
			if (option->game_type == GAME_NANPA2) {
				foreground1_color = getPoint(SURFACE_BUFFER3, foreground1_coord_x + x, foreground1_coord_y + y);
				foreground2_color = getPoint(SURFACE_BUFFER3, foreground2_coord_x + x, foreground2_coord_y + y);
				foreground3_color = getPoint(SURFACE_BUFFER3, foreground3_coord_x + x, foreground3_coord_y + y);
			}
			else {
				foreground1_color = getPoint(SURFACE_BUFFER2, foreground1_coord_x + x, foreground1_coord_y + y);
				foreground2_color = getPoint(SURFACE_BUFFER2, foreground2_coord_x + x, foreground2_coord_y + y);
				foreground3_color = getPoint(SURFACE_BUFFER2, foreground3_coord_x + x, foreground3_coord_y + y);
			}

			byte color;
			if (background_color >= COLOR_KEY) {
				color = background_color & SPRITE_COLOR_MASK;
			}
			else if ((foreground_layer_3rd != NULL) && (foreground3_color != COLOR_KEY)) {
				color = foreground3_color & SPRITE_COLOR_MASK;
			}
			else if ((foreground_layer_2nd != NULL) && (foreground2_color != COLOR_KEY)) {
				color = foreground2_color & SPRITE_COLOR_MASK;
			}
			else if ((foreground_layer_1st != NULL) && (foreground1_color != COLOR_KEY)) {
				color = foreground1_color & SPRITE_COLOR_MASK;
			}
			else if (background_color != COLOR_KEY) {
				color = background_color & SPRITE_COLOR_MASK;
			}
			else {
				continue;
			}

			putPoint(coord_x + x, coord_y + y, color);
		}
	}

	//TODO: which one is better?
	//updateScreen(coord_x, coord_y, SPRITE_SIZE, SPRITE_SIZE);
}


void Video::putPoint(byte surface_type, word coord_x, word coord_y, byte color_index)
{
	if ((coord_x >= 0 && coord_x < VIDEO_WIDTH) && (coord_y >= 0 && coord_y < VIDEO_HEIGHT)) {
		byte *surface = getSurface(surface_type);
		surface[(coord_y * VIDEO_WIDTH) + coord_x] = color_index;
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::putPoint()] out of bound\n");
		PAUSE;
	}
}


inline void Video::putPoint(word coord_x, word coord_y, byte color_index)
{
	putPoint(SURFACE_SCREEN, coord_x, coord_y, color_index);
}


byte Video::getPoint(byte surface_type, word coord_x, word coord_y)
{
	if ((coord_x >= 0 && coord_x < VIDEO_WIDTH) && (coord_y >= 0 && coord_y < VIDEO_HEIGHT)) {
		byte *surface = getSurface(surface_type);
		return surface[(coord_y * VIDEO_WIDTH) + coord_x];
	}
	else {
		return COLOR_NONE;
	}
}


inline byte Video::getPoint(word coord_x, word coord_y)
{
	return getPoint(SURFACE_SCREEN, coord_x, coord_y);
}


void Video::lockScreen(SDL_Surface *surface)
{
	if (SDL_MUSTLOCK(surface)) {
		if (SDL_LockSurface(surface) < 0) {
			//TODO: process error
			PRINT_ERROR("[Video::lockScreen()] unable to lock surface\n");
			exit(1);
		}
	}
}


inline void Video::lockScreen()
{
	lockScreen(sdl_screen);
}


void Video::unlockScreen(SDL_Surface *surface)
{
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}


inline void Video::unlockScreen()
{
	unlockScreen(sdl_screen);
}


void Video::drawPixel(SDL_Surface *sdl_surface, int x, int y, Uint32 sdl_color)
{
	if ((x >= 0) && (x < VIDEO_WIDTH) && (y >= 0) && (y < VIDEO_HEIGHT)) {
		switch (sdl_surface->format->BytesPerPixel) {
			case 1:
				{
					Uint8 *buffer;
					buffer = ((Uint8*) sdl_surface->pixels) + (y * sdl_surface->pitch) + x;
					*buffer = sdl_color;
				}
				break;
			
			case 2:
				{
					Uint16 *buffer;
					buffer = ((Uint16*) sdl_surface->pixels) + (y * sdl_surface->pitch / 2) + x;
					*buffer = sdl_color;
				}
				break;
			
			case 3:
				{
					Uint8 *buffer;
					buffer = ((Uint8*) sdl_surface->pixels) + (y * sdl_surface->pitch) + (x * 3);
					if (SDL_BYTEORDER == SDL_LIL_ENDIAN) {
						buffer[0] = sdl_color;
						buffer[1] = sdl_color >> 8;
						buffer[2] = sdl_color >> 16;
					}
					else {
						buffer[2] = sdl_color;
						buffer[1] = sdl_color >> 8;
						buffer[0] = sdl_color >> 16;
					}
				}
				break;
			
			case 4:
				{
					Uint32 *buffer;
					buffer = ((Uint32*) sdl_surface->pixels) + (y * sdl_surface->pitch / 4) + x;
					//TODO: check this
					//buffer = ((Uint32*) sdl_surface->pixels) + (y * sdl_surface->pitch) + (x << 2);
					*buffer = sdl_color;
				}
				break;
		}
	}
	else {
		PRINT("[Video::drawPixel()] out of bound\n");
		PAUSE;
	}
}


void Video::drawFont(word coord_x, word coord_y, const byte *font, long int offset, word width, word height)
{
	word length = (width / FONT_BPB) * height;
	MemoryBlock *b_Font = new MemoryBlock(0, length);
	b_Font->writeString(0, (char*) (font + offset), length);
	b_Font->set(0, BIT_FLOW_LEFT);

	byte color = (byte) memory->b_SystemVariable->queryWord(iw_Video_Color);
	byte foreground_color = color & COLOR_MASK;
	byte background_color = color >> 4;

	byte *surface = getDrawSurface();

	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			switch (b_Font->readBit()) {
				case FONT_BACKGROUND:
					surface[((coord_y + y) * VIDEO_WIDTH) + (coord_x + x)] = background_color;
					break;
				case FONT_FOREGROUND:
					surface[((coord_y + y) * VIDEO_WIDTH) + (coord_x + x)] = foreground_color;
					break;
			}
		}
	}

	//HACK: make bold
	b_Font->set(0, BIT_FLOW_LEFT);
	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			switch (b_Font->readBit()) {
				case FONT_FOREGROUND:
					if (x != (width - 1)) {
						surface[((coord_y + y) * VIDEO_WIDTH) + (coord_x + x + 1)] = foreground_color;
					}
					break;
			}
		}
	}
	
	delete b_Font;

	if (isScreen(surface)) {
		updateScreen(coord_x, coord_y, width, height);
	}
}


void Video::filter(word coord_x, word coord_y, word width, word height)
{
	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			Uint32 sdl_color = getColor(getPoint(coord_x + x, coord_y + y));;
/*
			if (sdl_color == 0x00000000) {
				drawPixel(sdl_screen, (coord_x + x), (coord_y + y), sdl_color);
			}
			else {
*/
				Uint8 color_red, color_green, color_blue;
				int color_red_sum = 0;
				int color_green_sum = 0;
				int color_blue_sum = 0;
				int count = 0;

				for (int dy = -FILTER_RADIUS; dy <= FILTER_RADIUS; dy++) {
					for (int dx = -FILTER_RADIUS; dx <= FILTER_RADIUS; dx++) {
						word color = getPoint(coord_x + x + dx, coord_y + y + dy);
						if (color != COLOR_NONE) {
							splitColor(color, &color_red, &color_green, &color_blue);

							color_red_sum += color_red;
							color_green_sum += color_green;
							color_blue_sum += color_blue;

							count++;
						}
					}
				}

				color_red = (int) (color_red_sum / count);
				color_green = (int) (color_green_sum / count);
				color_blue = (int) (color_blue_sum / count);

				sdl_color = SDL_MapRGB(sdl_screen->format, color_red, color_green, color_blue);
				drawPixel(sdl_screen, (coord_x + x), (coord_y + y), sdl_color);
//			}
		}
	}
}


void Video::dump()
{
	SDL_SaveBMP(sdl_screen, "screen.bmp");

	for (int i = 0; i < VIDEO_BUFFER; i++) {
		SDL_Surface *sdl_buffer = SDL_CreateRGBSurface(SDL_HWSURFACE, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, 0, 0, 0, 0);

		for (word y = 0; y < VIDEO_HEIGHT; y++) {
			for (word x = 0; x < VIDEO_WIDTH; x++) {
				Uint32 sdl_color = convertColor(intermediate_palette[buffer[i][(y * VIDEO_WIDTH) + x]]);
				drawPixel(sdl_buffer, x, y, sdl_color);
			}
		}

		char dump_filename[] = "buffer0.bmp";
		dump_filename[6] = (char) (i + '0');

		SDL_SaveBMP(sdl_buffer, dump_filename);

		SDL_FreeSurface(sdl_buffer);
	}
}
