#include "text.hpp"
#include "const.hpp"
#include "time.hpp"
#include <sstream>

Text::Text(int _x, int _y, const std::string& content, Camera* CAMERA,  std::string font_name, size_t char_size):
    initial_h(_y), camera(CAMERA)
{
    x = _x;
    y = _y;
    y_vel = -2;
    font = TTF_OpenFont(("data/fonts/"+font_name).c_str(), char_size);
    if (!font)
    {
        std::cerr << TTF_GetError();
        exit(1);
    }
    set_text(content);
}
Text::Text(int _x, int _y, int score, Camera* CAMERA, std::string font_name, size_t char_size):
    initial_h(_y), camera(CAMERA)
{
    std::stringstream str;
    str << score;
    x = _x;
    y = _y;
    y_vel = -3;
    font = TTF_OpenFont(("data/fonts/"+font_name).c_str(), char_size);
    if (!font)
    {
        std::cerr << TTF_GetError();
        exit(1);
    }
    set_text(str.str());
}

Text::~Text()
{
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    font = NULL;
}

bool Text::is_to_remove()
{
    return (y <= initial_h - 100);
}

void Text::update()
{
    y += y_vel;
}

void Text::show(SDL_Surface* screen)
{
    SDL_Rect pos;
    if (camera)
        pos = camera->apply_to(this);
    else
    {
        pos.x = (Sint16)x;
        pos.y = (Sint16)y;
    }
    SDL_BlitSurface(surface, NULL, screen, &pos);
}

void Text::set_text(const std::string& content)
{
    text = content;
    surface = TTF_RenderText_Solid(font, content.c_str(), WHITE);
}
//////////////////////////////////////////////////////////////////

Score::Score(int* mario_score):
    Text(50, 10, "MARIO", NULL, "emulogic.ttf") , score(mario_score),
    score_surf(NULL)
{
    update();
}
Score::~Score()
{
    SDL_FreeSurface(surface);
    SDL_FreeSurface(score_surf);
    TTF_CloseFont(font);
    font = NULL;
}
void Score::update()
{
    std::stringstream str, tmp;
    tmp << *score;
    for (int i=0; i<(int)(6-tmp.str().size()); ++i)
        str << 0;
    str << *score;
    SDL_FreeSurface(score_surf);
    score_surf = TTF_RenderText_Solid(font, str.str().c_str(), WHITE);
}
void Score::show(SDL_Surface* screen)
{
    SDL_Rect pos = {(Sint16)x, (Sint16)y};
    SDL_BlitSurface(surface, NULL, screen, &pos);
    if (score_surf)
    {
        pos.x = (Sint16)((pos.x+surface->w*.5)-score_surf->w*.5);
        pos.y += (surface->h + 2);
        SDL_BlitSurface(score_surf, NULL, screen, &pos);
    }
}

Coin_counter::Coin_counter(int* coin_eff):
    Text(200, 30, " x 00", NULL, "font.ttf", 20),
    state(0), coin_counter(coin_eff)
{
    image = SDL_LoadBMP("data/graphic/items/coin.bmp");
    if (!image)
    {
        std::cerr << TTF_GetError();
        exit(1);
    }
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x5c94fc);
    regulator = new Clock();
}
Coin_counter::~Coin_counter()
{
    delete regulator;
    SDL_FreeSurface(image);
    SDL_FreeSurface(surface);
    TTF_CloseFont(font);
    font = NULL;
}
void Coin_counter::update()
{
    std::stringstream str;
    str << " x ";
    if (regulator->as_milliseconds() >= ((!state)?250:150))
    {
        regulator->restart();
        state = (state+1)%4;
    }
    if ((*coin_counter) < 10)
        str << "0";
    str << (*coin_counter);
    SDL_FreeSurface(surface);
    surface = TTF_RenderText_Solid(font, str.str().c_str(), WHITE);
}
void Coin_counter::show(SDL_Surface* screen)
{
    SDL_Rect pos = {(Sint16)x, (Sint16)y},
            zone = {(Sint16)(state*15), 0, 15, 21};
    SDL_BlitSurface(image, &zone, screen, &pos);
    pos.x += 15;
    SDL_BlitSurface(surface, NULL, screen, &pos);
}

Indicator::Indicator():
    Text(WIDTH-250, 10, "WORLD", NULL, "emulogic.ttf"),
    tmp(TTF_RenderText_Solid(font, "1-1", WHITE))
{}
Indicator::~Indicator()
{
    SDL_FreeSurface(surface);
    SDL_FreeSurface(tmp);
    TTF_CloseFont(font);
    font = NULL;
}
void Indicator::update(){}
void Indicator::show(SDL_Surface* screen)
{
    SDL_Rect pos = {(Sint16)x, (Sint16)y};
    SDL_BlitSurface(surface, NULL, screen, &pos);
    pos.x = (Sint16)((pos.x+surface->w*.5)-tmp->w*.5);
    pos.y += (surface->h + 2);
    SDL_BlitSurface(tmp, NULL, screen, &pos);
}

