#include "field.h"

Field::Field(Memory *memory, Video *video, Input *input)
{
	this->memory = memory;
	this->video = video;
	this->input = input;
	
	data = NULL;
	map = NULL;

	header_offset = memory->b_SystemVariable->queryWord(iwpo_FieldHeader);

	is_path_found = false;
	has_moved = false;

	for (int i = 0; i < VIEW_SPRITES; i++) {
		for (int j = 0; j < VIEW_LAYERS; j++) {
			view[i][j] = 0;
			view_buffer[i][j] = 0;
		}
	}

	movement_direction = NONE;
	movement_collision = COLLISION_NOTDETECTED;
	movement_entrance = ENTRANCE_NOTFOUND;

	for (int i = 0; i < CHARACTER_LOG; i++) {
		character_log[i].frame = 0;
		character_log[i].coord_xw = 0;
		character_log[i].coord_yw = 0;
	}

	operation_type = 0x8A;	//TODO: avoid hard coding
}


Field::~Field()
{
}


void Field::initialize()
{
	word dummy_offset = 0;
	data = memory->getCurrentSegment()->get(&dummy_offset);
	map = memory->s_FieldMap->get(&dummy_offset);

	map_offset = data->queryWord(iwpo_MAPFile);

	view_leftlimit = data->queryWord(iw_View_LeftLimit);
	view_uplimit = data->queryWord(iw_View_UpLimit);
	view_rightlimit = data->queryWord(iw_View_RightLimit);
	view_downlimit = data->queryWord(iw_View_DownLimit);

	view_margin_xw = data->queryWord(iw_View_MarginXw);
	view_margin_y = data->queryWord(iw_View_MarginY);
	view_widthw = data->queryWord(iw_View_Widthw);
	view_heightw = data->queryWord(iw_View_Heightw);

	character_leftlimit = data->queryWord(iw_Character_LeftLimit);
	character_uplimit = data->queryWord(iw_Character_UpLimit);
	character_rightlimit = data->queryWord(iw_Character_RightLimit);
	character_downlimit = data->queryWord(iw_Character_DownLimit);

	view_coord_xw = data->queryWord(iw_View_CoordXw);
	view_coord_yw = data->queryWord(iw_View_CoordYw);

	map_widthw = data->queryWord(iw_Map_Widthw);
	map_heightw = data->queryWord(iw_Map_Heightw);
}


word Field::loadMapFile()
{
	map_widthw = data->queryWord(map_offset + MAP_WIDTHW);
	map_heightw = data->queryWord(map_offset + MAP_HEIGHTW);

	data->writeWord(iw_Map_Widthw, map_widthw);
	data->writeWord(iw_Map_Heightw, map_heightw);

	path_offset = ((map_widthw * map_heightw)) * 2 + map_offset + 4;
	is_path_found = false;

	movement_direction = NONE;
	movement_collision = COLLISION_NOTDETECTED;
	movement_entrance = ENTRANCE_NOTFOUND;

	return path_offset;
}


void Field::loadC5File()
{
	word si_offset = data->queryWord(header_offset + C5_4);
	word di_offset = data->queryWord(si_offset);
	data->writeByte(di_offset + 2, 0xC1);
	data->writeByte(di_offset + 4, 0xC3);

	movement_direction = NONE;
	movement_collision = COLLISION_NOTDETECTED;
	movement_entrance = ENTRANCE_NOTFOUND;
}


void Field::initializeMap()
{
	for (word yw = 0; yw < view_heightw; yw++) {
		if (yw >= map_heightw) {
			break;
		}

		for (word xw = 0; xw < view_widthw; xw++) {
			if (xw >= map_widthw) {
				break;
			}

			word sprite_index = data->queryWord(calculateMapOffset(view_coord_xw + xw, view_coord_yw + yw));

			view[(yw * view_widthw) + xw][0] = sprite_index;
			view[(yw * view_widthw) + xw][1] = 0;
			view[(yw * view_widthw) + xw][2] = 0;
			view[(yw * view_widthw) + xw][3] = 0;
		}
	}
}


