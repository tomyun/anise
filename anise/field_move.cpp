#include "field.h"

word Field::verifyMovement()
{
	word character_offset = data->queryWord(header_offset + C5_CHARACTER);
	word return_value = ENTRANCE_NOTFOUND;
	bool is_entrance = false;

	word collision = checkDetectCollision(0);
	if (collision == COLLISION_NOTDETECTED) {
		movement_collision = collision;
		is_entrance = true;
	}
	else {
		if (has_moved) {
			if (input->isLeftClicked()) {
				word mouse_coord_x, mouse_coord_y;
				input->refreshMouse(&mouse_coord_x, &mouse_coord_y);

				byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
				byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);
				word horizontal_center = (data->queryWord(character_offset + CHARACTER_COORD_XW) - view_coord_xw + view_margin_xw) * SPRITE_SIZE;
				word vertical_center = ((data->queryWord(character_offset + CHARACTER_COORD_YW) - view_coord_yw) * SPRITE_SIZE) + view_margin_y;

				if (((mouse_coord_x >= (horizontal_center - CHARACTER_CLICK_RANGE)) && (mouse_coord_x < (horizontal_center + (SPRITE_SIZE * character_widthw) + CHARACTER_CLICK_RANGE))) || ((mouse_coord_y >= (vertical_center - CHARACTER_CLICK_RANGE)) && (mouse_coord_y < (vertical_center + (SPRITE_SIZE * character_heightw) + CHARACTER_CLICK_RANGE)))) {
					movement_collision = collision;
				}
				else {
					if (movement_collision == collision) {
						is_entrance = true;
					}
					else {
						movement_collision = collision;
					}
				}
			}
			else if (input->isKeyPressed()) {
				movement_collision = collision;
			}
			else {
				is_entrance = true;
			}
		}
		else {
			is_entrance = true;
		}
	}

	if (is_entrance) {
		word entrance = checkEntrance(0);
		if (entrance != ENTRANCE_NOTFOUND) {
			if (movement_entrance != entrance) {
				movement_entrance = entrance;
				return_value = entrance + 50;
			}
			else {
				word direction = (word) (data->queryByte(character_offset + CHARACTER_FRAME) >> 1);
				if (movement_direction != direction) {
					return_value = movement_entrance + 50;
				}
			}
		}
	}

	movement_direction = (word) (data->queryByte(character_offset + CHARACTER_FRAME) >> 1);

	if (return_value != ENTRANCE_NOTFOUND) {
		while (input->isLeftClicked()) {
			input->refresh();
		}
	}

	return return_value;
}


word Field::detectCollision(word character_offset)
{
	byte character_flag = data->queryByte(character_offset + CHARACTER_FLAG);
	if ((character_flag & CHARACTER_FLAG_NOCOLLISION) == CHARACTER_FLAG_NOCOLLISION) {
		return COLLISION_NOTDETECTED;
	}
	else {
		word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
		word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);
		byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
		byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);

		byte character_direction = data->queryByte(character_offset + CHARACTER_FRAME) >> 1;
		switch (character_direction) {
			case CHARACTER_DIRECTION_UP:
				character_coord_yw--;
				break;

			case CHARACTER_DIRECTION_DOWN:
				character_coord_yw += character_heightw;
				break;

			case CHARACTER_DIRECTION_LEFT:
				character_coord_xw--;
				break;

			case CHARACTER_DIRECTION_RIGHT:
			default:
				character_coord_xw += character_widthw;
				break;
		}

		word main_character_offset = data->queryWord(header_offset + C5_CHARACTER);
		byte main_character_flag = data->queryByte(main_character_offset + CHARACTER_FLAG);
		if (main_character_flag == CHARACTER_FLAG_NULL) {
			return COLLISION_NOTDETECTED;
		}
		else {
			word collision_status = 0;
			while (true) {
				if (character_offset != main_character_offset) {
					if ((collision_status != 1) && (collision_status != 2)) {
						main_character_flag = data->queryByte(main_character_offset + CHARACTER_FLAG);
						if (main_character_flag == CHARACTER_FLAG_NULL) {
							return COLLISION_NOTDETECTED;
						}
						else {
							if (((main_character_flag & 0x20) != 0x20) && ((main_character_flag & CHARACTER_FLAG_DEACTIVATE) != CHARACTER_FLAG_DEACTIVATE) && ((main_character_flag & 0x02) == 0x02)) {
								word main_character_coord_xw = data->queryWord(main_character_offset + CHARACTER_COORD_XW);
								word main_character_coord_yw = data->queryWord(main_character_offset + CHARACTER_COORD_YW);

								word other_left = main_character_coord_xw;
								word self_horizontal = character_coord_xw;
								if ((character_direction == CHARACTER_DIRECTION_UP) || (character_direction == CHARACTER_DIRECTION_DOWN)) {
									self_horizontal += (character_widthw - 1);
								}

								if (other_left <= self_horizontal) {
									word other_right = main_character_coord_xw + (character_widthw - 1);

									if (other_right >= self_horizontal) {
										word other_top = main_character_coord_yw;
										word self_vertical = character_coord_yw;
										if ((character_direction == CHARACTER_DIRECTION_LEFT) || (character_direction == CHARACTER_DIRECTION_RIGHT)) {
											self_vertical += (character_heightw - 1);
										}

										if (other_top <= self_vertical) {
											word other_bottom = main_character_coord_yw + (character_heightw - 1);

											if (other_bottom >= self_vertical) {
												return collision_status;
											}
										}
									}
								}
							}
						}
					}
				}

				main_character_offset += C5_CHARACTER_SIZE;
				collision_status++;
				continue;
			}
		}
	}

	return 0;
}


