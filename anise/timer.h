#ifndef TIMER_H
#define TIMER_H

#include "anise.h"
#include "SDL_timer.h"

#define TIMER_INTERVAL	10

class Timer {
private:
	SDL_TimerID timer_id;

	word cursor_timer;
	dword frame_timer;
	dword delay_timer;
	word overlap_timer;
	word overlap_frame_timer;
	word spray_timer;

	static Uint32 callback(Uint32 interval, void *pointer);
	void accumulate();

public:
	Timer();
	~Timer();

	void resetCursorTimer();
	void resetFrameTimer();
	void resetDelayTimer();
	void resetOverlapTimer();
	void resetOverlapFrameTimer();
	void resetSprayTimer();

	word checkCursorTimer();
	dword checkFrameTimer();
	dword checkDelayTimer();
	word checkOverlapTimer();
	word checkOverlapFrameTimer();
	word checkSprayTimer();
};

#endif
