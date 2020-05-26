#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#include <iostream>
#include <vector>
#include <SDL.h>
#include "time.hpp"
#include "sound.hpp"
#include "const.hpp"

class Animation
{
public:
    Animation(SDL_Surface* master);
    virtual bool run() = 0;
    virtual ~Animation();
protected:
    SDL_Surface* screen;
    SDL_Event event;
    bool keys[SDLK_LAST], running;
    virtual void handle_events() = 0;
    virtual void show() = 0;
    virtual void update() = 0;
};

class ParticleSystem
{
public:
    ParticleSystem(int source_x = WIDTH/2, int source_y = HEIGHT/2);
    virtual ~ParticleSystem();
    void move();
    void show(SDL_Surface* screen);
    virtual void update();
    void set_source(int source_x, int source_y);

protected:
    struct Particle
    {
        float x_vel, y_vel;
        float x, y;
    };

    float particle_velocity;
    std::vector< Particle > particles;
    SDL_Rect source;
    void init_particle(int index);
};

class Show_logo: public Animation
{
public:
    Show_logo(SDL_Surface* master, Sound* sound);
    ~Show_logo();
    bool run();
private:
    Clock timer;
    int transparency;
    SDL_Surface *acf, *fg;
    SDL_Rect acf_pos, fg_pos, to_show;
    Sound* m_sound;
    void handle_events();
    void show();
    void update();
};
#endif // ANIMATION_HPP