void Field::executeOperation(word character_offset)
{
	byte operation_count = data->queryByte(character_offset + CHARACTER_OPERATION_COUNT);
	if (operation_count != 0x0F) {
		data->decreaseByte(character_offset + CHARACTER_OPERATION_COUNT);
	}

	byte operation_type = data->queryByte(character_offset + CHARACTER_OPERATION_TYPE) - 1;
	switch (operation_type) {
		case OPERATION_MOVE_UP:
			{
				moveCharacterUp(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE_DOWN:
			{
				moveCharacterDown(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE_LEFT:
			{
				moveCharacterLeft(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE_RIGHT:
			{
				moveCharacterRight(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_TOGGLE_FRAME:
			{
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE:
			{
				has_moved = false;

				byte character_frame = data->queryByte(character_offset + CHARACTER_FRAME);
				word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
				word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);

				if (input->isCursorVisible()) {
					// check right click
					if (!is_path_found) {
						if (input->isRightClicked()) {
							word mouse_coord_x, mouse_coord_y;
							input->refreshMouse(&mouse_coord_x, &mouse_coord_y);

							word view_coord_x0 = view_margin_xw * SPRITE_SIZE;
							word view_coord_x1 = (view_margin_xw + view_widthw - 1) * SPRITE_SIZE;
							word view_coord_y0 = view_margin_y;
							word view_coord_y1 = (view_heightw * SPRITE_SIZE) + view_margin_y;

							if ((mouse_coord_x >= view_coord_x0 && mouse_coord_x < view_coord_x1) && ((mouse_coord_y >= view_coord_y0) && (mouse_coord_y < view_coord_y1))) {
								word destination_coord_xw = (mouse_coord_x >> 4) - view_margin_xw + view_coord_xw;
								word destination_coord_yw = ((mouse_coord_y - view_margin_y) >> 4) + view_coord_yw;

								setPath(character_offset, destination_coord_xw, destination_coord_yw, false);
							}
							else {
								is_path_found = false;
							}
						}
					}
					else {
						moveCharacterOnPath(character_offset);
						toggleFrame(character_offset);
					}

					// check left click
					if (input->isLeftClicked()) {
						word mouse_coord_x, mouse_coord_y;
						input->refreshMouse(&mouse_coord_x, &mouse_coord_y);

						byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
						byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);
						word horizontal_center = (data->queryWord(character_offset + CHARACTER_COORD_XW) - view_coord_xw + view_margin_xw) * SPRITE_SIZE;
						word vertical_center = ((data->queryWord(character_offset + CHARACTER_COORD_YW) - view_coord_yw) * SPRITE_SIZE) + view_margin_y;

						// check up and down
						if ((mouse_coord_x >= (horizontal_center - CHARACTER_CLICK_RANGE)) && (mouse_coord_x < (horizontal_center + (SPRITE_SIZE * character_widthw) + CHARACTER_CLICK_RANGE))) {
							if (mouse_coord_y < vertical_center) {
								moveCharacterUp(character_offset);
								saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
								has_moved = true;
								break;
							}
							else if (mouse_coord_y >= (vertical_center + (SPRITE_SIZE * character_heightw))) {
								moveCharacterDown(character_offset);
								saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
								has_moved = true;
								break;
							}
						}

						// check left and right
						word vertical_range;
						if (vertical_center < CHARACTER_CLICK_RANGE) {
							vertical_range = vertical_center;
						}
						else {
							vertical_range = CHARACTER_CLICK_RANGE;
						}

						if ((mouse_coord_y >= (vertical_center - vertical_range)) && (mouse_coord_y < (vertical_center + (SPRITE_SIZE * character_heightw) + CHARACTER_CLICK_RANGE))) {
							if (mouse_coord_x < horizontal_center) {
								moveCharacterLeft(character_offset);
								saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
								has_moved = true;
								break;
							}
							else if (mouse_coord_x >= (horizontal_center + (SPRITE_SIZE * character_widthw))) {
								moveCharacterRight(character_offset);
								saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
								has_moved = true;
								break;
							}
						}

						// nothing
						has_moved = true;
						break;
					}					
				}

				// check keyboard
				if (input->check(INPUT_UP)) {
					moveCharacterUp(character_offset);
					saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
					has_moved = true;
					break;
				}
				else if (input->check(INPUT_DOWN)) {
					moveCharacterDown(character_offset);
					saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
					has_moved = true;
					break;
				}
				else if (input->check(INPUT_LEFT)) {
					moveCharacterLeft(character_offset);
					saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
					has_moved = true;
					break;
				}
				else if (input->check(INPUT_RIGHT)) {
					moveCharacterRight(character_offset);
					saveCharacterLog(character_offset, character_frame, character_coord_xw, character_coord_yw);
					has_moved = true;
					break;
				}

				// nothing
				has_moved = true;
				break;
			}
			break;

		case OPERATION_NULL:
			break;

		case OPERATION_MOVE_LEFT_UP:
			{
				moveCharacterLeft(character_offset);
				moveCharacterUp(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE_RIGHT_UP:
			{
				moveCharacterRight(character_offset);
				moveCharacterUp(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE_LEFT_DOWN:
			{
				moveCharacterLeft(character_offset);
				moveCharacterDown(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_MOVE_RIGHT_DOWN:
			{
				moveCharacterRight(character_offset);
				moveCharacterDown(character_offset);
				toggleFrame(character_offset);
			}
			break;

		case OPERATION_RESTART_ENGINE:
			{
				//TODO: restart engine?
			}
			break;

		case OPERATION_AUTOMOVE:
			{
				word destination_coord_xw = (word) (data->queryByte(character_offset + CHARACTER_DESTINATION_COORD_XW));
				word destination_coord_yw = (word) (data->queryByte(character_offset + CHARACTER_DESTINATION_COORD_YW));

				if (is_path_found) {
					moveCharacterOnPath(character_offset);
				}
				else {
					setPath(character_offset, destination_coord_xw, destination_coord_yw, true);
				}

				toggleFrame(character_offset);
			}
			break;
	}
}


bool Field::moveCharacterUp(word character_offset)
{
	byte character_frame = data->queryByte(character_offset + CHARACTER_FRAME);
	character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_UP;
	data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);
	byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
	byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);

	if (character_coord_yw != 0) {
		if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
			bool is_wall = false;
			if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_INVISIBLE) == false) {
				word sprite_offset = calculateMapOffset(character_coord_xw, character_coord_yw + character_heightw - 2);

				for (int i = 0; i < character_widthw; i++) {
					if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
						sprite_offset += 2;
						continue;
					}
					else {
						is_wall = true;
						break;
					}
				}
			}

			if (!is_wall) {
				// update
				data->writeWord(character_offset + CHARACTER_COORD_YW, --character_coord_yw);

				if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
					word heightw = character_coord_yw - view_coord_yw + 1;
					if (heightw <= character_uplimit) {
						if (view_coord_yw != view_uplimit) {
							data->decreaseWord(iw_View_CoordYw);
							has_map_scrolled = true;
						}
					}
				}

				return true;
			}
		}
		else {
			return false;
		}
	}

	// blocked
	word sprite_offset = calculateMapOffset(character_coord_xw - 1, character_coord_yw + character_heightw - 2);
	if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + 3, MAP_WALL_MASK))) {
		// bypass left
		character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_LEFT;
		data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

		if (character_coord_xw != 0) {
			if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
				sprite_offset = calculateMapOffset(character_coord_xw - 1, character_coord_yw + character_heightw - 1);

				if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
					// update
					data->writeWord(character_offset + CHARACTER_COORD_XW, --character_coord_xw);

					if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
						word widthw = character_coord_xw - view_coord_xw + 1;
						if (widthw <= character_leftlimit) {
							if (view_coord_xw != view_leftlimit) {
								data->decreaseWord(iw_View_CoordXw);
								has_map_scrolled = true;
							}
						}
					}

					return true;
				}
			}
		}

		return false;
	}
	else {
		sprite_offset += character_widthw * 2;
		if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + 3, MAP_WALL_MASK))) {
			// bypass right
			character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_RIGHT;
			data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

			if ((character_coord_xw + character_widthw) != map_widthw) {
				if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
					sprite_offset = calculateMapOffset(character_coord_xw + character_widthw, character_coord_yw + character_heightw - 1);

					if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
						// update
						data->writeWord(character_offset + CHARACTER_COORD_XW, ++character_coord_xw);

						if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
							word widthw = character_coord_xw + character_widthw - view_coord_xw + character_rightlimit - 1;

							if (widthw >= view_widthw) {
								if ((view_coord_xw + view_widthw - 1) != view_rightlimit) {
									data->increaseWord(iw_View_CoordXw);
									has_map_scrolled = true;
								}
							}
						}

						return true;
					}
				}
			}	
		}

		return false;
	}
}


