#include "video.h"

Video::Video(Memory *memory, Timer *timer, Option *option)
{
	this->memory = memory;
	this->timer = timer;
	this->option = option;

	if (option->is_fullscreen) {
		sdl_screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, SDL_SWSURFACE | SDL_FULLSCREEN);
	}
	else {
		sdl_screen = SDL_SetVideoMode(VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, SDL_SWSURFACE);
	}

	if (sdl_screen == NULL) {
		PRINT_ERROR("[Video::Video()] unable to set %dx%dx%d video mode: %s\n", VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, SDL_GetError());
	}

	const SDL_VideoInfo *video_info = SDL_GetVideoInfo();
	color_red_mask = video_info->vfmt->Rmask;
	color_green_mask = video_info->vfmt->Gmask;
	color_blue_mask = video_info->vfmt->Bmask;
	color_alpha_mask = video_info->vfmt->Amask;

#ifdef FIELD_EXPERIMENT
	sdl_map = NULL;
	map_width = 0;
	map_height = 0;

	for (int i = 0; i < CHARACTER_TOTAL; i++) {
		for (int j = 0; j < 2; j++) {
			character[i][j] = NULL;
		}
	}

	has_character_moved = false;
#endif

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

#ifdef FIELD_EXPERIMENT
	if (sdl_map != NULL) {
		SDL_FreeSurface(sdl_map);
	}

	for (int i = 0; i < CHARACTER_TOTAL; i++) {
		for (int j = 0; j < 2; j++) {
			if (character[i][j] != NULL) {
				delete character[i][j];
			}
		}
	}
#endif
}


void Video::setColor(byte color_index, word color)
{
	if (color_index < VIDEO_COLOR) {
		sdl_palette[color_index] = convertColor(color);
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::setColor()] out of bound: color_index = %d, color = %d\n", color_index, color);
	}

	//updateScreen();
}


Uint32 Video::getColor(byte color_index)
{
	if (color_index < VIDEO_COLOR) {
		return sdl_palette[color_index];
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::getColor()] out of bound: color_index = %d\n", color_index);
		return 0;
	}
}


Uint32 Video::convertColor(word color)
{
	Uint8 color_red, color_green, color_blue;
	splitColor(color, &color_red, &color_green, &color_blue);

	return SDL_MapRGB(sdl_screen->format, color_red, color_green, color_blue);
}


void Video::splitColor(word color, Uint8 *color_red, Uint8 *color_green, Uint8 *color_blue)
{
	*color_red = (Uint8) (((color >> 4) & COLOR_MASK) * 0x11);
	*color_green = (Uint8) (((color >> 8) & COLOR_MASK) * 0x11);
	*color_blue = (Uint8) (((color >> 0) & COLOR_MASK) * 0x11);
}


void Video::setIntermediateColor(byte color_index, word color)
{
	if (color_index < VIDEO_COLOR) {
		intermediate_palette[color_index] = color;
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::setIntermediateColor()] out of bound: color_index = %d, color = %d\n", color_index, color);
	}
}


word Video::getIntermediateColor(byte color_index)
{
	if (color_index < VIDEO_COLOR) {
		return intermediate_palette[color_index];
	}
	else {
		//TODO: process error
		PRINT_ERROR("[Video::getIntermediateColor()] out of bound: color_index = %d\n", color_index);
		return COLOR_NONE;
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


byte* Video::getSurface(SurfaceType surface_type)
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
#ifdef FIELD_EXPERIMENT
		case SURFACE_MAP:
			surface = map;
			break;
#endif
	}

	return surface;
}


SurfaceType Video::getDrawSurface()
{
	byte surface_type = (byte) (memory->b_SystemVariable->queryWord(iw_DisplayBuffer));

	return (SurfaceType) surface_type;
}


bool Video::isScreen(byte *surface)
{
	return (surface == screen);
}


bool Video::isScreen(SurfaceType surface_type)
{
	return (surface_type == SURFACE_SCREEN);
}