void Field::setCharactersOnMap()
{
	word character_offset = data->queryWord(header_offset + C5_CHARACTER);

	while (true) {
		byte flag = data->queryByte(character_offset + CHARACTER_FLAG);
		if (flag == CHARACTER_FLAG_NULL) {
			break;
		}

		if ((flag & CHARACTER_FLAG_DEACTIVATE) == CHARACTER_FLAG_DEACTIVATE) {
			character_offset += C5_CHARACTER_SIZE;
			continue;
		}
		if ((flag & 0x02) != 0x02) {
			character_offset += C5_CHARACTER_SIZE;
			continue;
		}

		word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
		word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);

		//TODO: clean it up
		word unknown_offset0 = (data->queryByte(character_offset + CHARACTER_6) * 8) + data->queryWord(header_offset + C5_FUNCTION3_2) + data->queryByte(character_offset + CHARACTER_FRAME);
		word unknown_offset1 = (data->queryByte(unknown_offset0) * 2) + data->queryWord(header_offset + C5_FUNCTION3_0);
		word sprite_info_offset = data->queryWord(unknown_offset1);

		byte character_widthw = data->queryByte(sprite_info_offset);
		byte character_heightw = data->queryByte(sprite_info_offset + 2);

		data->writeByte(character_offset + CHARACTER_WIDTHW, character_widthw);
		data->writeByte(character_offset + CHARACTER_HEIGHTW, character_heightw);

		word sprite_index_offset = sprite_info_offset + 4;

		for (word yw = 0; yw < character_heightw; yw++) {
			if (character_coord_yw + yw < view_coord_yw) {
				continue;
			}

			for (word xw = 0; xw < character_widthw; xw++) {
				if (character_coord_xw + xw < view_coord_xw) {
					continue;
				}

				//HACK: check it out
				//word sprite_index = data->queryWord(sprite_index_offset) + 1;
				word sprite_index = data->queryWord(sprite_index_offset);
				sprite_index_offset += 2;

				if (view[((character_coord_yw - view_coord_yw + yw) * view_widthw) + (character_coord_xw - view_coord_xw + xw)][1] == 0) {
					view[((character_coord_yw - view_coord_yw + yw) * view_widthw) + (character_coord_xw - view_coord_xw + xw)][1] = sprite_index;
				}
				else if (view[((character_coord_yw - view_coord_yw + yw) * view_widthw) + (character_coord_xw - view_coord_xw + xw)][2] == 0) {
					view[((character_coord_yw - view_coord_yw + yw) * view_widthw) + (character_coord_xw - view_coord_xw + xw)][2] = sprite_index;
				}
				else if (view[((character_coord_yw - view_coord_yw + yw) * view_widthw) + (character_coord_xw - view_coord_xw + xw)][3] == 0) {
					view[((character_coord_yw - view_coord_yw + yw) * view_widthw) + (character_coord_xw - view_coord_xw + xw)][3] = sprite_index;
				}
			}
		}

		character_offset += C5_CHARACTER_SIZE;
	}
}


void Field::quickDraw()
{
	bool is_drawn = false;

	for (word yw = 0; yw < view_heightw; yw++) {
		for (word xw = 0; xw < view_widthw; xw++) {
			if ((view_buffer[(yw * view_widthw) + xw][0] == view[(yw * view_widthw) + xw][0]) && (view_buffer[(yw * view_widthw) + xw][1] == view[(yw * view_widthw) + xw][1]) && (view_buffer[(yw * view_widthw) + xw][2] == view[(yw * view_widthw) + xw][2]) && (view_buffer[(yw * view_widthw) + xw][3] == view[(yw * view_widthw) + xw][3])) {
				continue;
			}

			view_buffer[(yw * view_widthw) + xw][0] = view[(yw * view_widthw) + xw][0];
			view_buffer[(yw * view_widthw) + xw][1] = view[(yw * view_widthw) + xw][1];
			view_buffer[(yw * view_widthw) + xw][2] = view[(yw * view_widthw) + xw][2];
			view_buffer[(yw * view_widthw) + xw][3] = view[(yw * view_widthw) + xw][3];

			video->putSprite((view_margin_xw + xw) * SPRITE_SIZE, view_margin_y + (yw * SPRITE_SIZE), view_buffer[(yw * view_widthw) + xw][0], view_buffer[(yw * view_widthw) + xw][1], view_buffer[(yw * view_widthw) + xw][2], view_buffer[(yw * view_widthw) + xw][3]);

			//TODO: which one is better?
			//video->updateScreen((view_margin_xw + xw) * SPRITE_SIZE, view_margin_y + (yw * SPRITE_SIZE), SPRITE_SIZE, SPRITE_SIZE);

			is_drawn = true;
		}
	}

	if (is_drawn) {
		video->updateScreen(view_margin_xw * SPRITE_SIZE, view_margin_y, view_widthw * SPRITE_SIZE, view_heightw * SPRITE_SIZE);
	}
}


