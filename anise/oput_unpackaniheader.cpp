#include "script.h"

enum ANIFileOffset {
	ANI_ENTRY_OFFSET = 0,
	ANI_HEADER_OFFSET = 2
};


enum UnpackANIHeaderDecode {
	ANI_DECODE_DIRECTCOPY = 0,
	ANI_DECODE_RLE = 1,
	DISTANCE_MASK = 0x0FFF,
	LENGTH_MASK = MASK_UPPER_BYTE
};


SCRIPTCALL Script::oput_unpackAniHeader()
{
	word source_offset = ANI_ENTRY_OFFSET;
	word destination_offset = memory->b_SystemVariable->queryWord(iwpo_AnimationScript);

	MemoryBlock *source = memory->s_Resources->get(&source_offset);
	MemoryBlock *destination = memory->s_Core->get(&destination_offset);

	word image_entry = source->queryWord(source_offset);
	word header_size = image_entry;

	//TODO: clean it up
	source->set(ANI_HEADER_OFFSET, BIT_FLOW_RIGHT);
	byte mode_byte = source->readByte();
	byte mode_index = 0;
	while (source->getCurrentIndex() < header_size) {
		if (mode_index > 7) {
			mode_byte = source->readByte();
			mode_index = 0;
		}
		byte mode = (mode_byte >> mode_index++) & 1;

		switch (mode) {
						case ANI_DECODE_DIRECTCOPY:
							{
								byte data = source->readByte();
								destination->writeByte(destination_offset++, data);
							}
							break;

						case ANI_DECODE_RLE:
							{
								word data = source->readWord();
								word distance = data & DISTANCE_MASK;
								byte length = (data >> 8) & LENGTH_MASK;

								if (length == LENGTH_MASK) {
									length = source->readByte();
								}
								else {
									length = (length >> 4) + 3;
								}

								word replica_offset = destination_offset - distance;
								for (byte i = 0; i < length; i++) {
									byte data = destination->queryByte(replica_offset++);
									destination->writeByte(destination_offset++, data);
								}
							}
							break;
		}
	}

	word gp4_entry = memory->b_SystemVariable->queryWord(iwpo_GP4File);
	word display_buffer = memory->b_SystemVariable->queryWord(iw_DisplayBuffer);

	memory->b_SystemVariable->writeWord(iwpo_GP4File, image_entry);
	memory->b_SystemVariable->writeWord(iw_DisplayBuffer, SURFACE_BUFFER1);

	memory->b_SystemVariable->orByte(ibf_DisabledStatus, DISABLE_PRESERVEIMAGEPALETTE);

	image->load(NULL);

	memory->b_SystemVariable->andByte(ibf_DisabledStatus, ~DISABLE_PRESERVEIMAGEPALETTE);

	memory->b_SystemVariable->writeWord(iwpo_GP4File, gp4_entry);
	memory->b_SystemVariable->writeWord(iw_DisplayBuffer, display_buffer);

	//TODO: save return value, is this necessary?
	memory->b_BaseVariable->writeWord(iw_Selection, gp4_entry);

	deleteParameter();

	return RETURN_NORMAL;
}