void Video::updateScreen(word coord_x, word coord_y, word width, word height)
{
//Uint32 start_ticks = SDL_GetTicks();

	if (option->is_filter) {
		if (coord_x >= FILTER_RADIUS) {
			coord_x -= FILTER_RADIUS;
		}
		if (((coord_x + width) + FILTER_RADIUS) < VIDEO_WIDTH) {
			width += FILTER_RADIUS;
		}
	}

	lockScreen();
	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			//TODO: change or not?
			//Uint32 sdl_color = getColor(getPoint(coord_x + x, coord_y + y));
			Uint32 sdl_color = getFilteredColor(coord_x + x, coord_y + y);
			drawPixel(sdl_screen, (coord_x + x), (coord_y + y), sdl_color);
		}
	}
	unlockScreen();

//Uint32 draw_ticks = SDL_GetTicks();

	SDL_UpdateRect(sdl_screen, coord_x, coord_y, width, height);

//Uint32 update_ticks = SDL_GetTicks();

//PRINT("[Video::updateScreen()] draw ticks = %d, update ticks = %d\n", (draw_ticks - start_ticks), (update_ticks - draw_ticks));
}


void Video::updateScreen()
{
	updateScreen(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
}


void Video::fadeScreen()
{
	for (int i = 0; i < VIDEO_COLOR; i++) {
		setColor(i, intermediate_palette[i]);
	}

	SDL_Surface *old_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, color_red_mask, color_green_mask, color_blue_mask, color_alpha_mask);
	SDL_Surface *new_screen = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, color_red_mask, color_green_mask, color_blue_mask, color_alpha_mask);

	SDL_BlitSurface(sdl_screen, NULL, old_screen, NULL);

	lockScreen(new_screen);
	for (word y = 0; y < VIDEO_HEIGHT; y++) {
		for (word x = 0; x < VIDEO_WIDTH; x++) {
			//TODO: change or not?
			//Uint32 sdl_color = getColor(getPoint(x, y));
			Uint32 sdl_color = getFilteredColor(x, y);
			drawPixel(new_screen, x, y, sdl_color);
		}
	}
	unlockScreen(new_screen);

	int fade_interval = (int) (SDL_ALPHA_OPAQUE / FADE_LEVEL);
	for (int i = SDL_ALPHA_OPAQUE - (fade_interval * FADE_LEVEL); i <= SDL_ALPHA_OPAQUE; i += fade_interval) {
		SDL_SetAlpha(new_screen, SDL_SRCALPHA, i);

		SDL_BlitSurface(old_screen, NULL, sdl_screen, NULL);
		SDL_BlitSurface(new_screen, NULL, sdl_screen, NULL);

		SDL_UpdateRect(sdl_screen, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);
	}

	SDL_FreeSurface(old_screen);
	SDL_FreeSurface(new_screen);
}


void Video::initializeOverlapScreen()
{
	if (overlap_old_screen != NULL) {
		SDL_FreeSurface(overlap_old_screen);
	}
	overlap_old_screen = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, color_red_mask, color_green_mask, color_blue_mask, color_alpha_mask);

	if (overlap_new_screen != NULL) {
		SDL_FreeSurface(overlap_new_screen);
	}
	overlap_new_screen = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, color_red_mask, color_green_mask, color_blue_mask, color_alpha_mask);

	SDL_BlitSurface(sdl_screen, NULL, overlap_old_screen, NULL);

	setIntermediatePalette();
	for (int i = 0; i < VIDEO_COLOR; i++) {
		setColor(i, intermediate_palette[i]);
	}

	lockScreen(overlap_new_screen);
	for (word y = 0; y < VIDEO_HEIGHT; y++) {
		for (word x = 0; x < VIDEO_WIDTH; x++) {
			//TODO: change or not?
			//Uint32 sdl_color = getColor(getPoint(x, y));
			Uint32 sdl_color = getFilteredColor(x, y);
			drawPixel(overlap_new_screen, x, y, sdl_color);
		}
	}
	unlockScreen(overlap_new_screen);

	overlap_inuse = true;
}


