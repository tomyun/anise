#include "script.h"

Script::Script(Memory *memory, Timer *timer, Input *input, Sound *sound, Video *video, Option *option)
{
	this->memory = memory;
	this->timer = timer;
	this->input = input;
	this->sound = sound;
	this->video = video;
	this->option = option;
	
	file = new File(memory, option);
	image = new Image(memory, video, file);
	animation = new Animation(memory, timer, input, video);
	dialogue = new Dialogue(memory, timer, input, video, animation, option);
	field = new Field(memory, input, video, option);

	parameter = NULL;

	parse_level = 0;

	random_seed = RANDOM_SEED;

	setOpcodes();
}


Script::~Script()
{	
	delete field;
	delete dialogue;
	delete animation;
	delete image;
	delete file;
}


byte Script::fetch()
{
	return memory->b_Script->queryByte();
}


byte Script::fetchAdvance()
{
	return memory->b_Script->readByte();
}


void Script::advance()
{
	memory->b_Script->advance();
}


word Script::skipScriptBlock()
{
	word number_of_blocks;
	byte code = fetchAdvance();

	if (code == CODE_BLOCK_START) {
		number_of_blocks = 1;
		while (number_of_blocks > 0) {
			code = fetchAdvance();
			if (code == CODE_BLOCK_START) {
				number_of_blocks++;
			}
			else if (code == CODE_BLOCK_END) {
				number_of_blocks--;
			}
			else {
				continue;
			}
		}

		return ((number_of_blocks << 8) | code);
	}
	else {
		return skipScriptBlock();
	}
}


word Script::getOffset()
{
	return memory->b_Script->getCurrentIndex();
}


void Script::setOffset(word offset)
{
	memory->b_Script->set(offset);
}


void Script::setOpcodes()
{
	static Opcode opcode_default_op_list[OPCODE_OP] = {
		// 00
		OPCODE(op_saveConstant),
		OPCODE(op_saveExpression),
		OPCODE(op_setBaseVariable),
		OPCODE(op_makeArray),
		// 04
		OPCODE(op_makeByteArray),
		OPCODE(op_if),
		OPCODE(op_setDialogueColor),
		OPCODE(op_wait),
		// 08
		OPCODE(op_defineProcedure),
		OPCODE(op_callProcedure),
		OPCODE(op_call),
		OPCODE(op_displayNumber),
		// 12
		OPCODE(op_delay),
		OPCODE(op_clearScreen),
		OPCODE(op_setColor),
		OPCODE(op_utility),
		// 16
		OPCODE(op_animate)
	};

	static Opcode opcode_default_op4_list[OPCODE_OP4] = {
		// 00
		OPCODE(op4_while),
		OPCODE(op4_continue),
		OPCODE(op4_break),
		OPCODE(op4_displaySelection),
		// 04
		OPCODE(op4_initializeSelection),
		OPCODE(op4_mouse),
		OPCODE(op4_palette),
		OPCODE(op4_drawSolidBox),
		// 08
		OPCODE(op4_drawInverseBox),
		OPCODE(op4_blitDirect),
		OPCODE(op4_blitSwapped),
		OPCODE(op4_blitMasked),
		// 12
		OPCODE(op4_loadFile),
		OPCODE(op4_loadImage),
		OPCODE(op4_jumpScript),
		OPCODE(op4_callScript),
		// 16
		OPCODE(op4_dummy),
		OPCODE(op4_manipulateFlag),
		OPCODE(op4_dummy),
		OPCODE(op4_checkClick),
		// 20
		OPCODE(op4_sound),
		OPCODE(op4_dummy),
		OPCODE(op4_field),
		OPCODE(op4_dummy)
	};

	static Opcode opcode_default_opre_list[OPCODE_OPRE] = {
		// 00
		OPCODE(opre_add),
		OPCODE(opre_subtract),
		OPCODE(opre_multiply),
		OPCODE(opre_divide),
		// 04
		OPCODE(opre_modular),
		OPCODE(opre_or),
		OPCODE(opre_and),
		OPCODE(opre_isEqual),
		// 08
		OPCODE(opre_isNotEqual),
		OPCODE(opre_isAbove),
		OPCODE(opre_isBelow),
		OPCODE(opre_getWordFromMemory),
		// 12
		OPCODE(opre_getByteFromMemory),
		OPCODE(opre_getConstantFromOperand),
		OPCODE(opre_getConstantFromStack),
		OPCODE(opre_random)
	};

	static Opcode opcode_nanpa2_oput_list[OPCODE_OPUT] = {
		// 00
		OPCODE(oput_unpackAniHeader),
		OPCODE(oput_dummy),		//TODO: getMainArg2
		OPCODE(oput_overlapScreen),
		OPCODE(oput_swapScreen),
		// 04
		OPCODE(oput_sprayScreen),
		OPCODE(oput_dummy),		//TODO: getParam1
		OPCODE(oput_dummy),		//TODO: exitWithMessage0 (call scroll_2)
		OPCODE(oput_dummy),		//TODO: exitWithMessage0 (call deja2_scroll)
		// 08
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		// 12
		OPCODE(oput_dummy),
		OPCODE(oput_dummy)
	};

	static Opcode opcode_nanpa1_oput_list[OPCODE_OPUT] = {
		// 00
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		// 04
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		// 08
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		// 12
		OPCODE(oput_dummy),
		OPCODE(oput_dummy)
	};

	static Opcode opcode_aisimai_oput_list[OPCODE_OPUT] = {
		// 00
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),		//TODO: getMainArg2
		OPCODE(oput_dummy),
		OPCODE(oput_sprayScreen),
		// 04
		OPCODE(oput_overlapScreen),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),		//TODO: waitForVSync
		OPCODE(oput_dummy),
		// 08
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		OPCODE(oput_dummy),
		// 12
		OPCODE(oput_dummy),
		OPCODE(oput_dummy)
	};

	opcode_op_list = opcode_default_op_list;
	opcode_op4_list = opcode_default_op4_list;
	opcode_opre_list = opcode_default_opre_list;

	if (option->game_type == GAME_NANPA2) {
		//TODO: make it work
		//opcode_op_list[6] = OPCODE(op_null);

		opcode_oput_list = opcode_nanpa2_oput_list;
	}
	else if (option->game_type == GAME_NANPA1) {
		opcode_oput_list = opcode_nanpa1_oput_list;
	}
	else if (option->game_type == GAME_AISIMAI) {
		opcode_oput_list = opcode_aisimai_oput_list;
	}
	else if (option->game_type == GAME_CRESCENT) {
		//HACK: same as aisimai
		opcode_oput_list = opcode_aisimai_oput_list;
	}
}


