#include "timer.h"

Timer::Timer()
{
	cursor_ticks = 0;
	frame_ticks = 0;
	delay_ticks = 0;
	overlap_ticks = 0;
	overlap_frame_ticks = 0;
	spray_ticks = 0;
}


Timer::~Timer()
{
}


void Timer::delay(Uint32 ticks)
{
	SDL_Delay(ticks);
}


void Timer::resetCursorTimer()
{
	cursor_ticks = SDL_GetTicks();
}


void Timer::resetFrameTimer()
{
	frame_ticks = SDL_GetTicks();
}


void Timer::resetDelayTimer()
{
	delay_ticks = SDL_GetTicks();
}


void Timer::resetOverlapTimer()
{
	overlap_ticks = SDL_GetTicks();
}


void Timer::resetOverlapFrameTimer()
{
	overlap_frame_ticks = SDL_GetTicks();
}


void Timer::resetSprayTimer()
{
	spray_ticks = SDL_GetTicks();
}


word Timer::checkCursorTimer()
{
	return (word) ((SDL_GetTicks() - cursor_ticks) / TIMER_INTERVAL);
}


dword Timer::checkFrameTimer()
{
	return (dword) ((SDL_GetTicks() - frame_ticks) / TIMER_INTERVAL);
}


dword Timer::checkDelayTimer()
{
	return (dword) ((SDL_GetTicks() - delay_ticks) / TIMER_INTERVAL);
}


word Timer::checkOverlapTimer()
{
	return (word) ((SDL_GetTicks() - overlap_ticks) / TIMER_INTERVAL);
}


word Timer::checkOverlapFrameTimer()
{
	return (word) ((SDL_GetTicks() - overlap_frame_ticks) / TIMER_INTERVAL);
}


word Timer::checkSprayTimer()
{
	return (word) ((SDL_GetTicks() - spray_ticks) / TIMER_INTERVAL);
}