void Video::overlapScreen()
{
	if (overlap_inuse) {
		if (timer->checkOverlapTimer() < overlap_delay) {
			return;
		}
		else {
			timer->resetOverlapTimer();
		}

		int overlap_interval = (int) (SDL_ALPHA_OPAQUE / overlap_level);
		int overlap_initial = SDL_ALPHA_OPAQUE - (overlap_interval * overlap_level);
		SDL_SetAlpha(overlap_new_screen, SDL_SRCALPHA, overlap_initial + (overlap_interval * overlap_current_level));

		SDL_BlitSurface(overlap_old_screen, NULL, sdl_screen, NULL);
		SDL_BlitSurface(overlap_new_screen, NULL, sdl_screen, NULL);

		SDL_UpdateRect(sdl_screen, 0, 0, VIDEO_WIDTH, VIDEO_HEIGHT);

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
}


void Video::drawBox(byte mode, word coord_x0b, word coord_y0, word coord_x1b, word coord_y1)
{
	word coord_x = coord_x0b << 3;
	word coord_y = coord_y0;
	word width = (coord_x1b - coord_x0b + 1) << 3;
	word height = coord_y1 - coord_y0 + 1;

	byte color = (byte) memory->b_SystemVariable->queryByte(iw_Video_Color);
	color = color >> 4;

	SurfaceType surface_type = getDrawSurface();

	switch (mode) {
		case DRAWBOX_SOLID:
			{
				for (word y = 0; y < height; y++) {
					for (word x = 0; x < width; x++) {
						putPoint(coord_x + x, coord_y + y, color, surface_type);
					}
				}

			}
			break;

		case DRAWBOX_INVERSE:
			{
				for (word y = 0; y < height; y++) {
					for (word x = 0; x < width; x++) {
						byte original_color = getPoint(coord_x + x, coord_y + y, surface_type);
						putPoint(coord_x + x, coord_y + y, original_color ^ color, surface_type);
					}
				}
			}
			break;
	}
	
	if (isScreen(surface_type)) {
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
	order.source.surface_type = (SurfaceType) source_type;
	order.destination.x = destination_coord_xb << 3;
	order.destination.y = destination_coord_y;
	order.destination.surface_type = (SurfaceType) destination_type;
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
			byte color = getPoint(order->source.x + x, order->source.y + y, order->source.surface_type);
			putPoint(order->destination.x + x, order->destination.y + y, color, order->destination.surface_type);
		}
	}

	if (isScreen(order->destination.surface_type)) {
		updateScreen(order->destination.x, order->destination.y, order->w, order->h);
	}
}


void Video::blitSwapped(BlitStruct *order)
{
	if (isScreen(order->source.surface_type) || isScreen(order->destination.surface_type)) {
		for (word y = 0; y < order->h; y++) {
			//HACK: is a meaning of width different here?
			//for (word x = 0; x < order->w; x++) {
			for (word x = 0; x < order->w - 8; x++) {
				byte source_color = getPoint(order->source.x + x, order->source.y + y, order->source.surface_type);
				byte destination_color = getPoint(order->destination.x + x, order->destination.y + y, order->destination.surface_type);

				putPoint(order->source.x + x, order->source.y + y, destination_color, order->source.surface_type);
				putPoint(order->destination.x + x, order->destination.y + y, source_color, order->destination.surface_type);
			}
		}

		if (isScreen(order->source.surface_type)) {
			updateScreen(order->source.x, order->source.y, order->w, order->h);
		}
		else if (isScreen(order->destination.surface_type)) {
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
			byte color = getPoint(order->source.x + x, order->source.y + y, order->source.surface_type);

			if (color != COLOR_KEY) {
				putPoint(order->destination.x + x, order->destination.y + y, color, order->destination.surface_type);
			}
		}
	}

	if (isScreen(order->destination.surface_type)) {
		updateScreen(order->destination.x, order->destination.y, order->w, order->h);
	}
}


