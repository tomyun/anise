#include "field.h"

#define DUMPPATH	FILE *test = fopen("c:\\temp\\path.txt", "ab");\
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
	if (config->game_type != GAME_NANPA2) {
		return;
	}

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

	if (data->queryByte(target_offset) != PATH_MARK_CLOSED) {
		data->writeByte(start_offset, PATH_MARK_CHARACTER);

		if ((generatePath(character_offset, target_coord_xw, target_coord_yw, PATH_MARK_INITIAL, PATH_SEQUENCE_DEFAULT) == false)) {
			data->writeByte(start_offset, PATH_MARK_CLOSED);
		}

//TODO: remove this
//DUMPPATH;

		if (data->queryByte(target_offset) < PATH_MARK_OPENED) {
			is_path_found = true;
			return;
		}
	}

	if (use_alternative) {
		//TODO: implement alternative pathfinding method
	}

	is_path_found = false;
	return;
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

	//TODO: need clean up
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