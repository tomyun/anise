#include "animation.h"

Animation::Animation(Memory *memory, Timer *timer, Input *input, Video *video)
{
	this->memory = memory;
	this->timer = timer;
	this->input = input;
	this->video = video;

	b_Slot = memory->b_AnimationSlot;
}


Animation::~Animation()
{
}


word Animation::getSlotOffset(word slot_index)
{
	return (memory->animation_slot_entry + (slot_index * SLOT_SIZE));
}


byte Animation::getSlotStatus(word slot_offset)
{
	return b_Slot->queryByte(slot_offset + ib_Status);
}


word Animation::getSlotEntryOffset(word slot_offset)
{
	return b_Slot->queryWord(slot_offset + iwpo_Entry);
}


word Animation::getSlotCurrentOffset(word slot_offset)
{
	return b_Slot->queryWord(slot_offset + iwpo_Current);
}


byte Animation::getSlotDelayFrame(word slot_offset)
{
	return b_Slot->queryByte(slot_offset + ib_Delay);
}


void Animation::setSlotStatus(word slot_offset, byte status)
{
	b_Slot->writeByte(slot_offset + ib_Status, status);
}


void Animation::setSlotEntryOffset(word slot_offset, word entry_offset)
{
	b_Slot->writeWord(slot_offset + iwpo_Entry, entry_offset);
}


void Animation::setSlotCurrentOffset(word slot_offset, word current_offset)
{
	b_Slot->writeWord(slot_offset + iwpo_Current, current_offset);
}


void Animation::setSlotDelayFrame(word slot_offset, byte delay_frame)
{
	b_Slot->writeByte(slot_offset + ib_Delay, delay_frame);
}


void Animation::decreaseSlotDelayFrame(word slot_offset)
{
	b_Slot->writeByte(slot_offset + ib_Delay, (getSlotDelayFrame(slot_offset) - 1));
}


void Animation::initializeBlit(word script_offset, BlitStruct *order)
{
	word source_coord_x0b = b_Script->queryByte(script_offset + SOURCE_COORD_X0B);
	word source_coord_y0 = b_Script->queryWord(script_offset + SOURCE_COORD_Y0);
	word source_coord_x1b = b_Script->queryByte(script_offset + SOURCE_COORD_X1B);
	word source_coord_y1 = b_Script->queryWord(script_offset + SOURCE_COORD_Y1);
	word destination_coord_x = b_Script->queryByte(script_offset + DESTINATION_COORD_XB);
	word destination_coord_y = b_Script->queryWord(script_offset + DESTINATION_COORD_Y);

	word width = (source_coord_x1b - source_coord_x0b + 1) << 3;
	word height = source_coord_y1 - source_coord_y0 + 1;

	order->source.x = source_coord_x0b << 3;
	order->source.y = source_coord_y0;
	order->source.s = video->getSurface((b_Script->queryByte(script_offset) >> 1) & 1);
	order->destination.x = destination_coord_x << 3;
	order->destination.y = destination_coord_y;
	order->destination.s = video->getSurface(b_Script->queryByte(script_offset) & 1);
	order->w = width;
	order->h = height;
}


