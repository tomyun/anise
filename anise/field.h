#ifndef FIELD_H
#define FIELD_H

#include "memory.h"
#include "video.h"
#include "input.h"
#include "option.h"

#define VIEW_SPRITES					1000
#define VIEW_LAYERS						4
#define TILE_SIZE						8
#define CHARACTER_LOG					10
#define C5_CHARACTER_SIZE				16
#define C5_ENTRANCE_SIZE				10
#define C5_ENTRYINFO_SIZE				18

#define COLLISION_NOTDETECTED			NONE
#define ENTRANCE_NOTFOUND				NONE
#define PATH_NOTFOUND					NONE

#define PATH_FOUND						0

#define SUBMAP_COORDX_START				248
#define SUBMAP_COORDX_END				392
#define SUBMAP_COORDY_START				288
#define SUBMAP_COORDY_END				320
#define SUBMAP_CLICKED					0xFFFE

#define MAP_WALL_MASK					0x80

#define PATH_MARK_CLOSED				0xFF
#define PATH_MARK_OPENED				0xFE
#define PATH_MARK_INITIAL				0xFD
#define PATH_MARK_CHARACTER				0x00

#define PATH_SEQUENCE_DEFAULT			0xFFFF
#define PATH_SEQUENCE_DOWN_MASK			0x0001
#define PATH_SEQUENCE_RIGHT_MASK		0x0010
#define PATH_SEQUENCE_HORIZONTAL_MASK	0x0100
#define PATH_SEQUENCE_MASK				0x0111

#define PATH_ORTHOGONAL_THRESHOLD		3

#define CHARACTER_FRAME_MASK			0x01

#define CHARACTER_CLICK_RANGE			48

#define MP_RLE_MASK						0x4000

enum C5Struct {
	C5_FUNCTION3_0 = 0x28,
	C5_FUNCTION3_2 = 0x2A,
	C5_4 = 0x2C,
	C5_CHARACTER = 0x2E,
	C5_ENTRANCE = 0x30,
	C5_ENTRYINFO = 0x32
};


enum MapStruct {
	MAP_WIDTHW = 0,
	MAP_HEIGHTW = 2
};


enum CharacterStruct {
	CHARACTER_FLAG = 0,
	CHARACTER_COORD_XW = 1,
	CHARACTER_COORD_YW = 3,
	CHARACTER_DESTINATION_COORD_XW = 5,
	CHARACTER_6 = 6,
	CHARACTER_OPERATION_OFFSET = 7,
	CHARACTER_OPERATION_OFFSET_INDEX = 8,
	CHARACTER_9 = 9,
	CHARACTER_FRAME = 10,
	CHARACTER_WIDTHW = 11,
	CHARACTER_HEIGHTW = 12,
	CHARACTER_OPERATION_TYPE = 13,
	CHARACTER_OPERATION_COUNT = 14,
	CHARACTER_DESTINATION_COORD_YW = 15
};


enum EntryInfoStruct {
	ENTRYINFO_VIEW_COORD_XW = 0,
	ENTRYINFO_VIEW_COORD_YW = 2,
	ENTRYINFO_CHARACTER_COORD_XW = 4,
	ENTRYINFO_CHARACTER_COORD_YW = 6,
	ENTRYINFO_CHARACTER_FRAME = 8,
	ENTRYINFO_VIEW_LEFTLIMIT = 10,
	ENTRYINFO_VIEW_UPLIMIT = 12,
	ENTRYINFO_VIEW_RIGHTLIMIT = 14,
	ENTRYINFO_VIEW_DOWNLIMIT = 16
};


enum OperationTypeStruct {
	OPERATION_MOVE_UP = 0,
	OPERATION_MOVE_DOWN = 1,
	OPERATION_MOVE_LEFT = 2,
	OPERATION_MOVE_RIGHT = 3,
	OPERATION_TOGGLE_FRAME = 4,
	OPERATION_MOVE = 5,
	OPERATION_NULL = 6,
	OPERATION_MOVE_LEFT_UP = 7,
	OPERATION_MOVE_RIGHT_UP = 8,
	OPERATION_MOVE_LEFT_DOWN = 9,
	OPERATION_MOVE_RIGHT_DOWN = 10,
	OPERATION_RESTART_ENGINE = 11,
	OPERATION_AUTOMOVE = 12
};