SCRIPTCALL Script::excuteOpcodeOp(byte code)
{
	PRINT("[Script::excuteOpcodeOp()] run %s, offset = %x\n", getOpcodeDescriptionOp(code), getOffset());

	OpcodeFunction operation = opcode_op_list[code].function;
	SCRIPTCALL condition = (this->*operation) ();

	return condition;
}


SCRIPTCALL Script::excuteOpcodeOp4(byte code)
{
	PRINT("[Script::excuteOpcodeOp4()] run %s, offset = %x\n", getOpcodeDescriptionOp4(code), getOffset());

	OpcodeFunction operation = opcode_op4_list[code].function;
	SCRIPTCALL condition = (this->*operation) ();

	return condition;
}


SCRIPTCALL Script::excuteOpcodeOpre(byte code)
{
	OpcodeFunction operation = opcode_opre_list[code].function;
	SCRIPTCALL condition = (this->*operation) ();

	return condition;
}


const char* Script::getOpcodeDescriptionOp(byte code)
{
	return opcode_op_list[code].description;
}


const char* Script::getOpcodeDescriptionOp4(byte code)
{
	return opcode_op4_list[code].description;
}


const char* Script::getOpcodeDescriptionOpre(byte code)
{
	return opcode_opre_list[code].description;
}