void Field::draw()
{
	for (word yw = 0; yw < view_heightw; yw++) {
		for (word xw = 0; xw < view_widthw; xw++) {
			view_buffer[(yw * view_widthw) + xw][0] = view[(yw * view_widthw) + xw][0];
			view_buffer[(yw * view_widthw) + xw][1] = view[(yw * view_widthw) + xw][1];
			view_buffer[(yw * view_widthw) + xw][2] = view[(yw * view_widthw) + xw][2];
			view_buffer[(yw * view_widthw) + xw][3] = view[(yw * view_widthw) + xw][3];

			video->putSprite((view_margin_xw + xw) * SPRITE_SIZE, view_margin_y + (yw * SPRITE_SIZE), view_buffer[(yw * view_widthw) + xw][0], view_buffer[(yw * view_widthw) + xw][1], view_buffer[(yw * view_widthw) + xw][2], view_buffer[(yw * view_widthw) + xw][3]);
		}
	}

	video->updateScreen(view_margin_xw * SPRITE_SIZE, view_margin_y, view_widthw * SPRITE_SIZE, view_heightw * SPRITE_SIZE);

	verifyMovement();
}


void Field::operateCharacters()
{
	word character_offset = data->queryWord(header_offset + C5_CHARACTER);

	while (true) {
		byte character_flag = data->queryByte(character_offset + CHARACTER_FLAG);
		if (character_flag == CHARACTER_FLAG_NULL) {
			break;
		}

		if ((character_flag & 0x01) != 0x01) {
			character_offset += C5_CHARACTER_SIZE;
			continue;
		}
		if ((character_flag & CHARACTER_FLAG_DEACTIVATE) == CHARACTER_FLAG_DEACTIVATE) {
			character_offset += C5_CHARACTER_SIZE;
			continue;
		}

		byte operation_count = data->queryByte(character_offset + CHARACTER_OPERATION_COUNT);
		if (operation_count != 0) {
			executeOperation(character_offset);
		}
		else {
			byte unknown_value;
			while (true) {
				word unknown_offset0 = (data->queryByte(character_offset + CHARACTER_OPERATION_OFFSET) * 2) + data->queryWord(header_offset + C5_4);
				word unknown_offset1 = data->queryWord(unknown_offset0) + data->queryByte(character_offset + CHARACTER_OPERATION_OFFSET_INDEX);
				unknown_value = data->queryByte(unknown_offset1);

				if (unknown_value == 0xFF) {
					data->writeByte(character_offset + CHARACTER_OPERATION_OFFSET_INDEX, 0);
					continue;
				}
				else {
					break;
				}
			}
			data->increaseByte(character_offset + CHARACTER_OPERATION_OFFSET_INDEX);

			byte count = unknown_value & MASK_LOWER_BYTE;
			byte type = (unknown_value & MASK_UPPER_BYTE) >> 4;

			if (type != 0x0F) {
				if (type == 0x0C) {
					data->writeByte(character_offset + CHARACTER_FRAME, character_log[count].frame);
					data->writeWord(character_offset + CHARACTER_COORD_XW, character_log[count].coord_xw);
					data->writeWord(character_offset + CHARACTER_COORD_YW, character_log[count].coord_yw);
				}
				else {
					if (type == 0x0E) {
						// rotate carry left and some more
						operation_type += ((operation_type << 2) | (operation_type >> (8 - 2))) + 1;

						type = (operation_type & 0x03) + 1;
					}
					data->writeByte(character_offset + CHARACTER_OPERATION_TYPE, type);

					if (count == 0) {
						count++;
					}
					data->writeByte(character_offset + CHARACTER_OPERATION_COUNT, count);

					executeOperation(character_offset);
				}
			}
			else if (count >= 8) {
				if (count == 8) {
					data->orByte(character_offset + CHARACTER_FLAG, 0x02);
				}
				else if (count == 9) {
					data->andByte(character_offset + CHARACTER_FLAG, 0xFD);
				}
			}
			else {
				data->writeByte(character_offset + CHARACTER_FRAME, count);
			}
		}

		character_offset += C5_CHARACTER_SIZE;
	}
}


