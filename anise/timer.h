#ifndef TIMER_H
#define TIMER_H

#include "anise.h"
#include "SDL_timer.h"

#define TIMER_INTERVAL	10

class Timer {
private:
	Uint32 cursor_ticks;
	Uint32 frame_ticks;
	Uint32 delay_ticks;
	Uint32 overlap_ticks;
	Uint32 overlap_frame_ticks;
	Uint32 spray_ticks;

public:
	Timer();
	~Timer();

	void delay(Uint32 ticks = TIMER_INTERVAL);

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