ScriptParameter* Script::getParameter()
{
	//assert(parameter == NULL);
	deleteParameter();

	ScriptParameter *parameter = new ScriptParameter(memory);

	byte code;
	while (true) {
		code = fetch();
		if (code == CODE_PARAMETER_STRING) {
			parameter->add(PARAMETER_TYPE_STRING, getOffset());

			advance();
			while (fetchAdvance() != CODE_PARAMETER_STRING) {
				;
			}
		}
		else if (code == CODE_PARAMETER_BLOCK) {
			parameter->add(PARAMETER_TYPE_BLOCK, getOffset());

			skipScriptBlock();
		}
		else {
			if ((code == CODE_CONSTANT_1OP) || (code == CODE_CONSTANT_2OP) || (code == CODE_CONSTANT_3OP) || ((code >= CODE_CONSTANT_RE_FIRST) && (code <= CODE_BASEVARIABLE_LAST))) {
				parameter->add(PARAMETER_TYPE_CONSTANT, readExpression());
			}
			else {
				break;
			}
		}

		code = fetch();
		if (code == CODE_CONTINUE) {
			advance();
			continue;
		}
		else {
			break;
		}
	}

	return parameter;
}


void Script::deleteParameter()
{
	if (parameter != NULL) {
		delete parameter;
		parameter = NULL;
	}
}


void Script::load(const char *filename)
{
	word script_offset = memory->b_SystemVariable->queryWord(iwpo_Script);
	MemoryBlock *b_Script = memory->s_Core->get(&script_offset);

	//TODO: need this?
	//b_Script->clear();

	file->open(filename);
	file->load(b_Script, script_offset);
	file->close();

	PRINT("[Script::load] set offset to %x\n", b_Script->queryWord(script_offset));
	setOffset(script_offset + b_Script->queryWord(script_offset));
}


SCRIPTCALL Script::parse()
{
	parse_level++;
	PRINT("[Script::parse()] parse_level = %d -> %d\n", (parse_level - 1), parse_level);

	SCRIPTCALL condition = RETURN_NORMAL;
	while (condition == RETURN_NORMAL) {
		//HACK: take screenshot
		if (input->isCapture()) {
			video->capture();
		}

		//TODO: improve this
		//TODO: debugmode
		animation->show();
		if (input->refresh() == false) {
			condition = RETURN_EXIT;
			break;
		}

		for (int i = 0; i < FRAME_RATE; i++) {
			animation->show();
		}

		video->overlapScreen();

		word reload = memory->b_SystemVariable->queryWord(iwf_Parser_Reload);
		if (reload > 0) {
			PRINT("[Script::parse()] reload\n");

			reload--;
			if (reload > 0) {
				reload = 0;

				word script_offset = memory->b_SystemVariable->queryWord(iwpo_Script);
				MemoryBlock *b_Script = memory->s_Core->get(&script_offset);

				file->open(memory->b_ScriptName->queryString(memory->script_name_entry, SCRIPT_NAME_SIZE));
				file->load(b_Script, script_offset);
				file->close();
			}

			memory->b_SystemVariable->writeWord(iwf_Parser_Reload, reload);
		}

		//TODO: check input disabled status

		byte code = fetch();
		//PRINT("[Script::parse()] code = %2x, offset = %x\n", code, getOffset());
		if (code == 0x00 || code == 0x02) {
			advance();
			condition = RETURN_SUCCESS;
		}
		else if (code == 0x01) {
			word offset = getOffset();
			condition = parseNested();
			setOffset(offset);
			skipScriptBlock();
		}
		else if (code == 0x04) {
			advance();
			code = fetchAdvance();

			code = code - 0x10;
			condition = excuteOpcodeOp4(code);
		}
		else if (code == 0x06) {
			dialogue->putHalfWidthCharacters();
		}
		else if (code > 0x09 && code < 0x20) {
			advance();

			code = code - 0x0A;
			condition = excuteOpcodeOp(code);
		}
		else if (code >= 0x60 && code <= 0x7F) {
			dialogue->putStandardText();
		}
		else if (code >= 0x80) {
			dialogue->putPredefinedText();
		}
		else {
			PRINT_ERROR("[Script::parse()] invalid opcode = %2x, offset = %x\n", code, getOffset());
			condition = RETURN_ERROR;
			break;
		}
	}

	if (condition != RETURN_ERROR) {
		parse_level--;
		PRINT("[Script::parse()] parse_level = %d -> %d, return condition = %x, offset = %x\n", (parse_level + 1), parse_level, condition, getOffset());

		return condition;
	}
	else {
		parse_level--;
		PRINT_ERROR("[Script::parse()] exit with error: parse_level = %d, offset = %x\n", parse_level, getOffset());
		return RETURN_ERROR;
	}
}


SCRIPTCALL Script::parseNested()
{
	advance();
	return parse();
}
