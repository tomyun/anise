#include "field.h"

Field::Field(Memory *memory, Input *input, Video *video, Option *option)
{
	this->memory = memory;
	this->input = input;
	this->video = video;
	this->option = option;
	
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

	if ((option->game_type == GAME_NANPA2) && input->isLeftClicked()) {
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


word Field::calculateMapOffset(word coord_xw, word coord_yw)
{
	return (((coord_yw * map_widthw) + coord_xw) * 2) + map_offset + 4;
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
