#include "engine.h"

Engine::Engine(Option *option)
{
	this->option = option;
}


Engine::~Engine()
{
}


bool Engine::on()
{
	//TODO: move this method into constructor?

	//TODO: initialize font

	//TODO: initialize video, mouse and keyboard (SDL related)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0) {
		PRINT("[Engine::initialize()] unable to initialize SDL backend: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption(TITLE, NULL);

	memory = new Memory(option);
	video = new Video(memory, option);
	sound = new Sound(option);
	timer = new Timer(video);
	input = new Input(memory, timer);

	script = new Script(memory, video, sound, timer, input, option);

	return true;
}


bool Engine::off()
{
	delete script;
	delete input;
	delete video;
	delete timer;
	delete memory;

	SDL_Quit();

	return true;
}


bool Engine::run()
{
	script->load(option->script_file_name);
	SCRIPTCALL condition = script->parse();
	
	PAUSE;

	return true;
}