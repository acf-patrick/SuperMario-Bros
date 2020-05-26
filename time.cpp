#include "time.hpp"
#include "const.hpp"
#include <sstream>

Clock::Clock(): start_time(SDL_GetTicks())
{}
void Clock::restart()
{
    start_time = SDL_GetTicks();
}
int Clock::as_milliseconds()
{
    return SDL_GetTicks() - start_time;
}
float Clock::as_second()
{
    return as_milliseconds()/1000.0;
}
bool Clock::elapsed_in(int a, int b)
{
    if (a>b)
        return elapsed_in(b, a);
    return (a <= as_milliseconds() and as_milliseconds() < b);
}

Timer::Timer():
    Text(WIDTH-100, 10, "TIME", NULL, "emulogic.ttf"),
    in(false), _countdown(false),
    remaining_time(400),
    time_surf(NULL), color(WHITE)
{}

Timer::~Timer()
{
    SDL_FreeSurface(surface);
    SDL_FreeSurface(time_surf);
    TTF_CloseFont(font);
    font = NULL;
}

bool Timer::over()
{
    if (_countdown)
        return false;
    return remaining_time <= 0;
}

bool Timer::out_of_time()
{
    if (_countdown)
        return false;
    if (remaining_time <= 100 and !in)
    {
        in = true;
        return true;
    }
    return false;
}

void Timer::restart()
{
    remaining_time = 400;
    color = WHITE;
    in = false;
    time_surf = NULL;
}

bool Timer::countdown()
{
    _countdown = true;
    return (remaining_time > 0);
}

void Timer::update()
{
    std::stringstream str;
    if (timer.as_milliseconds() >= (_countdown?1:500))
    {
        timer.restart();
        remaining_time --;
        if (remaining_time < 0)
            remaining_time = 0;
        str << remaining_time;
        if (_countdown)
            color = WHITE;
        else
            if (remaining_time <= 110)
                color = (color.g or color.b)?RED:WHITE;
        SDL_FreeSurface(time_surf);
        time_surf = TTF_RenderText_Solid(font, str.str().c_str(), color);
    }
}

void Timer::show(SDL_Surface* screen)
{
    SDL_Rect pos = {(Sint16)x, (Sint16)y};
    SDL_BlitSurface(surface, NULL, screen, &pos);
    if (time_surf)
    {
        pos.x = (Sint16)((pos.x+surface->w*.5)-time_surf->w*.5);
        pos.y += (surface->h + 2);
        SDL_BlitSurface(time_surf, NULL, screen, &pos);
    }
}

FPS::FPS(int _x, int _y, const std::string& content):
    Text(_x, _y, content, NULL, "emulogic.ttf", 10), avg_fps(0),
    frame(0)
{}

void FPS::start()
{
    fps.restart();
    timer.restart();
}

void FPS::update()
{
    frame ++;
    if (timer.as_second() >= 1)
    {
        std::stringstream caption;
        avg_fps = frame/fps.as_second();
        caption << "fps : " << avg_fps;
        set_text(caption.str());
        timer.restart();
    }
}