word Field::checkEntrance(word character_index)
{
	word entrance_offset = data->queryWord(header_offset + C5_ENTRANCE);

	word character_offset = (character_index * C5_CHARACTER_SIZE) + data->queryWord(header_offset + C5_CHARACTER);
	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);

	while (true) {
		word coord_xw_start = data->queryWord(entrance_offset);
		word coord_yw_start = data->queryWord(entrance_offset + 2);
		word coord_xw_end = data->queryWord(entrance_offset + 4);
		word coord_yw_end = data->queryWord(entrance_offset + 6);
		word entrance = data->queryWord(entrance_offset + 8);

		if (coord_xw_start == ENTRANCE_NOTFOUND) {
			return ENTRANCE_NOTFOUND;
		}
		else {
			if ((character_coord_xw >= coord_xw_start) && (character_coord_xw <= coord_xw_end) && (character_coord_yw >= coord_yw_start) && (character_coord_yw <= coord_yw_end)) {
				return entrance;
			}
			else {
				entrance_offset += C5_ENTRANCE_SIZE;
				continue;
			}
		}
	}
}


word Field::checkDetectCollision(word character_index)
{
	word character_offset = (character_index * C5_CHARACTER_SIZE) + data->queryWord(header_offset + C5_CHARACTER);
	return detectCollision(character_offset);
}


void Field::setEntryInfo(word character_index)
{
	is_path_found = false;

	// read phase
	word entryinfo_offset = data->queryWord(header_offset + C5_ENTRYINFO) + (character_index * C5_ENTRYINFO_SIZE);

	view_coord_xw = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_COORD_XW);
	view_coord_yw = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_COORD_YW);

	word character_coord_xw = data->queryWord(entryinfo_offset + ENTRYINFO_CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(entryinfo_offset + ENTRYINFO_CHARACTER_COORD_YW);
	byte character_frame = (byte) data->queryWord(entryinfo_offset + ENTRYINFO_CHARACTER_FRAME);

	view_leftlimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_LEFTLIMIT);
	view_uplimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_UPLIMIT);
	view_rightlimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_RIGHTLIMIT);
	view_downlimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_DOWNLIMIT);

	// write phase
	data->writeWord(iw_View_CoordXw, view_coord_xw);
	data->writeWord(iw_View_CoordYw, view_coord_yw);

	for (int i = 0; i < 3; i++) {
		word character_offset = data->queryWord(header_offset + C5_CHARACTER) + (i * C5_CHARACTER_SIZE);

		data->writeWord(character_offset + CHARACTER_COORD_XW, character_coord_xw);
		data->writeWord(character_offset + CHARACTER_COORD_YW, character_coord_yw);
		data->writeByte(character_offset + CHARACTER_FRAME, character_frame);
	}

	for (int i = 0; i < CHARACTER_LOG; i++) {
		character_log[i].coord_xw = character_coord_xw;
		character_log[i].coord_yw = character_coord_yw;
		character_log[i].frame = character_frame;
	}

	data->writeWord(iw_View_LeftLimit, view_leftlimit);
	data->writeWord(iw_View_UpLimit, view_uplimit);
	data->writeWord(iw_View_RightLimit, view_rightlimit);
	data->writeWord(iw_View_DownLimit, view_downlimit);
}


void Field::resetEntryInfo(word character_index)
{
	// read phase
	word entryinfo_offset = data->queryWord(header_offset + C5_ENTRYINFO) + (character_index * 18);
	word character_offset = data->queryWord(header_offset + C5_CHARACTER);

	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);
	byte character_frame = (byte) data->queryWord(character_offset + CHARACTER_FRAME);

	view_leftlimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_LEFTLIMIT);
	view_uplimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_UPLIMIT);
	view_rightlimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_RIGHTLIMIT);
	view_downlimit = data->queryWord(entryinfo_offset + ENTRYINFO_VIEW_DOWNLIMIT);

	// write phase
	for (int i = 1; i < 3; i++) {
		character_offset = data->queryWord(header_offset + C5_CHARACTER) + (i * C5_CHARACTER_SIZE);

		data->writeWord(character_offset + CHARACTER_COORD_XW, character_coord_xw);
		data->writeWord(character_offset + CHARACTER_COORD_YW, character_coord_yw);
		data->writeByte(character_offset + CHARACTER_FRAME, character_frame);
	}

	for (int i = 0; i < CHARACTER_LOG; i++) {
		character_log[i].coord_xw = character_coord_xw;
		character_log[i].coord_yw = character_coord_yw;
		character_log[i].frame = character_frame;
	}

	data->writeWord(iw_View_LeftLimit, view_leftlimit);
	data->writeWord(iw_View_UpLimit, view_uplimit);
	data->writeWord(iw_View_RightLimit, view_rightlimit);
	data->writeWord(iw_View_DownLimit, view_downlimit);
}


