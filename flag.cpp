#include <iostream>
#include "flag.hpp"

Flag::Flag(Map *m):
    _map(m),
    y_vel(4),
    sliding_down(false), yet(false)
{
    spritesheet = m->tileset;
    width = m->tile_w;
    height = m->tile_h;
    flag = SDL_LoadBMP("data/graphic/items/flag.bmp");
    if (!flag)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    x = 6877;
    y = 2*height;
}

Flag::~Flag()
{
    SDL_FreeSurface(flag);
}

bool Flag::is_down()
{
    return (y+flag->h >= 11*_map->tile_h);
}

void Flag::update()
{
    if (sliding_down)
    {
        if (!yet)
        {
            _map->game_sound->play("flag");
            yet = true;
        }
        y += y_vel;
        if (y+flag->h >= 11*_map->tile_h)
        {
            sliding_down = false;
            y_vel = 0;
            (*player_flag_boolean) = true;
        }
    }
}

void Flag::slide_down()
{
    sliding_down = true;
}

void Flag::show(SDL_Surface* screen)
{
    SDL_Rect rect = {(Sint16)(16*width), (Sint16)(11*height), height, height},
              pos = {(Sint16)(197*width - _map->camera->x0), 0, height, height};
    for (Sint16 i=2; i<11; ++i)
    {
        pos.y = (Sint16)(i*height - _map->camera->y0);
        SDL_BlitSurface(spritesheet, &rect, screen, &pos);
    }
    rect.x = 16*width;
    rect.y = 8*width;
    pos.y = (Sint16)(height - _map->camera->y0);
    SDL_BlitSurface(spritesheet, &rect, screen, &pos);
    pos.x = (Sint16)(x-_map->camera->x0);
    pos.y = (Sint16)(y-_map->camera->y0);
    SDL_BlitSurface(flag, NULL, screen, &pos);
}

SDL_Rect Flag::get_rect()
{
    SDL_Rect ret = {6913, 20, 10, 350};
    return ret;
}
