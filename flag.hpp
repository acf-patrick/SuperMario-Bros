#ifndef FLAG_HPP
#define FLAG_HPP
#include <SDL.h>
#include "map.hpp"

class Map;

class Flag
{
public:
    Flag(Map* m);
    ~Flag();
    void slide_down();
    void show(SDL_Surface* screen);
    void update();
    bool is_down();
    SDL_Rect get_rect();
    bool* player_flag_boolean;
private:
    Map* _map;
    SDL_Surface *spritesheet, *flag;
    int x, y;
    Uint16 width, height;
    float y_vel;
    bool sliding_down, yet;
};

#endif // FLAG_HPP
