#include "engine.h"

Engine::Engine(Config *config)
{
	this->config = config;
}


Engine::~Engine()
{
}


bool Engine::on()
{
	//TODO: move this method into constructor?

	//TODO: initialize font

	//TODO: initialize video, mouse and keyboard (SDL related)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		PRINT("[Engine::initialize()] unable to initialize SDL backend: %s\n", SDL_GetError());
		exit(1);
	}

	SDL_WM_SetCaption(config->title, NULL);

	memory = new Memory(config);
	video = new Video(memory, config);
	timer = new Timer(video);
	input = new Input(memory, timer);

	script = new Script(memory, video, timer, input, config);

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
	script->load(config->script_file_name);
	SCRIPTCALL condition = script->parse();
	
	PAUSE;

	return true;
}