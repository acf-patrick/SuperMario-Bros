#ifndef TEXT_HPP
#define TEXT_HPP
#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>
#include "entity.hpp"
#include "camera.hpp"

class Clock;

class Text: public Base
{
public:
    Text(int _x, int _y, const std::string& content, Camera* CAMERA = NULL, std::string font_name = "font.ttf", size_t char_size = 15);
    Text(int _x, int _y, int score, Camera* CAMERA = NULL, std::string font_name = "font.ttf", size_t char_size = 15);
    virtual ~Text();
    virtual void update();
    virtual void show(SDL_Surface* screen);
    bool is_to_remove();
    void set_text(const std::string& content);
protected:
    std::string text;
    SDL_Surface* surface;
    int initial_h;
    float y_vel;
    TTF_Font* font;
private:
    Camera* camera;
};

class Score: public Text
{
public:
    Score(int* mario_score);
    ~Score();
    void update();
    void show(SDL_Surface* screen);
private:
    int *score;
    SDL_Surface* score_surf;
};

class Indicator: public Text
{
public:
    Indicator();
    ~Indicator();
    void update();
    void show(SDL_Surface* screen);
private:
    SDL_Surface* tmp;
};

class Coin_counter: public Text
{
public:
    Coin_counter(int* coin_eff);
    ~Coin_counter();
    void update();
    void show(SDL_Surface* screen);
private:
    SDL_Surface* image;
    Clock* regulator;
    int state, *coin_counter;
};
#endif // TEXT_HPP