word Field::checkClick()
{
	has_moved = false;

	operateCharacters();
	initializeMap();
	setCharactersOnMap();
	quickDraw();

	if (input->isLeftClicked()) {
		word coord_x = memory->b_SystemVariable->queryWord(iw_Mouse_CoordX);
		word coord_y = memory->b_SystemVariable->queryWord(iw_Mouse_CoordY);

		if ((coord_x >= SUBMAP_COORDX_START) && (coord_x < SUBMAP_COORDX_END) && (coord_y >= SUBMAP_COORDY_START) && (coord_y < SUBMAP_COORDY_END)) {
			while (input->isLeftClicked()) {
				input->refresh();
			}

			return SUBMAP_CLICKED;
		}
	}

    return verifyMovement();
}


void Field::setCharacterOperationOffset(word character_index, word character_operation_offset)
{
	word character_offset = data->queryWord(header_offset + C5_CHARACTER) + (character_index * C5_CHARACTER_SIZE);

	data->writeByte(character_offset + CHARACTER_OPERATION_OFFSET, (byte) character_operation_offset);
	data->writeByte(character_offset + CHARACTER_OPERATION_OFFSET_INDEX, 0);
	data->writeByte(character_offset + CHARACTER_OPERATION_TYPE, 0);
	data->writeByte(character_offset + CHARACTER_OPERATION_COUNT, 0);
}


void Field::activateCharacter(word character_index, word flag)
{
	word character_offset = data->queryWord(header_offset + C5_CHARACTER) + (character_index * C5_CHARACTER_SIZE);

	if (flag == 0) {
		data->andByte(character_offset + CHARACTER_FLAG, ~CHARACTER_FLAG_DEACTIVATE);
	}
	else {
		data->orByte(character_offset + CHARACTER_FLAG, CHARACTER_FLAG_DEACTIVATE);
	}
}


void Field::unpackMPFile(word mp_offset)
{
	map_widthw = data->queryWord(mp_offset + MAP_WIDTHW);
	map_heightw = data->queryWord(mp_offset + MAP_HEIGHTW);

	data->writeWord(map_offset + MAP_WIDTHW, map_widthw);
	data->writeWord(map_offset + MAP_HEIGHTW, map_heightw);

	word current_mp_offset = mp_offset + 4;
	word current_map_offset = map_offset + 4;

	byte rle_count = 0;
	word sprite_index = 0;
	for (word xw = 0; xw < map_widthw; xw++) {
		for (word yw = 0; yw < map_heightw; yw++) {
			if (rle_count != 0) {
				data->writeWord(current_map_offset, sprite_index);
				current_map_offset += 2;
				rle_count--;
			}
			else {
				sprite_index = data->queryWord(current_mp_offset);
				current_mp_offset += 2;

				if ((sprite_index & MP_RLE_MASK) == MP_RLE_MASK) {
					sprite_index &= ~MP_RLE_MASK;

					rle_count = data->queryByte(current_mp_offset++);

					data->writeWord(current_map_offset, sprite_index);
					current_map_offset += 2;
					rle_count--;
				}
				else {
					data->writeWord(current_map_offset, sprite_index);
					current_map_offset += 2;
				}
			}
		}
	}
}


void Field::copyMapBlock(word source_coord_x0w, word source_coord_y0w, word source_coord_x1w, word source_coord_y1w, word destination_coord_xw, word destination_coord_yw)
{
	word destination_offset = calculateMapOffset(destination_coord_xw, destination_coord_yw);

	word block_widthw = source_coord_x1w - source_coord_x0w + 1;
	word block_heightw = source_coord_y1w - source_coord_y0w + 1;

	for (word yw = 0; yw < block_heightw; yw++) {
		for (word xw = 0; xw < block_widthw; xw++) {
			word source_offset = calculateMapOffset(source_coord_x0w + xw, source_coord_y0w + yw);
			word sprite_index = data->queryWord(source_offset);
			data->writeWord(destination_offset, sprite_index);
		}
	}
}


