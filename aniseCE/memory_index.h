#ifndef MEMORY_INDEX_H
#define MEMORY_INDEX_H

enum BaseVariableIndex {
	// 00
	iw_SystemVariable = 0,
	iw_Array = 2,
	iw_ByteArray = 4,
	iw_C = 6,
	// 04
	iw_Day = 8,
	iw_E = 10,
	iw_F = 12,
	iw_G = 14,
	// 08
	iw_H__Date = 16,
	iw_Item = 18,
	iw_J = 20,
	iw_K = 22,
	// 12
	iw_L = 24,
	iw_Money = 26,
	iw_N = 28,
	iw_O = 30,
	// 16
	iw_P = 32,
	iw_Q = 34,
	iw_R = 36,
	iw_Selection = 38,
	// 20
	iw_Time = 40,
	iw_U = 42,
	iw_V = 44,
	iw_W = 46,
	// 24
	iw_X = 48,
	iw_Y = 50,
	iw_Z = 52
};


enum SystemVariableIndex {
	// 00
	iwpo_Heap = 0,
	iwpo_GP4File = 2,
	iwpo_AnimationScript = 4,
	iwpo_MFile = 6,
	// 04
	iwpo_SysVar4 = 8,
	iwpo_FieldHeader = 10,
	iw_DisplayBuffer = 12,
	iw_Segment = 14,
	// 08
	iw_Dialogue_Delay = 16,
	ibf_DisabledStatus = 18,
	ibf_Selection_Status = 19,
	iw_Mouse_CoordX = 20,
	iw_Mouse_CoordY = 22,
	// 12
	iwf_Parser_Reload = 24,
	iw_Video_CoordXb = 26,
	iw_Video_CoordY = 28,
	iw_Video_MaxCoordXb = 30,
	// 16
	iw_Video_MaxCoordY = 32,
	iw_Dialogue_CoordXb = 34,
	iw_Dialogue_CoordY = 36,
	iw_Video_Color = 38,
	// 20
	ib_SysVar20l = 40,
	ibf_OP_DisplayNumber = 41,		//TODO: change name (OP_prnum)
	ib_Dialogue_FontHeight = 42,
	ib_Dialogue_FontWidthb = 43,
	iwpo_Selection_Item = 44,		//TODO: may change name (SELECT_)
	iw_SysVar23 = 46,
	// 24
	iw_Selection_SkipItemCount = 48,	//TODO: may change name (SELECT_)
	iw_Selection_TotalItemCount = 50,	//TODO: may change name (SELECT_)
	iw_Selection_CurrentItemCount = 52,	//TODO: may change name (SELECT_)
	iwf_Selection_InUse = 54,		//TODO: may change name (SELECT_)
	// 28
	iw_Video_CoordFactor = 56,		//UNKNOWN
	iw_SysVar29 = 58,
	iw_SysVar30 = 60,
	iw_Video_Palette0 = 62,
	// 32
	iw_Video_Palette1 = 64,
	iw_Video_Palette2 = 66,
	iw_Video_Palette3 = 68,
	iw_Video_Palette4 = 70,
	// 36
	iw_Video_Palette5 = 72,
	iw_Video_Palette6 = 74,
	iw_Video_Palette7 = 76,
	iw_Video_Palette8 = 78,
	// 40
	iw_Video_Palette9 = 80,
	iw_Video_Palette10 = 82,
	iw_Video_Palette11 = 84,
	iw_Video_Palette12 = 86,
	// 44
	iw_Video_Palette13 = 88,
	iw_Video_Palette14 = 90,
	iw_Video_Palette15 = 92,
	iw_Video_Color0 = 94,
	// 48
	iw_Video_Color1 = 96,
	iw_Video_Color2 = 98,
	iw_Video_Color3 = 100,
	iw_Video_Color4 = 102,
	// 52
	iw_Video_Color5 = 104,
	iw_Video_Color6 = 106,
	iw_Video_Color7 = 108,
	idp_Core = 110,
	// 57
	idp_MapFile = 114,
	idp_MapProcess = 118,
	// 61
	idp_Video_Screen_Plane0 = 122,
	idp_Video_Screen_Plane1 = 126,
	idp_Video_Screen_Plane2 = 130,
	idp_Video_Screen_Plane3 = 134,
	// 69
	idp_Video_Buffer1_Plane0 = 138,
	idp_Video_Buffer1_Plane1 = 142,
	idp_Video_Buffer1_Plane2 = 146,
	idp_Video_Buffer1_Plane3 = 150,
	// 77
	idp_Video_Buffer2_Plane0 = 154,
	idp_Video_Buffer2_Plane1 = 158,
	idp_Video_Buffer2_Plane2 = 162,
	idp_Video_Buffer2_Plane3 = 166,
	// 85
	idp_Video_Buffer3_Plane0 = 170,
	idp_Video_Buffer3_Plane1 = 174,
	idp_Video_Buffer3_Plane2 = 178,
	idp_Video_Buffer3_Plane3 = 182,
	// 93
	ib_PackedFile_Slot0 = 186,
	ib_PackedFile_Slot1 = 187,
	iw_GP4File_CoordXb = 188,
	iw_GP4File_CoordY = 190,
	// 96
	iw_GP4File_Widthb = 192,
	iw_GP4File_Height = 194,
	iw_File_Size = 196,			//TODO: may change name (LoadFile_)
	iwpo_Script = 198			//TODO: may change name (LoadFile_)
};