void Video::blitMerged(byte mode, word foreground_coord_x, word foreground_coord_y, word background_coord_x, word background_coord_y, word destination_coord_x, word destination_coord_y, word width, word height)
{
	SurfaceType foreground_type = (SurfaceType) ((mode >> 1) & 1);
	SurfaceType background_type = (SurfaceType) (mode & 1);
	SurfaceType destination_type = (SurfaceType) ((mode >> 2) & 1);

	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			byte foreground_color = getPoint(foreground_coord_x + x, foreground_coord_y + y, foreground_type);
			byte background_color = getPoint(background_coord_x + x, background_coord_y + y, background_type);
			byte color;

			if (foreground_color != COLOR_KEY) {
				color = foreground_color;
			}
			else {
				color = background_color;
			}

			putPoint(destination_coord_x + x, destination_coord_y + y, color, destination_type);
		}
	}

	if (isScreen(destination_type)) {
		updateScreen(destination_coord_x, destination_coord_y, width, height);
	}
}


#ifdef FIELD_EXPERIMENT
void Video::initializeMap(word width, word height)
{
	if (sdl_map != NULL) {
		SDL_FreeSurface(sdl_map);
	}
	if (map != NULL) {
		delete map;
	}

	map_width = width;
	map_height = height;

	sdl_map = SDL_CreateRGBSurface(SDL_SWSURFACE, map_width, map_height, VIDEO_COLOR_DEPTH, color_red_mask, color_green_mask, color_blue_mask, color_alpha_mask);
	map = new byte[map_width * map_height];
}


void Video::createMap()
{
	lockScreen(sdl_map);
	for (int y = 0; y < map_height; y++) {
		for (int x = 0; x < map_width; x++) {
			Uint32 sdl_color = getFilteredColor(x, y, SURFACE_MAP);
			drawPixel(sdl_map, x, y, sdl_color);
		}
	}
	unlockScreen(sdl_map);
}


void Video::drawMap(SDL_Rect *map_region, SDL_Rect *screen_region)
{
	SDL_BlitSurface(sdl_map, map_region, sdl_screen, screen_region);
	SDL_UpdateRect(sdl_screen, screen_region->x, screen_region->y, screen_region->w, screen_region->h);
}


void Video::updateCharacter(int index, CharacterSprite *new_character)
{
	if ((character[index][SPRITE_PREVIOUS] != NULL) && (character[index][SPRITE_CURRENT] != NULL)) {
		word previous_coord_xw = character[index][SPRITE_PREVIOUS]->coord_xw;
		word previous_coord_yw = character[index][SPRITE_PREVIOUS]->coord_yw;

		if ((previous_coord_xw != 0) && (previous_coord_yw != 0)) {
			if (character[index][SPRITE_CURRENT]->compare(new_character)) {
				return;
			}
		}
	}

    has_character_moved = true;

	if (character[index][SPRITE_PREVIOUS] != NULL) {
		delete character[index][SPRITE_PREVIOUS];
	}
	character[index][SPRITE_PREVIOUS] = character[index][SPRITE_CURRENT];

	character[index][SPRITE_CURRENT] = new_character;
}