word Field::makeSetPath(word character_index)
{
	word character_offset = (character_index * C5_CHARACTER_SIZE) + data->queryWord(header_offset + C5_CHARACTER);
	word destination_coord_xw = (word) (data->queryByte(character_offset + CHARACTER_DESTINATION_COORD_XW));
	word destination_coord_yw = (word) (data->queryByte(character_offset + CHARACTER_DESTINATION_COORD_YW));
	setPath(character_offset, destination_coord_xw, destination_coord_yw, true);

	if (is_path_found) {
		return PATH_FOUND;
	}
	else {
		return PATH_NOTFOUND;
	}
}


void Field::clearPathFoundStatus()
{
	is_path_found = false;
}


inline word Field::calculateMapOffset(word coord_xw, word coord_yw)
{
	return (((coord_yw * map_widthw) + coord_xw) * 2) + map_offset + 4;
}


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


inline void Field::toggleFrame(word character_offset)
{
	data->xorByte(character_offset + CHARACTER_FRAME, CHARACTER_FRAME_MASK);
}


#define TW	FILE *test = fopen("c:\\temp\\path.txt", "ab");\
	for (word yw = 0; yw < map_heightw; yw++) {\
	for (word xw = 0; xw < map_widthw; xw++) {\
	byte test_type = data->queryByte(calculatePathOffset(xw, yw));\
	if (test_type == 0xFF) fprintf(test, "[]");\
					else if (test_type == 0xFE) fprintf(test, "  ");\
					else fprintf(test, "%2x", test_type);\
	}\
	fprintf(test, "\n");\
	}\
	fclose(test);


void Field::setPath(word character_offset, word target_coord_xw, word target_coord_yw, bool use_alternative)
{
	if (is_path_found) {
		is_path_found = false;
		return;
	}

	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);

	word start_offset = calculatePathOffset(character_coord_xw, character_coord_yw);
	word target_offset = calculatePathOffset(target_coord_xw, target_coord_yw);

	// initialize path
	initializePath(character_offset);

	//if (data->queryByte(target_offset) != PATH_MARK_CLOSED) {
		data->writeByte(start_offset, PATH_MARK_CHARACTER);

		if ((generatePath(character_offset, target_coord_xw, target_coord_yw, PATH_MARK_INITIAL, PATH_SEQUENCE_DEFAULT) == false)) {
			data->writeByte(start_offset, PATH_MARK_CLOSED);
		}

//TODO: remove this
//TW;

		if (data->queryByte(target_offset) < PATH_MARK_OPENED) {
			is_path_found = true;
			return;
		}
	//}

	if (use_alternative) {
		//TODO: implement alternative pathfinding method
	}

	is_path_found = false;
	return;
}


inline word Field::calculatePathOffset(word coord_xw, word coord_yw)
{
	return ((coord_yw * map_widthw) + coord_xw) + path_offset;
}


inline byte Field::getPathMark(word coord_xw, word coord_yw)
{
	return (data->queryByte(calculatePathOffset(coord_xw, coord_yw)));
}


bool Field::checkPathMark(word coord_xw, word coord_yw, PathDirection direction)
{
	byte current_mark = getPathMark(coord_xw, coord_yw);

	byte next_mark;
	switch (direction) {
		case PATH_DIRECTION_UP_LEFT:
			next_mark = getPathMark(coord_xw - 1, coord_yw - 1);
			break;

		case PATH_DIRECTION_UP_RIGHT:
			next_mark = getPathMark(coord_xw + 1, coord_yw - 1);
			break;

		case PATH_DIRECTION_DOWN_LEFT:
			next_mark = getPathMark(coord_xw - 1, coord_yw + 1);
			break;

		case PATH_DIRECTION_DOWN_RIGHT:
			next_mark = getPathMark(coord_xw + 1, coord_yw + 1);
			break;

		case PATH_DIRECTION_LEFT:
			next_mark = getPathMark(coord_xw - 1, coord_yw);
			break;

		case PATH_DIRECTION_RIGHT:
			next_mark = getPathMark(coord_xw + 1, coord_yw);
			break;

		case PATH_DIRECTION_DOWN:
			next_mark = getPathMark(coord_xw, coord_yw + 1);
			break;

		case PATH_DIRECTION_UP:
			next_mark = getPathMark(coord_xw, coord_yw - 1);
			break;

		default:
			next_mark = PATH_MARK_CLOSED;
	}

	return ((next_mark < PATH_MARK_OPENED) && (next_mark > current_mark));
}