bool Animation::parse(word slot_offset)
{
	bool done = false;
	while (!done) {
		if (getSlotStatus(slot_offset) == STATUS_DEACTIVATED) {
			break;
		}

		if (getSlotDelayFrame(slot_offset) != 0) {
			decreaseSlotDelayFrame(slot_offset);
			break;
		}

		word current_offset = getSlotCurrentOffset(slot_offset);
		b_Script = memory->s_Core->get(&current_offset);
		byte code = b_Script->queryByte(current_offset++);
		switch (code) {
			case CODE_ANIMATION_01:
				{
					if (getSlotStatus(slot_offset) == STATUS_1) {
						setSlotStatus(slot_offset, STATUS_DEACTIVATED);
					}
				}
				done = true;
				break;

			case CODE_ANIMATION_SETDELAY:
				{
					byte delay_frame = b_Script->queryByte(current_offset++);
					setSlotDelayFrame(slot_offset, delay_frame);
				}
				done = true;
				break;

			case CODE_ANIMATION_REWIND:
				{
					current_offset = getSlotEntryOffset(slot_offset) - b_Script->getEntry();
					//current_offset = getSlotEntryOffset(slot_offset);
					//b_Script = memory->s_Core->get(&current_offset);
				}
				done = true;
				break;

			case CODE_ANIMATION_STOP_04:
			case CODE_ANIMATION_STOP_FF:
				{
					setSlotStatus(slot_offset, STATUS_DEACTIVATED);
				}
				done = true;
				break;

			case CODE_ANIMATION_05:
				{
					code = b_Script->queryByte(current_offset++);

					b_Slot->writeByte(slot_offset + ib_10, code);
					b_Slot->writeWord(slot_offset + iwpo_6, current_offset + b_Script->getEntry());
				}
				done = true;
				break;

			case CODE_ANIMATION_06:
				{
					if (b_Slot->decreaseByte(slot_offset + ib_10) != 0) {
						current_offset = b_Slot->queryWord(slot_offset + iwpo_6) - b_Script->getEntry();
					}
				}
				done = true;
				break;

			case CODE_ANIMATION_07:
				{
					code = b_Script->queryByte(current_offset++);

					b_Slot->writeByte(slot_offset + ib_11, code);
					b_Slot->writeWord(slot_offset + iwpo_8, current_offset + b_Script->getEntry());
				}
				done = true;
				break;

			case CODE_ANIMATION_08:
				{
					if (b_Slot->decreaseByte(slot_offset + ib_11) != 0) {
						current_offset = b_Slot->queryWord(slot_offset + iwpo_8) - b_Script->getEntry();
					}
				}
				done = true;
				break;

			default:
				{
					word operation_offset = (code - CODE_ANIMATION_OPERATION) * OPERATION_SIZE;
					word script_entry = memory->b_SystemVariable->queryWord(iwpo_AnimationScript);
					b_Script = memory->s_Core->get(&script_entry);
					byte script_index = b_Script->queryByte(script_entry);
					word script_offset = script_entry + ((script_index * 2) + 1) + operation_offset;

					code = b_Script->queryByte(script_offset);
					code &= 0xF0;
					switch (code) {
						case CODE_ANIMATION_BLITDIRECT:
							{
								BlitStruct order;
								initializeBlit(script_offset, &order);
								video->blitDirect(&order);
							}
							break;

						case CODE_ANIMATION_BLITMASKED:
							{
								BlitStruct order;
								initializeBlit(script_offset, &order);
								video->blitMasked(&order);
							}
							break;

						case CODE_ANIMATION_BLITSWAPPED:
							{
								BlitStruct order;
								initializeBlit(script_offset, &order);
								video->blitSwapped(&order);
							}
							break;

						case CODE_ANIMATION_SETCOLOR:
							{
								byte color_index = b_Script->queryByte(script_offset) & COLOR_MASK;
								word color = b_Script->queryWord(script_offset + 1);

								video->setIntermediateColor(color_index, color);
								video->setColor(color_index, color);
								video->updateScreen();

								for (int i = 0; i < VIDEO_COLOR; i++) {
									memory->b_SystemVariable->writeWord(iw_Video_Palette0 + (i * 2), video->getIntermediateColor(i));
								}
							}
							break;

						case CODE_ANIMATION_BLITMERGED:
							{
								byte mode = b_Script->queryByte(script_offset);
								word foreground_coord_x0b = b_Script->queryByte(script_offset + 1);
								word foreground_coord_y0 = b_Script->queryWord(script_offset + 2);
								word foreground_coord_x1b = b_Script->queryByte(script_offset + 4);
								word foreground_coord_y1 = b_Script->queryWord(script_offset + 5);
								word background_coord_xb = b_Script->queryByte(script_offset + 7);
								word background_coord_y = b_Script->queryWord(script_offset + 8);
								word destination_coord_xb = b_Script->queryByte(script_offset + 10);
								word destination_coord_y = b_Script->queryWord(script_offset + 11);

								word width = (foreground_coord_x1b - foreground_coord_x0b + 1) << 3;
								word height = foreground_coord_y1 - foreground_coord_y0 + 1;

								word foreground_coord_x = foreground_coord_x0b << 3;
								word background_coord_x = background_coord_xb << 3;
								word destination_coord_x = destination_coord_xb << 3;
								word foreground_coord_y = foreground_coord_y0;

								video->blitMerged(mode, foreground_coord_x, foreground_coord_y, background_coord_x, background_coord_y, destination_coord_x, destination_coord_y, width, height);
							}
							break;

						case CODE_ANIMATION_ERROR:
							{
								BlitStruct order;
								initializeBlit(script_offset, &order);
								//TODO: process exit
								PRINT_ERROR("[Animation::parse()] invalid code: %x\n", code);
								return false;
							}
							break;

						case CODE_ANIMATION_SETPALETTE:
							{
								script_offset++;

								for (int i = 0; i < VIDEO_COLOR; i++) {
									word color = b_Script->queryWord(script_offset);
									video->setIntermediateColor(i, color);
									memory->b_SystemVariable->writeWord(iw_Video_Palette0 + (i * 2), color);
									script_offset += 2;
								}

								video->setPalette();
							}
							break;

						default:
							done = true;
					}
				}
		}

		setSlotCurrentOffset(slot_offset, b_Script->getEntry() + current_offset);
	}

	b_Script = NULL;

	return true;
}


