#include "input.h"
#include "video.h"

#ifdef _WIN32_WCE
	#define _WIN32_WCE_IBEE // for IBEE
#endif

#ifdef _WIN32_WCE_IBEE
static bool IBEE_key_func = false;
#endif

Input::Input(Memory *memory, Timer *timer)
{
	this->memory = memory;
	this->timer = timer;

	keyboard_status = 0;
	mouse_status = 0;

	is_left_clicked = false;
	is_right_clicked = false;

	is_quit = false;
	is_capture = false;

	for (int i = 0; i < 2; i++) {
		for (int y = 0; y < CURSOR_HEIGHT; y++) {
			for (int x = 0; x < CURSOR_WIDTH; x++) {
				cursor_image[i][(y * CURSOR_WIDTH) + x] = 0;
				cursor_mask[i][(y * CURSOR_WIDTH) + x] = 0;
			}
		}
	}

	cursor[CURSOR_FIRSTFRAME] = NULL;
	cursor[CURSOR_SECONDFRAME] = NULL;

	current_cursor_frame = 0;

	hideCursor();
}


Input::~Input()
{
	if (cursor[CURSOR_FIRSTFRAME] != NULL) {
		SDL_FreeCursor(cursor[CURSOR_FIRSTFRAME]);
	}
	if (cursor[CURSOR_SECONDFRAME] != NULL) {
		SDL_FreeCursor(cursor[CURSOR_SECONDFRAME]);
	}
}