enum ProcIndex {
	// 00
	iwpo_Proc0 = 0,
	iwpo_Proc1 = 2,
	iwpo_Proc2 = 4,
	iwpo_Proc3 = 6,
	// 04
	iwpo_Proc4 = 8,
	iwpo_Proc5 = 10,
	iwpo_Proc6 = 12,
	iwpo_Proc7 = 14,
	// 08
	iwpo_Proc8 = 16,
	iwpo_Proc9 = 18,
	iwpo_Proc10 = 20,
	iwpo_Proc11 = 22,
	// 12
	iwpo_Proc12 = 24,
	iwpo_Proc13 = 26,
	iwpo_Proc14 = 28,
	iwpo_Proc15 = 30,
	// 16
	iwpo_Proc16 = 32,
	iwpo_Proc17 = 34,
	iwpo_Proc18 = 36,
	iwpo_Proc19 = 38,
	// 20
	iwpo_Proc20 = 40,
	iwpo_Proc21 = 42,
	iwpo_Proc22 = 44,
	iwpo_Proc23 = 46,
	// 24
	iwpo_Proc24 = 48,
	iwpo_Proc25 = 50,
	iwpo_Proc26 = 52,
	iwpo_Proc27 = 54,
	// 28
	iwpo_Proc28 = 56,
	iwpo_Proc29 = 58,
	iwpo_Proc30 = 60,
	iwpo_Proc31 = 62,
	// 32
	iwpo_Selection_OK = 64,
	iwpo_Selection_Cancel = 66,
	iwpo_Selection_Up = 68,
	iwpo_Selection_Down = 70,
	// 36
	iwpo_Selection_Left = 72,
	iwpo_Selection_Right = 74,
	iwpo_Selection_ShowMenu = 76,
	iwpo_Selection_CheckPosition = 78,
	// 40
	iwpo_Selection_List = 80,
	iwpo_Proc41 = 82,
	iwpo_Proc42 = 84,
	iwpo_Proc43 = 86,
	// 44
	iwpo_Proc44 = 88,
	iwpo_Proc45 = 90,
	iwpo_Proc46 = 92,
	iwpo_Proc47 = 94,
	// 48
	iwpo_Proc48 = 96,
	iwpo_Proc49 = 98,
	iwpo_Proc50 = 100,
	iwpo_Proc51 = 102,
	// 52
	iwpo_Proc52 = 104,
	iwpo_Proc53 = 106,
	iwpo_Proc54 = 108,
	iwpo_Proc55 = 110,
	// 56
	iwpo_Proc56 = 112,
	iwpo_Proc57 = 114,
	iwpo_Proc58 = 116,
	iwpo_Proc59 = 118,
	// 60
	iwpo_Proc60 = 120
};


enum AnimationSlotIndex {
	// 00
	ia_Slot0 = 0x00,
	ia_Slot1 = 0x10,
	ia_Slot2 = 0x20,
	ia_Slot3 = 0x30,
	// 04
	ia_Slot4 = 0x40,
	ia_Slot5 = 0x50,
	ia_Slot6 = 0x60,
	ia_Slot7 = 0x70,
	// 08
	ia_Slot8 = 0x80,
	ia_Slot9 = 0x90,
	ia_Terminator = 0xA0
};


enum AnimationSlotStructure {
	// 00
	ib_Status = 0,
	ib_Delay = 1,
	iwpo_Entry = 2,
	iwpo_Current = 4,
	// 04
	iwpo_6 = 6,
	iwpo_8 = 8,
	ib_10 = 10,
	ib_11 = 11,
	// 08
	iwpo_Buffer = 12,
	iwpo_VRAM = 14
};


enum FieldHeaderIndex {
	iwpo_MAPFile = 0,
	iw_View_LeftLimit = 2,
	iw_View_UpLimit = 4,
	iw_View_RightLimit = 6,
	iw_View_DownLimit = 8,
	iw_View_MarginXw = 10,
	iw_View_MarginY = 12,
	iw_View_Widthw = 14,
	iw_View_Heightw = 16,
	iw_Character_LeftLimit = 18,
	iw_Character_UpLimit = 20,
	iw_Character_RightLimit = 22,
	iw_Character_DownLimit = 24,
	iw_View_CoordXw = 26,
	iw_View_CoordYw = 28,
	iw_Map_Widthw = 30,
	iw_Map_Heightw = 32
};


enum DisabledStatus {
	DISABLE_PRESERVEIMAGEPALETTE = 0x01,	// 0000_0001b
	DISABLE_ANIMATION = 0x02,		// 0000_0010b
	DISABLE_4 = 0x04,			// 0000_0100b
	DISABLE_PREDEFINEDTEXT = 0x08,		// 0000_1000b
	DISABLE_PRESERVEPALETTE = 0x10,		// 0001_0000b
	DISABLE_FLAG_RESET = 0x60,		// 0110_0000b
	DISABLE_FLAG_SET = 0x9F			// 1001_1111b
};

#endif
