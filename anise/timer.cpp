#include "timer.h"

Timer::Timer()
{
	cursor_timer = 0;
	frame_timer = 0;
	delay_timer = 0;
	overlap_timer = 0;
	overlap_frame_timer = 0;
	spray_timer = 0;

	timer_id = SDL_AddTimer(TIMER_INTERVAL, (SDL_NewTimerCallback) callback, this);
}


Timer::~Timer()
{
	SDL_RemoveTimer(timer_id);
}


Uint32 Timer::callback(Uint32 interval, void *pointer)
{
	((Timer*)pointer)->accumulate();

	return interval;
}


void Timer::accumulate()
{
	cursor_timer++;
	frame_timer++;
	delay_timer++;
	overlap_timer++;
	overlap_frame_timer++;
	spray_timer++;
}


void Timer::resetCursorTimer()
{
	cursor_timer = 0;
}


void Timer::resetFrameTimer()
{
	frame_timer = 0;
}


void Timer::resetDelayTimer()
{
	delay_timer = 0;
}


void Timer::resetOverlapTimer()
{
	overlap_timer = 0;
}


void Timer::resetOverlapFrameTimer()
{
	overlap_frame_timer = 0;
}


void Timer::resetSprayTimer()
{
	spray_timer = 0;
}


word Timer::checkCursorTimer()
{
	return cursor_timer;
}


dword Timer::checkFrameTimer()
{
	return frame_timer;
}


dword Timer::checkDelayTimer()
{
	return delay_timer;
}


word Timer::checkOverlapTimer()
{
	return overlap_timer;
}


word Timer::checkOverlapFrameTimer()
{
	return overlap_frame_timer;
}


word Timer::checkSprayTimer()
{
	return spray_timer;
}