bool Input::refreshKeyboard()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch(event.type){
		case SDL_QUIT:
			is_quit = true;

			return false;
#ifdef _WIN32_WCE_IBEE
		case SDL_KEYUP:
			//printf("Key Up : %d\n", event.key.keysym.sym);
			switch (event.key.keysym.sym) {
				case SDLK_PAGEDOWN:
					IBEE_key_func = false;
					break;

				case SDLK_RETURN:
					keyboard_status &= ~(INPUT_OK | INPUT_CTRL);
					break;

				case SDLK_PAGEUP:
					keyboard_status &= ~INPUT_CANCEL;
					break;

				case SDLK_UP:
					keyboard_status &= ~INPUT_UP;
					break;

				case SDLK_DOWN:
					keyboard_status &= ~INPUT_DOWN;
					break;

				case SDLK_LEFT:
					keyboard_status &= ~INPUT_LEFT;
					break;

				case SDLK_RIGHT:
					keyboard_status &= ~INPUT_RIGHT;
					break;

				case SDLK_END:
					keyboard_status &= ~INPUT_END;
					break;

				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					keyboard_status &= ~INPUT_SHIFT;
					break;
			};

			break;

		case SDL_KEYDOWN:
			//printf("Key Down : %d\n", event.key.keysym.sym);
			switch (event.key.keysym.sym) {
				case SDLK_PAGEDOWN:
					IBEE_key_func = true;
					break;

				case SDLK_RETURN:
					if(!IBEE_key_func){
						keyboard_status |= INPUT_OK;
					} else {
						keyboard_status |= INPUT_CTRL;
					};
					break;

				case SDLK_PAGEUP:
					if(!IBEE_key_func){
						keyboard_status |= INPUT_CANCEL;
					} else {
						is_quit = true;
					};
					break;

				case SDLK_UP:
					keyboard_status |= INPUT_UP;
					break;

				case SDLK_DOWN:
					keyboard_status |= INPUT_DOWN;
					break;

				case SDLK_LEFT:
					keyboard_status |= INPUT_LEFT;
					break;

				case SDLK_RIGHT:
					keyboard_status |= INPUT_RIGHT;
					break;

				case SDLK_END:
					keyboard_status |= INPUT_END;
					break;

				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					keyboard_status |= INPUT_SHIFT;
					break;

				case SDLK_HOME:
					is_capture = true;
					break;
			};
			break;
#else
		case SDL_KEYUP:
			//printf("Key Up : %d\n", event.key.keysym.sym);
			switch (event.key.keysym.sym) {
				case SDLK_RETURN:
				case SDLK_SPACE:
					keyboard_status &= ~INPUT_OK;
					break;

				case SDLK_ESCAPE:
				case SDLK_INSERT:
					keyboard_status &= ~INPUT_CANCEL;
					break;

				case SDLK_UP:
					keyboard_status &= ~INPUT_UP;
					break;

				case SDLK_DOWN:
					keyboard_status &= ~INPUT_DOWN;
					break;

				case SDLK_LEFT:
					keyboard_status &= ~INPUT_LEFT;
					break;

				case SDLK_RIGHT:
					keyboard_status &= ~INPUT_RIGHT;
					break;

				case SDLK_RCTRL:
				case SDLK_LCTRL:
					keyboard_status &= ~INPUT_CTRL;
					break;

				case SDLK_END:
					keyboard_status &= ~INPUT_END;
					break;

				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					keyboard_status &= ~INPUT_SHIFT;
					break;
			};

			break;

		case SDL_KEYDOWN:
			//printf("Key Down : %d\n", event.key.keysym.sym);
			switch (event.key.keysym.sym) {
				case SDLK_RETURN:
				case SDLK_SPACE:
					keyboard_status |= INPUT_OK;
					break;

				case SDLK_ESCAPE:
				case SDLK_INSERT:
				case SDLK_PAGEUP:
					keyboard_status |= INPUT_CANCEL;
					break;

				case SDLK_UP:
					keyboard_status |= INPUT_UP;
					break;

				case SDLK_DOWN:
					keyboard_status |= INPUT_DOWN;
					break;

				case SDLK_LEFT:
					keyboard_status |= INPUT_LEFT;
					break;

				case SDLK_RIGHT:
					keyboard_status |= INPUT_RIGHT;
					break;

				case SDLK_RCTRL:
				case SDLK_LCTRL:
				case SDLK_PAGEDOWN:
					keyboard_status |= INPUT_CTRL;
					break;

				case SDLK_END:
					keyboard_status |= INPUT_END;
					break;

				case SDLK_RSHIFT:
				case SDLK_LSHIFT:
					keyboard_status |= INPUT_SHIFT;
					break;

				case SDLK_HOME:
					is_capture = true;
					break;

				case SDLK_DELETE:
					is_quit = true;
					break;
			};
			break;
#endif
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
			mouse_x = event.motion.x;
			mouse_y = event.motion.y;

			memory->b_SystemVariable->writeWord(iw_Mouse_CoordX, mouse_x);
			memory->b_SystemVariable->writeWord(iw_Mouse_CoordY, mouse_y);

			if (timer->checkCursorTimer() > CURSOR_FRAME) {
				current_cursor_frame ^= 1;
				setCursorImage(current_cursor_frame);
				timer->resetCursorTimer();
			}

#ifdef _WIN32_WCE
			if (event.button.button == SDL_BUTTON_LEFT)
				is_left_clicked = (event.type == SDL_MOUSEBUTTONUP);
			else if (event.button.button == SDL_BUTTON_RIGHT)
				is_right_clicked = (event.type == SDL_MOUSEBUTTONUP);
#else
			if (event.button.button == SDL_BUTTON_LEFT)
				is_left_clicked = (event.type == SDL_MOUSEBUTTONDOWN);
			else if (event.button.button == SDL_BUTTON_RIGHT)
				is_right_clicked = (event.type == SDL_MOUSEBUTTONDOWN);
#endif
			break;

		}
	}

	return true;
}

void Input::refreshMouse(word *coord_x, word *coord_y)
{
/*
	int row, column;
	mouse_status = (word) SDL_GetMouseState(&row, &column);

	*coord_x = (word) row;
	*coord_y = (word) column;

	if((mouse_status & SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK)
		printf("Mouse Position(Left Click) : %d x %d\n", *coord_x, *coord_y);
	if((mouse_status & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK)
		printf("Mouse Position(Right Click) : %d x %d\n", *coord_x, *coord_y);
*/
	refreshKeyboard();
	*coord_x = mouse_x;
	*coord_y = mouse_y;
/*
	memory->b_SystemVariable->writeWord(iw_Mouse_CoordX, *coord_x);
	memory->b_SystemVariable->writeWord(iw_Mouse_CoordY, *coord_y);

	if (timer->checkCursorTimer() > CURSOR_FRAME) {
		current_cursor_frame ^= 1;
		setCursorImage(current_cursor_frame);
		timer->resetCursorTimer();
	}
*/
}


void Input::refreshMouse()
{
	word coord_x, coord_y;
	refreshMouse(&coord_x, &coord_y);
}


bool Input::refresh()
{
	if (!is_quit) {
		refreshMouse();
		return refreshKeyboard();
	}
	else {
		return false;
	}
}