void Video::drawCharacter(word view_coord_xw, word view_coord_yw, word view_margin_xw, word view_margin_y, bool is_forced)
{
	if (has_character_moved || is_forced) {
		for (int i = 0; i < CHARACTER_TOTAL; i++) {
			// recover character's previous region if it exists
			if (character[i][SPRITE_PREVIOUS] != NULL) {
				word widthw = character[i][SPRITE_PREVIOUS]->widthw;
				word heightw = character[i][SPRITE_PREVIOUS]->heightw;

				word coord_x = ((character[i][SPRITE_PREVIOUS]->coord_xw - view_coord_xw) + view_margin_xw) * SPRITE_SIZE;
				word coord_y = ((character[i][SPRITE_PREVIOUS]->coord_yw - view_coord_yw) * SPRITE_SIZE) + view_margin_y;

				for (word yw = 0; yw < heightw; yw++) {
					if (option->is_filter) {
						word left_background_layer = character[i][SPRITE_PREVIOUS]->neighbour_background_layer[(yw * 2) + SPRITE_LEFT];
						word left_foreground_layer_1st = character[i][SPRITE_PREVIOUS]->neighbour_foreground_layer_1st[(yw * 2) + SPRITE_LEFT];
						word left_foreground_layer_2nd = character[i][SPRITE_PREVIOUS]->neighbour_foreground_layer_2nd[(yw * 2) + SPRITE_LEFT];
						word left_foreground_layer_3rd = character[i][SPRITE_PREVIOUS]->neighbour_foreground_layer_3rd[(yw * 2) + SPRITE_LEFT];

						word right_background_layer = character[i][SPRITE_PREVIOUS]->neighbour_background_layer[(yw * 2) + SPRITE_RIGHT];
						word right_foreground_layer_1st = character[i][SPRITE_PREVIOUS]->neighbour_foreground_layer_1st[(yw * 2) + SPRITE_RIGHT];
						word right_foreground_layer_2nd = character[i][SPRITE_PREVIOUS]->neighbour_foreground_layer_2nd[(yw * 2) + SPRITE_RIGHT];
						word right_foreground_layer_3rd = character[i][SPRITE_PREVIOUS]->neighbour_foreground_layer_3rd[(yw * 2) + SPRITE_RIGHT];

						putSprite(coord_x - SPRITE_SIZE, coord_y + (yw * SPRITE_SIZE), left_background_layer, left_foreground_layer_1st, left_foreground_layer_2nd, left_foreground_layer_3rd);
						putSprite(coord_x + (widthw * SPRITE_SIZE), coord_y + (yw * SPRITE_SIZE), right_background_layer, right_foreground_layer_1st, right_foreground_layer_2nd, right_foreground_layer_3rd);
					}

					for (word xw = 0; xw < widthw; xw++) {
						word background_layer = character[i][SPRITE_PREVIOUS]->background_layer[(yw * widthw) + xw];
						word foreground_layer_1st = character[i][SPRITE_PREVIOUS]->foreground_layer_1st[(yw * widthw) + xw];
						word foreground_layer_2nd = character[i][SPRITE_PREVIOUS]->foreground_layer_2nd[(yw * widthw) + xw];
						word foreground_layer_3rd = character[i][SPRITE_PREVIOUS]->foreground_layer_3rd[(yw * widthw) + xw];

						if (foreground_layer_3rd != 0) {
							foreground_layer_3rd = 0;
						}
						else if (foreground_layer_2nd != 0) {
							foreground_layer_2nd = 0;
						}
						else if (foreground_layer_1st != 0) {
							foreground_layer_1st = 0;
						}

						putSprite(coord_x + (xw * SPRITE_SIZE), coord_y + (yw * SPRITE_SIZE), background_layer, foreground_layer_1st, foreground_layer_2nd, foreground_layer_3rd);
						updateScreen(coord_x + (xw * SPRITE_SIZE), coord_y + (yw * SPRITE_SIZE), SPRITE_SIZE, SPRITE_SIZE);
					}
				}
			}

			// draw character at current region
			if (character[i][SPRITE_CURRENT] != NULL) {
				word widthw = character[i][SPRITE_CURRENT]->widthw;
				word heightw = character[i][SPRITE_CURRENT]->heightw;

				word coord_x = ((character[i][SPRITE_CURRENT]->coord_xw - view_coord_xw) + view_margin_xw) * SPRITE_SIZE;
				word coord_y = ((character[i][SPRITE_CURRENT]->coord_yw - view_coord_yw) * SPRITE_SIZE) + view_margin_y;

				for (word yw = 0; yw < heightw; yw++) {
					if (option->is_filter) {
						word left_background_layer = character[i][SPRITE_CURRENT]->neighbour_background_layer[(yw * 2) + SPRITE_LEFT];
						word left_foreground_layer_1st = character[i][SPRITE_CURRENT]->neighbour_foreground_layer_1st[(yw * 2) + SPRITE_LEFT];
						word left_foreground_layer_2nd = character[i][SPRITE_CURRENT]->neighbour_foreground_layer_2nd[(yw * 2) + SPRITE_LEFT];
						word left_foreground_layer_3rd = character[i][SPRITE_CURRENT]->neighbour_foreground_layer_3rd[(yw * 2) + SPRITE_LEFT];

						word right_background_layer = character[i][SPRITE_CURRENT]->neighbour_background_layer[(yw * 2) + SPRITE_RIGHT];
						word right_foreground_layer_1st = character[i][SPRITE_CURRENT]->neighbour_foreground_layer_1st[(yw * 2) + SPRITE_RIGHT];
						word right_foreground_layer_2nd = character[i][SPRITE_CURRENT]->neighbour_foreground_layer_2nd[(yw * 2) + SPRITE_RIGHT];
						word right_foreground_layer_3rd = character[i][SPRITE_CURRENT]->neighbour_foreground_layer_3rd[(yw * 2) + SPRITE_RIGHT];

						putSprite(coord_x - SPRITE_SIZE, coord_y + (yw * SPRITE_SIZE), left_background_layer, left_foreground_layer_1st, left_foreground_layer_2nd, left_foreground_layer_3rd);
						putSprite(coord_x + (widthw * SPRITE_SIZE), coord_y + (yw * SPRITE_SIZE), right_background_layer, right_foreground_layer_1st, right_foreground_layer_2nd, right_foreground_layer_3rd);
					}

					for (word xw = 0; xw < widthw; xw++) {
						word background_layer = character[i][SPRITE_CURRENT]->background_layer[(yw * widthw) + xw];
						word foreground_layer_1st = character[i][SPRITE_CURRENT]->foreground_layer_1st[(yw * widthw) + xw];
						word foreground_layer_2nd = character[i][SPRITE_CURRENT]->foreground_layer_2nd[(yw * widthw) + xw];
						word foreground_layer_3rd = character[i][SPRITE_CURRENT]->foreground_layer_3rd[(yw * widthw) + xw];

						putSprite(coord_x + (xw * SPRITE_SIZE), coord_y + (yw * SPRITE_SIZE), background_layer, foreground_layer_1st, foreground_layer_2nd, foreground_layer_3rd);
						updateScreen(coord_x + (xw * SPRITE_SIZE), coord_y +(yw * SPRITE_SIZE), SPRITE_SIZE, SPRITE_SIZE);
					}
				}
			}
		}

		has_character_moved = false;
	}
}
#endif