bool Field::moveCharacterDown(word character_offset)
{
	byte character_frame = data->queryByte(character_offset + CHARACTER_FRAME);
	character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_DOWN;
	data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);
	byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
	byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);

	if ((character_coord_yw + character_heightw) != map_heightw) {
		if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
			bool is_wall = false;
			if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_INVISIBLE) == false) {
				// check wall
				word sprite_offset = calculateMapOffset(character_coord_xw, character_coord_yw + character_heightw);

				for (int i = 0; i < character_widthw; i++) {
					if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
						sprite_offset += 2;
						continue;
					}
					else {
						is_wall = true;
						break;
					}
				}
			}

			if (!is_wall) {
				// update
				data->writeWord(character_offset + CHARACTER_COORD_YW, ++character_coord_yw);

				if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
					word heightw = character_coord_yw + character_heightw - view_coord_yw + character_downlimit - 1;
					if (heightw >= view_heightw) {
						if ((view_coord_yw + view_heightw - 1) != view_downlimit) {
							data->increaseWord(iw_View_CoordYw);
							has_map_scrolled = true;
						}
					}
				}

				return true;
			}
		}
		else {
			return false;
		}
	}

	// blocked
	word sprite_offset = calculateMapOffset(character_coord_xw - 1, character_coord_yw + character_heightw);
	if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + 3, MAP_WALL_MASK))) {
		// bypass left
		character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_LEFT;
		data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

		if (character_coord_xw != 0) {
			if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
				sprite_offset = calculateMapOffset(character_coord_xw - 1, character_coord_yw + character_heightw - 1);

				if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
					// update
					data->writeWord(character_offset + CHARACTER_COORD_XW, --character_coord_xw);

					if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
						word widthw = character_coord_xw - view_coord_xw + 1;
						if (widthw <= character_leftlimit) {
							if (view_coord_xw != view_leftlimit) {
								data->decreaseWord(iw_View_CoordXw);
								has_map_scrolled = true;
							}
						}
					}

					return true;
				}
			}
		}

		return false;
	}
	else {
		sprite_offset += character_widthw * 2;
		if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + 3, MAP_WALL_MASK))) {
			// bypass right
			character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_RIGHT;
			data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

			if ((character_coord_xw + character_widthw) != map_widthw) {
				if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
					sprite_offset = calculateMapOffset(character_coord_xw + character_widthw, character_coord_yw + character_heightw - 1);

					if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
						// update
						data->writeWord(character_offset + CHARACTER_COORD_XW, ++character_coord_xw);

						if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
							word widthw = character_coord_xw + character_widthw - view_coord_xw + character_rightlimit - 1;

							if (widthw >= view_widthw) {
								if ((view_coord_xw + view_widthw - 1) != view_rightlimit) {
									data->increaseWord(iw_View_CoordXw);
									has_map_scrolled = true;
								}
							}
						}

						return true;
					}
				}
			}	
		}

		return false;
	}
}