bool Input::check(word type)
{
	word keyboard = 0;
	word mouse = 0;

	switch (type) {
		case INPUT_OK:
			keyboard = keyboard_status & INPUT_OK;
			mouse = isLeftClicked(); //mouse_status & SDL_BUTTON_LMASK;
			break;

		case INPUT_CANCEL:
			keyboard = keyboard_status & INPUT_CANCEL;
			mouse = isRightClicked(); //mouse_status & SDL_BUTTON_RMASK;
			break;

		case INPUT_UP:
			keyboard = keyboard_status & INPUT_UP;
			break;

		case INPUT_DOWN:
			keyboard = keyboard_status & INPUT_DOWN;
			break;

		case INPUT_LEFT:
			keyboard = keyboard_status & INPUT_LEFT;
			break;

		case INPUT_RIGHT:
			keyboard = keyboard_status & INPUT_RIGHT;
			break;

		case INPUT_CTRL:
			keyboard = keyboard_status & INPUT_CTRL;
			break;

		case INPUT_END:
			keyboard = keyboard_status & INPUT_END;
			break;

		case INPUT_SHIFT:
			keyboard = keyboard_status & INPUT_SHIFT;
			break;
	}

	return ((keyboard != 0) || (mouse != 0));
}


bool Input::isCapture()
{
	if (is_capture) {
		is_capture = false;
		return true;
	}
	else {
		return false;
	}
}


bool Input::isKeyPressed()
{
	//TODO: need refreshKeyboard()?
	// refreshKeyboard();
	return (keyboard_status != 0);
}


bool Input::isLeftClicked()
{
	refreshMouse();

	bool ret = is_left_clicked;
	is_left_clicked = false;

	return ret;
/*
	refreshMouse();

	return ((mouse_status & SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK);
*/
}


bool Input::isRightClicked()
{
	refreshMouse();

	bool ret = is_right_clicked;
	is_right_clicked = false;

	return ret;
/*
	refreshMouse();

	return ((mouse_status & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK);
*/
}


bool Input::isCursorVisible()
{
	if (SDL_ShowCursor(SDL_QUERY) == SDL_ENABLE) {
		return true;
	}
	else {
		return false;
	}
}


void Input::showCursor()
{
	SDL_ShowCursor(SDL_ENABLE);
}


void Input::hideCursor()
{
	SDL_ShowCursor(SDL_DISABLE);
}


void Input::setCursorPosition(word coord_x, word coord_y)
{
	SDL_WarpMouse((Uint16) coord_x, (Uint16) coord_y);

	mouse_x = coord_x;
	mouse_y = coord_y;

	memory->b_SystemVariable->writeWord(iw_Mouse_CoordX, mouse_x);
	memory->b_SystemVariable->writeWord(iw_Mouse_CoordY, mouse_y);

	if (timer->checkCursorTimer() > CURSOR_FRAME) {
		current_cursor_frame ^= 1;
		setCursorImage(current_cursor_frame);
		timer->resetCursorTimer();
	}
}


void Input::loadCursorImage(word offset)
{
	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *source = segment->get(&offset);

	for (int i = 0; i < 2; i++) {
		int x, y;
		for (y = 0; y < CURSOR_HEIGHT; y++) {
			for (x = 0; x < CURSOR_WIDTH; x++) {
				cursor_image[i][(y * CURSOR_WIDTH) + x] = source->queryByte(offset++);
			}
		}

		for (y = 0; y < CURSOR_HEIGHT; y++) {
			for (x = 0; x < CURSOR_WIDTH; x++) {
				cursor_mask[i][(y * CURSOR_WIDTH) + x] = ~(source->queryByte(offset++));
			}
		}

		for (y = 0; y < CURSOR_HEIGHT; y++) {
			for (x = 0; x < CURSOR_WIDTH; x++) {
				cursor_image[i][(y * CURSOR_WIDTH) + x] ^= cursor_mask[i][(y * CURSOR_WIDTH) + x];
			}
		}

		cursor[i] = SDL_CreateCursor(cursor_image[i], cursor_mask[i], CURSOR_SIZE, CURSOR_SIZE, 0, 0);
	}
}


void Input::setCursorImage(int cursor_frame)
{
	SDL_SetCursor(cursor[cursor_frame]);
}

void Input::reset()
{
	is_left_clicked = false;
	is_right_clicked = false;
}