void Video::putSprite(word coord_x, word coord_y, word background_layer, word foreground_layer_1st, word foreground_layer_2nd, word foreground_layer_3rd, SurfaceType surface_type)
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
			byte background_color = getPoint(background_coord_x + x, background_coord_y + y, SURFACE_BUFFER2);

			byte foreground1_color;
			byte foreground2_color;
			byte foreground3_color;
			if (option->game_type == GAME_NANPA2) {
				foreground1_color = getPoint(foreground1_coord_x + x, foreground1_coord_y + y, SURFACE_BUFFER3);
				foreground2_color = getPoint(foreground2_coord_x + x, foreground2_coord_y + y, SURFACE_BUFFER3);
				foreground3_color = getPoint(foreground3_coord_x + x, foreground3_coord_y + y, SURFACE_BUFFER3);
			}
			else {
				foreground1_color = getPoint(foreground1_coord_x + x, foreground1_coord_y + y, SURFACE_BUFFER2);
				foreground2_color = getPoint(foreground2_coord_x + x, foreground2_coord_y + y, SURFACE_BUFFER2);
				foreground3_color = getPoint(foreground3_coord_x + x, foreground3_coord_y + y, SURFACE_BUFFER2);
			}

			byte color;
			if (background_color >= COLOR_KEY) {
				color = background_color & SPRITE_COLOR_MASK;
			}
			else if ((foreground_layer_3rd != 0) && (foreground3_color != COLOR_KEY)) {
				color = foreground3_color & SPRITE_COLOR_MASK;
			}
			else if ((foreground_layer_2nd != 0) && (foreground2_color != COLOR_KEY)) {
				color = foreground2_color & SPRITE_COLOR_MASK;
			}
			else if ((foreground_layer_1st != 0) && (foreground1_color != COLOR_KEY)) {
				color = foreground1_color & SPRITE_COLOR_MASK;
			}
			else if (background_color != COLOR_KEY) {
				color = background_color & SPRITE_COLOR_MASK;
			}
			else {
				continue;
			}

			putPoint(coord_x + x, coord_y + y, color, surface_type);
		}
	}

	//TODO: which one is better?
	//updateScreen(coord_x, coord_y, SPRITE_SIZE, SPRITE_SIZE);
}