bool Field::moveCharacterLeft(word character_offset)
{
	byte character_frame = data->queryByte(character_offset + CHARACTER_FRAME);
	character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_LEFT;
	data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);
	byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
	byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);

	if (character_coord_xw != 0) {
		if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
			bool is_wall = false;
			if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_INVISIBLE) == false) {
				// check wall
				word sprite_offset = calculateMapOffset(character_coord_xw - 1, character_coord_yw + character_heightw - 1);

				if (data->testByte(sprite_offset + 1, MAP_WALL_MASK)) {
					is_wall = true;
				}
			}

			if (!is_wall) {
				// update
				data->writeWord(character_offset + CHARACTER_COORD_XW, --character_coord_xw);

				if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
					word widthw = character_coord_xw - view_coord_xw + 1;
					if (widthw <= character_leftlimit) {
						if (view_coord_xw != view_leftlimit) {
							data->decreaseWord(iw_View_CoordXw);
							has_map_scrolled = true;
						}
					}
				}

				return true;
			}
		}
		else {
			return false;
		}
	}

	// blocked
	word sprite_offset = calculateMapOffset(character_coord_xw - 1, character_coord_yw + character_heightw - 3);
	if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + (map_widthw * 2) + 1, MAP_WALL_MASK))) {
		// bypass up
		character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_UP;
		data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

		if (character_coord_yw != 0) {
			if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
				// check wall
				bool is_wall = false;
				word sprite_offset = calculateMapOffset(character_coord_xw, character_coord_yw + character_heightw - 2);

				for (int i = 0; i < character_widthw; i++) {
					if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
						sprite_offset += 2;
						continue;
					}
					else {
						is_wall = true;
						break;
					}
				}

				if (!is_wall) {
					// update
					data->writeWord(character_offset + CHARACTER_COORD_YW, --character_coord_yw);

					if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
						word heightw = character_coord_yw - view_coord_yw + 1;
						if (heightw <= character_uplimit) {
							if (view_coord_yw != view_uplimit) {
								data->decreaseWord(iw_View_CoordYw);
								has_map_scrolled = true;
							}
						}
					}

					return true;
				}
			}
		}

		return false;
	}
	else {
		sprite_offset += map_widthw * 6;
		if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + (map_widthw * 2) + 1, MAP_WALL_MASK))) {
			// bypass down
			character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_DOWN;
			data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

			if ((character_coord_yw + character_heightw) != map_heightw) {
				if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
					// check wall
					bool is_wall = false;
					sprite_offset = calculateMapOffset(character_coord_xw, character_coord_yw + character_heightw);

					for (int i = 0; i < character_widthw; i++) {
						if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
							sprite_offset += 2;
							continue;
						}
						else {
							is_wall = true;
							break;
						}
					}

					if (!is_wall) {
						// update
						data->writeWord(character_offset + CHARACTER_COORD_YW, ++character_coord_yw);

						if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
							word heightw = character_coord_yw + character_heightw - view_coord_yw + character_downlimit - 1;
							if (heightw >= view_heightw) {
								if ((view_coord_yw + view_heightw - 1) != view_downlimit) {
									data->increaseWord(iw_View_CoordYw);
									has_map_scrolled = true;
								}
							}
						}

						return true;
					}
				}
			}			
		}

		return false;
	}
}


