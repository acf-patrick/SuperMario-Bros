#ifndef TIME_HPP
#define TIME_HPP
#include <SDL.h>
#include "text.hpp"

class Clock
{
public:
    Clock();
    void restart();
    bool elapsed_in(int a, int b);
    float as_second();
    int as_milliseconds();
private:
    int start_time;
};

class Timer: public Text
{
public:
    int* score;

    Timer();
    ~Timer();

    bool out_of_time();
    bool over();
    bool countdown();
    void update();
    void restart();
    void show(SDL_Surface* screen);
private:
    bool in;
    bool _countdown;
    Clock timer;
    int remaining_time;
    SDL_Surface* time_surf;
    SDL_Color color;
};

class FPS: public Text
{
public:
    FPS(int _x, int _y, const std::string& content);
    void update();
    void start();
    float avg_fps;
private:
    Clock fps, timer;
    int frame;
};
#endif // TIME_HPP