void Video::putPoint(word coord_x, word coord_y, byte color_index, SurfaceType surface_type)
{
#ifdef FIELD_EXPERIMENT
	word max_coord_x;
	word max_coord_y;

	if (surface_type == SURFACE_MAP) {
		max_coord_x = map_width;
		max_coord_y = map_height;
	}
	else {
		max_coord_x = VIDEO_WIDTH;
		max_coord_y = VIDEO_HEIGHT;
	}

	if (((coord_x >= 0) && (coord_x < max_coord_x)) && ((coord_y >= 0) && (coord_y < max_coord_y))) {
		byte *surface = getSurface(surface_type);
		surface[(coord_y * max_coord_x) + coord_x] = color_index;
	}
#else
	if (((coord_x >= 0) && (coord_x < VIDEO_WIDTH)) && ((coord_y >= 0) && (coord_y < VIDEO_HEIGHT))) {
		byte *surface = getSurface(surface_type);
		surface[(coord_y * VIDEO_WIDTH) + coord_x] = color_index;
	}
#endif
	else {
		//TODO: process error
		PRINT_ERROR("[Video::putPoint()] out of bound: st = %d, x = %d, y = %d, c = %d\n", surface_type, coord_x, coord_y, color_index);
	}
}


byte Video::getPoint(word coord_x, word coord_y, SurfaceType surface_type)
{
#ifdef FIELD_EXPERIMENT
	word max_coord_x;
	word max_coord_y;
	if (surface_type == SURFACE_MAP) {
		max_coord_x = map_width;
		max_coord_y = map_height;
	}
	else {
		max_coord_x = VIDEO_WIDTH;
		max_coord_y = VIDEO_HEIGHT;
	}

	if (((coord_x >= 0) && (coord_x < max_coord_x)) && ((coord_y >= 0) && (coord_y < max_coord_y))) {
		byte *surface = getSurface(surface_type);
		return surface[(coord_y * max_coord_x) + coord_x];
	}
#else
	if (((coord_x >= 0) && (coord_x < VIDEO_WIDTH)) && ((coord_y >= 0) && (coord_y < VIDEO_HEIGHT))) {
		byte *surface = getSurface(surface_type);
		return surface[(coord_y * VIDEO_WIDTH) + coord_x];
	}
#endif
	else {
		PRINT_ERROR("[Video::getPoint()] out of bound: type = %d, coord_x = %d, coord_y = %d\n", surface_type, coord_x, coord_y);
		return COLOR_NONE;
	}
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


void Video::lockScreen()
{
	lockScreen(sdl_screen);
}


void Video::unlockScreen(SDL_Surface *surface)
{
	if (SDL_MUSTLOCK(surface)) {
		SDL_UnlockSurface(surface);
	}
}


void Video::unlockScreen()
{
	unlockScreen(sdl_screen);
}


void Video::drawPixel(SDL_Surface *sdl_surface, int x, int y, Uint32 sdl_color)
{
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
				//buffer = ((Uint32*) sdl_surface->pixels) + (y * sdl_surface->pitch / 4) + x;
				buffer = ((Uint32*) sdl_surface->pixels) + (y * sdl_surface->w) + x;
				*buffer = sdl_color;
			}
			break;
	}
}


