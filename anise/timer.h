#ifndef TIMER_H
#define TIMER_H

#include "memory.h"
#include "video.h"
#include "SDL_timer.h"

#define TIMER_INTERVAL	10

class Timer {
private:
	Video *video;

	SDL_TimerID timer_id;

	word cursor_timer;
	dword frame_timer;
	dword delay_timer;
	word overlap_timer;
	word spray_timer;

	static Uint32 callback(Uint32 interval, void *pointer);
	void accumulate();
	void display();

public:
	Timer(Video *video);
	~Timer();

	void resetCursorTimer();
	void resetFrameTimer();
	void resetDelayTimer();
	void resetOverlapTimer();
	void resetSprayTimer();

	word checkCursorTimer();
	dword checkFrameTimer();
	dword checkDelayTimer();
	word checkOverlapTimer();
	word checkSprayTimer();
};

#endif