bool Field::moveCharacterRight(word character_offset)
{
	byte character_frame = data->queryByte(character_offset + CHARACTER_FRAME);
	character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_RIGHT;
	data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);
	byte character_widthw = data->queryByte(character_offset + CHARACTER_WIDTHW);
	byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);

	if ((character_coord_xw + character_heightw) != map_widthw) {
		if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
			bool is_wall = false;
			if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_INVISIBLE) == false) {
				word sprite_offset = calculateMapOffset(character_coord_xw + character_widthw, character_coord_yw + character_heightw - 1);

				if (data->testByte(sprite_offset + 1, MAP_WALL_MASK)) {
					is_wall = true;
				}
			}

			if (!is_wall) {
				// update
				data->writeWord(character_offset + CHARACTER_COORD_XW, ++character_coord_xw);

				if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
					word widthw = character_coord_xw + character_widthw - view_coord_xw + character_rightlimit - 1;
					if (widthw >= view_widthw) {
						if ((view_coord_xw + view_widthw - 1) != view_rightlimit) {
							data->increaseWord(iw_View_CoordXw);
							has_map_scrolled = true;
						}
					}
				}

				return true;
			}
		}
		else {
			return false;
		}
	}

	// blocked
	word sprite_offset = calculateMapOffset(character_coord_xw + character_widthw, character_coord_yw + character_heightw - 3);
	if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + (map_widthw * 2) + 1, MAP_WALL_MASK))) {
		// bypass up
		character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_UP;
		data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

		if (character_coord_yw != 0) {
			if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
				// check wall
				bool is_wall = false;
				word sprite_offset = calculateMapOffset(character_coord_xw, character_coord_yw + character_heightw - 2);

				for (int i = 0; i < character_widthw; i++) {
					if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
						sprite_offset += 2;
						continue;
					}
					else {
						is_wall = true;
						break;
					}
				}

				if (!is_wall) {
					// update
					data->writeWord(character_offset + CHARACTER_COORD_YW, --character_coord_yw);

					if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
						word heightw = character_coord_yw - view_coord_yw + 1;
						if (heightw <= character_uplimit) {
							if (view_coord_yw != view_uplimit) {
								data->decreaseWord(iw_View_CoordYw);
								has_map_scrolled = true;
							}
						}
					}

					return true;
				}
			}
		}

		return false;
	}
	else {
		sprite_offset += map_widthw * 6;
		if (!(data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || !(data->testByte(sprite_offset + (map_widthw * 2) + 1, MAP_WALL_MASK))) {
			// bypass down
			character_frame = (character_frame & CHARACTER_FRAME_MASK) + CHARACTER_FRAME_DOWN;
			data->writeByte(character_offset + CHARACTER_FRAME, character_frame);

			if ((character_coord_yw + character_heightw) != map_heightw) {
				if (detectCollision(character_offset) == COLLISION_NOTDETECTED) {
					// check wall
					bool is_wall = false;
					sprite_offset = calculateMapOffset(character_coord_xw, character_coord_yw + character_heightw);

					for (int i = 0; i < character_widthw; i++) {
						if (data->testByte(sprite_offset + 1, MAP_WALL_MASK) == false) {
							sprite_offset += 2;
							continue;
						}
						else {
							is_wall = true;
							break;
						}
					}

					if (!is_wall) {
						// update
						data->writeWord(character_offset + CHARACTER_COORD_YW, ++character_coord_yw);

						if (data->testByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_CHECKSCROLL)) {
							word heightw = character_coord_yw + character_heightw - view_coord_yw + character_downlimit - 1;
							if (heightw >= view_heightw) {
								if ((view_coord_yw + view_heightw - 1) != view_downlimit) {
									data->increaseWord(iw_View_CoordYw);
									has_map_scrolled = true;
								}
							}
						}

						return true;
					}
				}
			}
		}

		return false;
	}
}


void Field::toggleFrame(word character_offset)
{
	data->xorByte(character_offset + CHARACTER_FRAME, CHARACTER_FRAME_MASK);
}