Uint32 Video::getFilteredColor(word coord_x, word coord_y, SurfaceType surface_type)
{
	if (option->is_filter) {
#ifdef FIELD_EXPERIMENT
		word max_coord_x;
		if (surface_type == SURFACE_MAP) {
			max_coord_x = map_width;
		}
		else {
			max_coord_x = VIDEO_WIDTH;
		}
#endif

		int color_red_sum = 0;
		int color_green_sum = 0;
		int color_blue_sum = 0;

		int count = 0;
		for (int dx = -FILTER_RADIUS; dx <= FILTER_RADIUS; dx++) {
#ifdef FIELD_EXPERIMENT
			if (((coord_x + dx) >= 0) && ((coord_x + dx) < max_coord_x)) {
#else
			if (((coord_x + dx) >= 0) && ((coord_x + dx) < VIDEO_WIDTH)) {
#endif
				byte color_index = getPoint(coord_x + dx, coord_y, surface_type);
				Uint32 color = getColor(color_index);

				Uint8 color_red, color_green, color_blue;
				SDL_GetRGB(color, sdl_screen->format, &color_red, &color_green, &color_blue);

				//HACK: stress original color
				if (dx == 0) {
					color_red_sum += color_red * FILTER_PRESERVE;
					color_green_sum += color_green * FILTER_PRESERVE;
					color_blue_sum += color_blue * FILTER_PRESERVE;

					count += FILTER_PRESERVE;
				}
				else {
					color_red_sum += color_red;
					color_green_sum += color_green;
					color_blue_sum += color_blue;

					count++;
				}
			}
		}

		//HACK: make scanline
		if (option->is_scanline) {
			if ((coord_y % 2) == 0) {
				count++;
			}
		}

		Uint8 filtered_color_red = (Uint8) (color_red_sum / count);
		Uint8 filtered_color_green = (Uint8) (color_green_sum / count);
		Uint8 filtered_color_blue = (Uint8) (color_blue_sum / count);

		return SDL_MapRGB(sdl_screen->format, filtered_color_red, filtered_color_green, filtered_color_blue);
	}
	else {
		return getColor(getPoint(coord_x, coord_y, surface_type));
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

	SurfaceType surface_type = getDrawSurface();

	for (word y = 0; y < height; y++) {
		for (word x = 0; x < width; x++) {
			switch (b_Font->readBit()) {
				case FONT_BACKGROUND:
					putPoint(coord_x + x, coord_y + y, background_color, surface_type);
					break;
				case FONT_FOREGROUND:
					putPoint(coord_x + x, coord_y + y, foreground_color, surface_type);
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
						putPoint((coord_x + x) + 1, coord_y + y, foreground_color, surface_type);
					}
					break;
			}
		}
	}
	
	delete b_Font;

	if (isScreen(surface_type)) {
		updateScreen(coord_x, coord_y, width, height);
	}
}


void Video::capture()
{
	string screen_str = "scrn";
	string buffer_str = "buf";
	string extension_str = ".bmp";

	string screen_name;
	string buffer_name[VIDEO_BUFFER];

	int count = 0;

	FILE *handle = NULL;
	while (true) {
		char count_c_str[5];
		sprintf(count_c_str, "%04d", count);
		string count_str = count_c_str;

		screen_name = count_str + screen_str + extension_str;
		for (int i = 0; i <= VIDEO_BUFFER; i++) {
			char index_str[2];
			sprintf(index_str, "%d", i);

			buffer_name[i] = count_str + buffer_str + index_str + extension_str;
		}

		if (fopen(screen_name.c_str(), "r")) {
			count++;
			continue;
		}
		for (int i = 0; i <= VIDEO_BUFFER; i++) {
			if (fopen(buffer_name[i].c_str(), "r")) {
				count++;
				continue;
			}
		}

		break;
	}

	SDL_SaveBMP(sdl_screen, screen_name.c_str());

	for (int i = 0; i <= VIDEO_BUFFER; i++) {
		SDL_Surface *sdl_buffer = SDL_CreateRGBSurface(SDL_SWSURFACE, VIDEO_WIDTH, VIDEO_HEIGHT, VIDEO_COLOR_DEPTH, color_red_mask, color_green_mask, color_blue_mask, color_alpha_mask);

		byte *buffers;
		if (i == 0) {
			buffers = screen;
		}
		else {
			buffers = buffer[i - 1];
		}

		for (word y = 0; y < VIDEO_HEIGHT; y++) {
			for (word x = 0; x < VIDEO_WIDTH; x++) {
				Uint32 sdl_color = convertColor(intermediate_palette[buffers[(y * VIDEO_WIDTH) + x]]);
				drawPixel(sdl_buffer, x, y, sdl_color);
			}
		}

		SDL_SaveBMP(sdl_buffer, buffer_name[i].c_str());

		SDL_FreeSurface(sdl_buffer);
	}

	printf("[Video::capture()] screenshots taken: %s and buffers\n", screen_name.c_str());
}
