#include "input.h"

Input::Input(Memory *memory, Timer *timer)
{
	this->memory = memory;
	this->timer = timer;

	keyboard_status = 0;
	mouse_status = 0;

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
		if (event.type == SDL_QUIT) {
			is_quit = true;

			return false;
		}
		else if (event.type == SDL_KEYUP) {
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
			}
		}
		else if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
				case SDLK_RETURN:
				case SDLK_SPACE:
					keyboard_status |= INPUT_OK;
					break;

				case SDLK_ESCAPE:
				case SDLK_INSERT:
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
			}
		}
	}

	return true;
}


void Input::refreshMouse(word *coord_x, word *coord_y)
{
	int row, column;
	mouse_status = (word) SDL_GetMouseState(&row, &column);

	*coord_x = (word) row;
	*coord_y = (word) column;

	memory->b_SystemVariable->writeWord(iw_Mouse_CoordX, *coord_x);
	memory->b_SystemVariable->writeWord(iw_Mouse_CoordY, *coord_y);

	if (timer->checkCursorTimer() > CURSOR_FRAME) {
		current_cursor_frame ^= 1;
		setCursorImage(current_cursor_frame);
		timer->resetCursorTimer();
	}
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
			mouse = mouse_status & SDL_BUTTON_LMASK;
			break;

		case INPUT_CANCEL:
			keyboard = keyboard_status & INPUT_CANCEL;
			mouse = mouse_status & SDL_BUTTON_RMASK;
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
	return (keyboard_status != 0);
}


bool Input::isLeftClicked()
{
	refreshMouse();
	return ((mouse_status & SDL_BUTTON_LMASK) == SDL_BUTTON_LMASK);
}


bool Input::isRightClicked()
{
	refreshMouse();
	return ((mouse_status & SDL_BUTTON_RMASK) == SDL_BUTTON_RMASK);
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
}


void Input::loadCursorImage(word offset)
{
	MemorySegment *segment = memory->getCurrentSegment();
	MemoryBlock *source = segment->get(&offset);

	for (int i = 0; i < 2; i++) {
		for (int y = 0; y < CURSOR_HEIGHT; y++) {
			for (int x = 0; x < CURSOR_WIDTH; x++) {
				cursor_image[i][(y * CURSOR_WIDTH) + x] = source->queryByte(offset++);
			}
		}

		for (int y = 0; y < CURSOR_HEIGHT; y++) {
			for (int x = 0; x < CURSOR_WIDTH; x++) {
				cursor_mask[i][(y * CURSOR_WIDTH) + x] = ~(source->queryByte(offset++));
			}
		}

		for (int y = 0; y < CURSOR_HEIGHT; y++) {
			for (int x = 0; x < CURSOR_WIDTH; x++) {
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