bool Animation::setSlot(word function, word slot_index, word parameter_2, word parameter_3)
{
	word slot_offset = getSlotOffset(slot_index);
	b_Slot = memory->s_Core->get(&slot_offset);

	switch (function) {
		case SLOT_INITIALIZE:
			{
				word script_index = parameter_2;
				word script_entry = memory->b_SystemVariable->queryWord(iwpo_AnimationScript);
				word script_offset = script_entry + (script_index * 2) + 1;

				MemoryBlock *source = memory->s_Core->get(&script_offset);
				word offset = source->queryWord(script_offset);

				b_Slot->writeWord(slot_offset + iwpo_Entry, script_entry + offset);
				b_Slot->writeWord(slot_offset + iwpo_Current, script_entry + offset);
				b_Slot->writeByte(slot_offset + ib_Status, 0);
				b_Slot->writeByte(slot_offset + ib_Delay, 0);
				b_Slot->writeByte(slot_offset + ib_10, 0);
				b_Slot->writeByte(slot_offset + ib_11, 0);
				b_Slot->writeWord(slot_offset + iwpo_Buffer, 0);
				b_Slot->writeWord(slot_offset + iwpo_VRAM, 0);
			}
			break;

		case SLOT_ACTIVATE:
			{
				setSlotStatus(slot_offset, STATUS_ACTIVATED);
				word entry_offset = getSlotEntryOffset(slot_offset);
				setSlotCurrentOffset(slot_offset, entry_offset);
			}
			break;

		case SLOT_FINALIZE:
			{
				if (getSlotStatus(slot_offset) != STATUS_DEACTIVATED) {
					setSlotStatus(slot_offset, STATUS_1);
				}
			}
			break;

		case SLOT_DEACTIVATE:
			{
				setSlotStatus(slot_offset, STATUS_DEACTIVATED);
			}
			break;

		case SLOT_PLAY:
			{
				setSlotStatus(slot_offset, STATUS_ACTIVATED);
				word entry_offset = getSlotEntryOffset(slot_offset);
				setSlotCurrentOffset(slot_offset, entry_offset);

				do {
					dword tick;
					do {
						tick = timer->checkFrameTimer();
					} while (tick <= FRAME_DELAY);

					if (parse(slot_offset) == false) {
						return false;
					}

					//TODO: debugmode
					show();
					if (input->refresh() == false) {
						break;
					}

					timer->resetFrameTimer();
				} while (getSlotStatus(slot_offset) != STATUS_DEACTIVATED);
			}
			break;

		case SLOT_SETDISPLAYBUFFER:
			{
				b_Slot->writeWord(slot_offset + iwpo_Buffer, parameter_2);
				b_Slot->writeWord(slot_offset + iwpo_VRAM, parameter_3);
			}
			break;

		case SLOT_FINALIZEALL:
			{
				slot_offset = ia_Slot1;

				for (int i = 0; i < 7; i++) {
					if (getSlotStatus(slot_offset + (i * SLOT_SIZE)) != STATUS_DEACTIVATED) {
						setSlotStatus(slot_offset + (i * SLOT_SIZE), STATUS_1);
					}
				}
			}
			break;

		case SLOT_CLEAR:
			{
				slot_offset = ia_Slot0;
				word entry_offset = memory->animation_slot_entry + ia_Terminator;

				for (int i = 0; i < SLOTS; i++) {
					setSlotStatus(slot_offset + (i * SLOT_SIZE), STATUS_DEACTIVATED);
					setSlotEntryOffset(slot_offset + (i * SLOT_SIZE), entry_offset);
				}
			}
			break;
	}

	return true;
}


void Animation::clearSlot()
{
	setSlot(SLOT_CLEAR, 0);
}


bool Animation::show()
{
	if (memory->b_SystemVariable->testByte(ibf_DisabledStatus, DISABLE_ANIMATION)) {
		return true;
	}

	dword tick = timer->checkFrameTimer();
	if (tick < 1) {
		return true;
	}

	for (int i = 0; i < SLOTS; i++) {
		word slot_offset = getSlotOffset(i);
		b_Slot = memory->s_Core->get(&slot_offset);
		
		if (parse(slot_offset) == false) {
			return false;
		};
	}

	timer->resetFrameTimer();

	return true;
}
