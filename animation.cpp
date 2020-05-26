#include "animation.hpp"
#include <cmath>

Animation::Animation(SDL_Surface* master):
    screen(master), running(true)
{
    for (int i=0; i<SDLK_LAST; ++i)
        keys[i] = false;
}
Animation::~Animation() {}

Show_logo::Show_logo(SDL_Surface* master, Sound* sound):
    Animation(master)
{
    transparency = 0;
    m_sound = sound;

    acf = SDL_LoadBMP("data/graphic/acf.bmp");
    if (!acf)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_SetColorKey(acf, SDL_SRCCOLORKEY, 0xffffff);

    acf_pos.x = (Sint16)((WIDTH-acf->w)/2);
    acf_pos.y = (Sint16)((HEIGHT-acf->h)/2);

    fg_pos.x = 0;
    fg_pos.y = 0;
    fg = SDL_CreateRGBSurface(SDL_HWSURFACE, WIDTH, HEIGHT, 32, 0, 0, 0, 0);
    SDL_FillRect(fg, NULL, 0x0);

    to_show.x = 0;
    to_show.y = 0;
    to_show.w = 0;
    to_show.h = acf->h;

}

Show_logo::~Show_logo()
{
    SDL_FreeSurface(acf);
}

bool Show_logo::run()
{
    while (running)
    {
        while (SDL_PollEvent(&event))
            handle_events();
        update();
        show();
        SDL_Flip(screen);
        SDL_Delay(2);
    }
    return false;
}

void Show_logo::handle_events()
{
    running = !(event.type == SDL_QUIT);
    if (event.type == SDL_MOUSEBUTTONUP)
        SDL_SaveBMP(screen, "screenshot.bmp");
}

void Show_logo::update()
{
    if (timer.as_milliseconds() >= 10)
    {
        timer.restart();
        if (to_show.w >= acf->w)
        {
            if (!transparency)
                m_sound->play("logo");
            transparency++;
        }
        else
            to_show.w += 1;
        SDL_SetAlpha(fg, SDL_SRCALPHA, transparency);
    }
    if (transparency >= 255)
        running = false;
}

void Show_logo::show()
{
    SDL_FillRect(screen, NULL, 0x0);
    SDL_BlitSurface(acf, &to_show, screen, &acf_pos);
    SDL_BlitSurface(fg, NULL, screen, &fg_pos);
}

ParticleSystem::ParticleSystem(int source_x, int source_y):
    particles(500)
{
    source.x = source_x;
    source.y = source_y;
    for (int i=0; i<(int)particles.size(); ++i)
        init_particle(i);
    particle_velocity = 10;
}
ParticleSystem::~ParticleSystem()
{}

void ParticleSystem::init_particle(int index)
{
    int angle(rand()%360);
    particles[index].x = source.x;
    particles[index].y = source.y;

    particles[index].x_vel = particle_velocity*cos(angle);
    particles[index].y_vel = particle_velocity*sin(angle);
}

void ParticleSystem::move()
{
    source.x += 1;
}

void ParticleSystem::show(SDL_Surface* screen)
{
    SDL_Rect pos;
    pos.w = 1;
    pos.h = 1;
    for (int i=0; i<(int)particles.size(); ++i)
    {
        pos.x = (Sint16)particles[i].x;
        pos.y = (Sint16)particles[i].y;
        SDL_FillRect(screen, &pos, 0xffffff);
    }
}

void ParticleSystem::update()
{
    for (int i=0; i<(int)particles.size(); ++i)
    {
        particles[i].x += particles[i].x_vel;
        particles[i].y += particles[i].y_vel;
        if (!((0< particles[i].x and particles[i].x <WIDTH) and
                (0< particles[i].y and particles[i].y <HEIGHT)))
            init_particle(i);
    }
}

void ParticleSystem::set_source(int source_x, int source_y)
{
    source.x = (Sint16)source_x;
    source.y = (Sint16)source_y;
    for (int i=0; i<(int)particles.size(); ++i)
        init_particle(i);
}