void Field::initializePath(word character_offset)
{
	byte character_heightw = data->queryByte(character_offset + CHARACTER_HEIGHTW);

	for (word yw = (character_heightw - 1); yw < map_heightw; yw++) {
		for (word xw = 0; xw < map_widthw; xw++) {
			word sprite_offset = calculateMapOffset(xw, yw);

			byte path_type;
			if ((data->testByte(sprite_offset + 1, MAP_WALL_MASK)) || (data->testByte(sprite_offset + 3, MAP_WALL_MASK))) {
				path_type = PATH_MARK_CLOSED;
			}
			else {
				path_type = PATH_MARK_OPENED;
			}
			data->writeByte(path_offset + ((yw - (character_heightw - 1)) * map_widthw) + xw, path_type);
		}
	}
}


bool Field::generatePath(word character_offset, word coord_xw, word coord_yw, word mark, word sequence)
{
	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);

	word current_offset = calculatePathOffset(coord_xw, coord_yw);

	// various asserts
	if (data->queryByte(current_offset) == PATH_MARK_CLOSED) {
		return false;
	}

	if (coord_xw < 0 || coord_xw >= (map_widthw - 1) || coord_yw < 0 || coord_yw >= (map_heightw - 1)) {
		return false;
	}

	//TODO: what does this mean?
	if (mark <= PATH_MARK_CHARACTER || mark >= PATH_MARK_OPENED) {
		return false;
	}

	// check found
	if (data->queryByte(current_offset) == PATH_MARK_CHARACTER) {
		data->writeByte(current_offset, (byte) mark);
		return true;
	}

	// make horizontal and vertical sequences
	bool is_first_time = false;
	if (sequence == PATH_SEQUENCE_DEFAULT) {
		is_first_time = true;
		sequence = 0;
	}

	if (coord_yw < character_coord_yw) {
		sequence |= PATH_SEQUENCE_DOWN_MASK;
	}
	else if (coord_yw > character_coord_yw) {
		sequence &= ~PATH_SEQUENCE_DOWN_MASK;
	}

	if (coord_xw < character_coord_xw) {
		sequence |= PATH_SEQUENCE_RIGHT_MASK;
	}
	else if (coord_xw > character_coord_xw) {
		sequence &= ~PATH_SEQUENCE_RIGHT_MASK;
	}

	// make orthogonal sequence
	int vertical_difference = abs(coord_yw - character_coord_yw);
	int horizontal_difference = abs(coord_xw - character_coord_xw);

	int difference = abs(horizontal_difference - vertical_difference);
	if ((difference > PATH_ORTHOGONAL_THRESHOLD) || is_first_time) {
		if (vertical_difference <= horizontal_difference) {
			sequence |= PATH_SEQUENCE_HORIZONTAL_MASK;
		}
		else {
			sequence &= ~PATH_SEQUENCE_HORIZONTAL_MASK;
		}
	}

	// verify sequence
	data->writeByte(current_offset, PATH_MARK_CLOSED);

	if (sequence <= PATH_SEQUENCE_MASK) {
#define GENERATE_PATH_UP	if (generatePath(character_offset, coord_xw, coord_yw - 1, mark - 1, sequence)) {	\
								data->writeByte(current_offset, (byte) mark);	return true;	}
#define GENERATE_PATH_DOWN	if (generatePath(character_offset, coord_xw, coord_yw + 1, mark - 1, sequence)) {	\
								data->writeByte(current_offset, (byte) mark);	return true;	}
#define GENERATE_PATH_LEFT	if (generatePath(character_offset, coord_xw - 1, coord_yw, mark - 1, sequence)) {	\
								data->writeByte(current_offset, (byte) mark);	return true;	}