enum CharacterFlag {
	CHARACTER_FLAG_CHECKSCROLL = 0x04,
	CHARACTER_FLAG_INVISIBLE = 0x08,
	CHARACTER_FLAG_NOCOLLISION = 0x10,
	CHARACTER_FLAG_DEACTIVATE = 0x80,
	CHARACTER_FLAG_NULL = 0xFF
};


enum CharacterDirection {
	CHARACTER_DIRECTION_UP = 0,
	CHARACTER_DIRECTION_DOWN = 1,
	CHARACTER_DIRECTION_LEFT = 2,
	CHARACTER_DIRECTION_RIGHT = 3
};


enum CharacterFrame {
	CHARACTER_FRAME_UP = 0x00,
	CHARACTER_FRAME_DOWN = 0x02,
	CHARACTER_FRAME_LEFT = 0x04,
	CHARACTER_FRAME_RIGHT = 0x06
};


enum PathDirection {
	PATH_DIRECTION_UP_LEFT,
	PATH_DIRECTION_UP_RIGHT,
	PATH_DIRECTION_DOWN_LEFT,
	PATH_DIRECTION_DOWN_RIGHT,
	PATH_DIRECTION_LEFT,
	PATH_DIRECTION_RIGHT,
	PATH_DIRECTION_DOWN,
	PATH_DIRECTION_UP
};


class Field {
private:
	Memory *memory;
	Video *video;
	Input *input;
	Option *option;

	MemoryBlock *data;
	MemoryBlock *map;

	word header_offset;
	word map_offset;
	word path_offset;

	bool is_path_found;
	bool has_moved;

	word view_leftlimit;
	word view_uplimit;
	word view_rightlimit;
	word view_downlimit;

	word view_margin_xw;
	word view_margin_y;
	word view_widthw;
	word view_heightw;

	word character_leftlimit;
	word character_uplimit;
	word character_rightlimit;
	word character_downlimit;

	word view_coord_xw;
	word view_coord_yw;

	word map_widthw;
	word map_heightw;

	word view[VIEW_SPRITES][VIEW_LAYERS];
	word view_buffer[VIEW_SPRITES][VIEW_LAYERS];

	byte operation_type;	//TODO: figure it out

	word movement_direction;
	word movement_collision;
	word movement_entrance;

	struct CharacterLogStruct {
		byte frame;
		word coord_xw;
		word coord_yw;
	} character_log[CHARACTER_LOG];

	word calculateMapOffset(word coord_xw, word coord_yw);
	void saveCharacterLog(word character_offset, byte character_frame, word character_coord_xw, word character_coord_yw);

	// character movement related stuffs (field_move.cpp)
	word verifyMovement();
	word detectCollision(word character_offset);
	void executeOperation(word character_offset);

	bool moveCharacterUp(word character_offset);
	bool moveCharacterDown(word character_offset);
	bool moveCharacterLeft(word character_offset);
	bool moveCharacterRight(word character_offset);

	void toggleFrame(word character_offset);

	// pathfinding related stuffs (field_path.cpp)
	void setPath(word character_offset, word target_coord_xw, word target_coord_yw, bool use_alternatve);

	void initializePath(word character_offset);
	bool generatePath(word character_offset, word coord_xw, word coord_yw, word mark, word sequence);
	void moveCharacterOnPath(word character_offset);

	word calculatePathOffset(word coord_xw, word coord_yw);
	byte getPathMark(word coord_xw, word coord_yw);
	bool checkPathMark(word coord_xw, word coord_yw, PathDirection direction);

public:
	Field(Memory *memory, Video *video, Input *input, Option *option);
	~Field();

	void initialize();

	word loadMapFile();
	void loadC5File();
	void initializeMap();
	void setCharactersOnMap();
	void quickDraw();
	void draw();
	void operateCharacters();
	word checkEntrance(word character_index);
	word checkDetectCollision(word character_index);
	void setEntryInfo(word character_index);
	void resetEntryInfo(word character_index);
	word checkClick();
	void setCharacterOperationOffset(word character_index, word character_operation_offset);
	void activateCharacter(word character_index, word flag);
	void unpackMPFile(word mp_offset);
	void copyMapBlock(word source_coord_x0w, word source_coord_y0w, word source_coord_x1w, word source_coord_y1w, word destination_coord_xw, word destination_coord_yw);
	word makeSetPath(word character_index);
	void clearPathFoundStatus();
};

#endif
