#include "memory.h"

Memory::Memory(Option *option)
{
	this->option = option;

	initialize();
}


Memory::~Memory()
{
	delete b_Script;
	delete b_ScriptName;
	delete b_Variable;
	delete b_BaseVariable;
	delete b_SystemVariable;
	delete b_Heap;
	delete b_SelectionItem;
	delete b_Procedure;
	delete b_AnimationSlot;

	delete s_Core;
	delete s_Resources;
	delete s_FieldData;
	delete s_FieldMap;
}


bool Memory::initialize()
{
	// set default values
	variable_size = option->variable_size;

	script_entry = SCRIPT_ENTRY;
	script_name_entry = script_entry + SCRIPT_SIZE;
	variable_entry = script_name_entry + SCRIPT_NAME_SIZE;
	base_variable_entry = variable_entry + variable_size;
	system_variable_entry = base_variable_entry + BASE_VARIABLE_SIZE;
	heap_entry = system_variable_entry + SYSTEM_VARIABLE_SIZE;
	selection_item_entry = option->selection_item_entry;
	procedure_entry = option->procedure_entry;
	animation_slot_entry = option->animation_slot_entry;
	animation_script_entry = option->animation_script_entry;
	flag_entry = script_name_entry;

	// make memory segments
	s_Core = new MemorySegment(0, SEGMENT_SIZE);
	s_FieldData = new MemorySegment(0, SEGMENT_SIZE);
	s_FieldMap = new MemorySegment(0, SEGMENT_SIZE);
	s_Resources = new MemorySegment(0, SEGMENT_SIZE);

	// make memory blocks for saving .MES script file
	b_Script = new MemoryBlock(script_entry, SCRIPT_SIZE);
	b_ScriptName = new MemoryBlock(script_name_entry, SCRIPT_NAME_SIZE);
	b_Variable = new MemoryBlock(variable_entry, variable_size);
	b_BaseVariable = new MemoryBlock(base_variable_entry, BASE_VARIABLE_SIZE);
	b_SystemVariable = new MemoryBlock(system_variable_entry, SYSTEM_VARIABLE_SIZE);
	b_Heap = new MemoryBlock(heap_entry, FLAG_SIZE - (heap_entry - flag_entry));
	b_SelectionItem = new MemoryBlock(selection_item_entry, SELECTION_ITEM_SIZE);
	b_Procedure = new MemoryBlock(procedure_entry, PROCEDURE_SIZE);
	b_AnimationSlot = new MemoryBlock(animation_slot_entry, ANIMATION_SLOT_SIZE);

	// connect each memory memory_block to its segment;
	s_Core->add(b_Script);
	s_Core->add(b_ScriptName);
	s_Core->add(b_Variable);
	s_Core->add(b_BaseVariable);
	s_Core->add(b_SystemVariable);
	s_Core->add(b_Heap);
	s_Core->add(b_SelectionItem);
	s_Core->add(b_Procedure);
	s_Core->add(b_AnimationSlot);

	// initialize variables with their default values
	// BaseVariable
	b_BaseVariable->writeWord(iw_SystemVariable, system_variable_entry);

	// SystemVariable
	b_SystemVariable->writeWord(iwpo_Heap, heap_entry);				// SysVar0
	b_SystemVariable->writeWord(iwpo_AnimationScript, animation_script_entry);	// SysVar2
	b_SystemVariable->writeWord(iwpo_MFile, 0xA000);				// SysVar3
	b_SystemVariable->writeWord(iwpo_SysVar4, 0xA000);				// SysVar4
	b_SystemVariable->writeByte(ibf_Selection_Status, 0x7E);			// SysVar9u
	b_SystemVariable->writeWord(iw_Video_MaxCoordXb, 79);				// SysVar15
	b_SystemVariable->writeWord(iw_Video_MaxCoordY, 399);				// SysVar16
	b_SystemVariable->writeWord(iw_Video_Color, 7);					// SysVar19
	b_SystemVariable->writeByte(ib_Dialogue_FontHeight, 16);			// SysVar21l
	b_SystemVariable->writeByte(ib_Dialogue_FontWidthb, 2);				// SysVar21u
	b_SystemVariable->writeWord(iwpo_Selection_Item, selection_item_entry);		// SysVar22
	b_SystemVariable->writeWord(iw_SysVar29, 0xFFFF);				// SysVar29
	b_SystemVariable->writeWord(iw_SysVar30, 0xFFFF);				// SysVar30
	b_SystemVariable->writeWord(iw_Video_Palette0, 0x0000);				// SysVar31
	b_SystemVariable->writeWord(iw_Video_Palette1, 0x000F);				// SysVar32
	b_SystemVariable->writeWord(iw_Video_Palette2, 0x00F0);				// SysVar33
	b_SystemVariable->writeWord(iw_Video_Palette3, 0x00FF);				// SysVar34
	b_SystemVariable->writeWord(iw_Video_Palette4, 0x0F00);				// SysVar35
	b_SystemVariable->writeWord(iw_Video_Palette5, 0x0F0F);				// SysVar36
	b_SystemVariable->writeWord(iw_Video_Palette6, 0x0FF0);				// SysVar37
	b_SystemVariable->writeWord(iw_Video_Palette7, 0x0FFF);				// SysVar38
	b_SystemVariable->writeWord(iw_Video_Palette8, 0x0000);				// SysVar39
	b_SystemVariable->writeWord(iw_Video_Palette9, 0x000F);				// SysVar40
	b_SystemVariable->writeWord(iw_Video_Palette10, 0x00F0);			// SysVar41
	b_SystemVariable->writeWord(iw_Video_Palette11, 0x00FF);			// SysVar42
	b_SystemVariable->writeWord(iw_Video_Palette12, 0x0F00);			// SysVar43
	b_SystemVariable->writeWord(iw_Video_Palette13, 0x0F0F);			// SysVar44
	b_SystemVariable->writeWord(iw_Video_Palette14, 0x0FF0);			// SysVar45
	b_SystemVariable->writeWord(iw_Video_Palette15, 0x0FFF);			// SysVar46
	b_SystemVariable->writeWord(iw_Video_Color0, 0x0888);				// SysVar47
	b_SystemVariable->writeWord(iw_Video_Color1, 0x0A3F);				// SysVar48
	b_SystemVariable->writeWord(iw_Video_Color2, 0x06F5);				// SysVar49
	b_SystemVariable->writeWord(iw_Video_Color3, 0x00FB);				// SysVar50
	b_SystemVariable->writeWord(iw_Video_Color4, 0x0F30);				// SysVar51
	b_SystemVariable->writeWord(iw_Video_Color5, 0x0F0F);				// SysVar52
	b_SystemVariable->writeWord(iw_Video_Color6, 0x0DF0);				// SysVar53
	b_SystemVariable->writeWord(iw_Video_Color7, 0x0FFF);				// SysVar54
	b_SystemVariable->writeWord(idp_Video_Screen_Plane0, 0xA800);			// SysVar61
	b_SystemVariable->writeWord(idp_Video_Screen_Plane1, 0xB000);			// SysVar63
	b_SystemVariable->writeWord(idp_Video_Screen_Plane2, 0xB800);			// SysVar65
	b_SystemVariable->writeWord(idp_Video_Screen_Plane3, 0xE000);			// SysVar67
	b_SystemVariable->writeWord(idp_Video_Buffer1_Plane0, 0xA800);			// SysVar69
	b_SystemVariable->writeWord(idp_Video_Buffer1_Plane1, 0xB000);			// SysVar71
	b_SystemVariable->writeWord(idp_Video_Buffer1_Plane2, 0xB800);			// SysVar73
	b_SystemVariable->writeWord(idp_Video_Buffer1_Plane3, 0xE000);			// SysVar75
	b_SystemVariable->writeWord(idp_Video_Buffer2_Plane0, 0xA800);			// SysVar77
	b_SystemVariable->writeWord(idp_Video_Buffer2_Plane1, 0xB000);			// SysVar79
	b_SystemVariable->writeWord(idp_Video_Buffer2_Plane2, 0xB800);			// SysVar81
	b_SystemVariable->writeWord(idp_Video_Buffer2_Plane3, 0xE000);			// SysVar83
	b_SystemVariable->writeWord(idp_Video_Buffer3_Plane0, 0xA800);			// SysVar85
	b_SystemVariable->writeWord(idp_Video_Buffer3_Plane1, 0xB000);			// SysVar87
	b_SystemVariable->writeWord(idp_Video_Buffer3_Plane2, 0xB800);			// SysVar89
	b_SystemVariable->writeWord(idp_Video_Buffer3_Plane3, 0xE000);			// SysVar91
	b_SystemVariable->writeWord(ib_DATFile_Slot0, 0x01);				// SysVar93l
	b_SystemVariable->writeWord(ib_DATFile_Slot1, 0x00);				// SysVar93u

	b_AnimationSlot->writeWord(ia_Slot0 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot1 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot2 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot3 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot4 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot5 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot6 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot7 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot8 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeWord(ia_Slot9 + iwpo_Entry, animation_slot_entry + ia_Terminator);
	b_AnimationSlot->writeByte(ia_Terminator, 0xFF);

	return true;
}


MemorySegment* Memory::getCurrentSegment()
{
	MemorySegment *segment;

	word selector = b_SystemVariable->queryWord(iw_Segment);
	switch (selector) {
		case 0:
			segment = s_Core;
			break;
		case 1:
			segment = s_FieldData;
			break;
		case 2:
			segment = s_FieldMap;
			break;
		case 3:
		default:
			segment = s_Resources;
			break;
	}
	/*
	switch (selector) {
		case 0:
			segment = s_Core;
			break;
		case 1:
			segment = s_FieldData;
			break;
		case 2:
		default:
			segment = s_Resources;
			break;
	}
	*/

	return segment;
}


void Memory::loadFlag(MemoryBlock *b_Flag)
{
	b_ScriptName->writeBlock(b_Flag, script_name_entry - flag_entry);
	b_Variable->writeBlock(b_Flag, variable_entry - flag_entry);
	b_BaseVariable->writeBlock(b_Flag, base_variable_entry - flag_entry);
	b_SystemVariable->writeBlock(b_Flag, system_variable_entry - flag_entry);
	b_Heap->writeBlock(b_Flag, heap_entry - flag_entry);
}


byte Memory::loadVariable(word index)
{
	byte variable = b_Variable->queryByte(index >> 1);
	if ((index & 1) == 1) {
		return ((variable & MASK_UPPER_BYTE) >> 4);
	}
	else {
		return (variable & MASK_LOWER_BYTE);
	}
}


void Memory::saveVariable(word index, word data)
{
	if (data > 15) {
		data = 15;
	}

	byte variable = b_Variable->queryByte(index >> 1);
	if ((index & 1) == 1) {
		variable = (variable & MASK_LOWER_BYTE) | (data << 4);
		b_Variable->writeByte(index >> 1, variable);
	}
	else {
		variable = (variable & MASK_UPPER_BYTE) | data;
		b_Variable->writeByte(index >> 1, variable);
	}
}