#define GENERATE_PATH_RIGHT	if (generatePath(character_offset, coord_xw + 1, coord_yw, mark - 1, sequence)) {	\
								data->writeByte(current_offset, (byte) mark);	return true;	}

		switch (sequence) {
			case 0x0000:
				{
					GENERATE_PATH_UP;
					GENERATE_PATH_LEFT;
					GENERATE_PATH_RIGHT;
					GENERATE_PATH_DOWN;
				}
				break;

			case 0x0001:
				{
					GENERATE_PATH_DOWN;
					GENERATE_PATH_LEFT;
					GENERATE_PATH_RIGHT;
					GENERATE_PATH_UP;
				}
				break;

			case 0x0010:
				{
					GENERATE_PATH_UP;
					GENERATE_PATH_RIGHT;
					GENERATE_PATH_LEFT;
					GENERATE_PATH_DOWN;
				}
				break;

			case 0x0011:
				{
					GENERATE_PATH_DOWN;
					GENERATE_PATH_RIGHT;
					GENERATE_PATH_LEFT;
					GENERATE_PATH_UP;
				}
				break;

			case 0x0100:
				{
					GENERATE_PATH_LEFT;
					GENERATE_PATH_UP;
					GENERATE_PATH_DOWN;
					GENERATE_PATH_RIGHT;
				}
				break;

			case 0x0101:
				{
					GENERATE_PATH_LEFT;
					GENERATE_PATH_DOWN;
					GENERATE_PATH_UP;
					GENERATE_PATH_RIGHT;
				}
				break;

			case 0x0110:
				{
					GENERATE_PATH_RIGHT;
					GENERATE_PATH_UP;
					GENERATE_PATH_DOWN;
					GENERATE_PATH_LEFT;
				}
				break;

			case 0x0111:
				{
					GENERATE_PATH_RIGHT;
					GENERATE_PATH_DOWN;
					GENERATE_PATH_UP;
					GENERATE_PATH_LEFT;
				}
				break;
		}

		return false;
	}
	else {
		data->writeByte(current_offset, (byte) mark);
		return true;
	}
}


void Field::moveCharacterOnPath(word character_offset)
{
	word character_coord_xw = data->queryWord(character_offset + CHARACTER_COORD_XW);
	word character_coord_yw = data->queryWord(character_offset + CHARACTER_COORD_YW);

	byte current_mark = getPathMark(character_coord_xw, character_coord_yw);
	if (current_mark >= PATH_MARK_INITIAL) {
		is_path_found = false;
		return;
	}

	// check up-left
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_UP_LEFT)) {
		// move up and left
		if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_UP)) {
			is_path_found = moveCharacterUp(character_offset) && moveCharacterLeft(character_offset);
			return;
		}
		// move left and up
		else if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_LEFT)) {
			is_path_found = moveCharacterLeft(character_offset) && moveCharacterUp(character_offset);
			return;
		}
	}

	// check up-right
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_UP_RIGHT)) {
		// move up and right
		if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_UP)) {
			is_path_found = moveCharacterUp(character_offset) && moveCharacterRight(character_offset);
			return;
		}
		// move right and up
		else if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_RIGHT)) {
			is_path_found = moveCharacterRight(character_offset) && moveCharacterUp(character_offset);
			return;
		}
	}

	// check down-right
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_DOWN_RIGHT)) {
		// move down and right
		if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_DOWN)) {
			is_path_found = moveCharacterDown(character_offset) && moveCharacterRight(character_offset);
			return;
		}
		// move right and down
		else if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_RIGHT)) {
			is_path_found = moveCharacterRight(character_offset) && moveCharacterDown(character_offset);
			return;
		}
	}

	// check down-left
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_DOWN_LEFT)) {
		// move down and left
		if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_DOWN)) {
			is_path_found = moveCharacterDown(character_offset) && moveCharacterLeft(character_offset);
			return;
		}
		// move left and down
		else if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_LEFT)) {
			is_path_found = moveCharacterLeft(character_offset) && moveCharacterDown(character_offset);
			return;
		}
	}

	// move left
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_LEFT)) {
		is_path_found = moveCharacterLeft(character_offset);
		return;
	}

	// move right
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_RIGHT)) {
		is_path_found = moveCharacterRight(character_offset);
		return;
	}

	// move down
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_DOWN)) {
		is_path_found = moveCharacterDown(character_offset);
		return;
	}

	// move up
	if (checkPathMark(character_coord_xw, character_coord_yw, PATH_DIRECTION_UP)) {
		is_path_found = moveCharacterUp(character_offset);
		return;
	}
}


void Field::saveCharacterLog(word character_offset, byte character_frame, word character_coord_xw, word character_coord_yw)
{
	is_path_found = false;

	toggleFrame(character_offset);

	for (int i = (CHARACTER_LOG - 2); i >= 0 ; i--) {
		character_log[i + 1].frame = character_log[i].frame;
		character_log[i + 1].coord_xw = character_log[i].coord_xw;
		character_log[i + 1].coord_yw = character_log[i].coord_yw;
	}

	character_log[0].frame = character_frame;
	character_log[0].coord_xw = character_coord_xw;
	character_log[0].coord_yw = character_coord_yw;
}