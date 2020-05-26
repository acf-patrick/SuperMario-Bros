#include "enemies.hpp"

std::vector< Enemy* > remove_from(std::vector< Enemy* > liste, int index)
{
    if (index)
    {
        for (int i=index+1; i < (int)liste.size(); ++i)
            liste[i-1] = liste[i];
        liste.resize(liste.size()-1);
    }
    else
        liste.erase(liste.begin());
    return liste;
}

////////////////////[GOOMBAS]////////////////////
Goomba::Goomba(int x0, int y0, Map* m): Enemy(x0, y0, m)
{
    SDL_Surface *spritesheet = SDL_LoadBMP("data/graphic/enemies/goomba.bmp");
    if (!spritesheet)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }

    SDL_Rect dest = {0, 0, (Uint16)(spritesheet->w/4), (Uint16)spritesheet->h}, pos = {0, 0};

    width = dest.w;
    height = dest.h;

    for (int i=0; i<4; ++i)
    {
        dest.x = i*dest.w;
        image[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, dest.w, dest.h, 32,
                                        0, 0, 0, 0);
        SDL_BlitSurface(spritesheet, &dest, image[i], &pos);
        SDL_SetColorKey(image[i], SDL_SRCCOLORKEY, 0xFFFFFF);
    }
    x_vel = -GOOMBA_SPEED;

    type = "goomba";
}
Goomba::~Goomba()
{
    for (int i=0; i<3; ++i)
        SDL_FreeSurface(image[i]);
}
void Goomba::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this);
    if (kicked)
        SDL_BlitSurface(image[3], NULL, screen, &pos);
    else
    {
        if (!squished)
            SDL_BlitSurface(image[direction], NULL, screen, &pos);
        else
            SDL_BlitSurface(image[2], NULL, screen, &pos);
    }
}

void Goomba::update()
{
    if (squished)
    {
        x_vel = 0;
    }
    gravity_effect();
    move(x_vel, y_vel);

    y += 2;
    if (m_map->collision(this))
        y_vel = 0;
    y -= 2;

    int s(sgn(x_vel));
    x += 2*s;
    if (m_map->collision(this) or x < 0)
        x_vel *= -1;
    x -= 2*s;

    //if (x < 0) x = 0;
    if (timer.as_milliseconds() > GOOMBA_NEXT_ANIM)
    {
        timer.restart();
        direction = (direction+1)%2;
    }
}
////////////////////////////////////////////////////////////

////////////////////[KOOPAS]////////////////////
Koopa::Koopa(int x0, int y0, Map* m):
    Enemy(x0, y0, m)
{
    SDL_Surface* spritesheet = SDL_LoadBMP("data/graphic/enemies/koopa.bmp");
    if (!spritesheet)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }

    SDL_Rect dest = {0, 0, (Uint16)(spritesheet->w/2), (Uint16)(spritesheet->h/2)}, pos = {0, 0};

    width = dest.w;
    height = dest.h;

    for (int i=0; i<2; ++i)
        for (int j=0; j<2; ++j)
        {
            dest.x = j*dest.w;
            dest.y = i*dest.h;
            image[i][j] = SDL_CreateRGBSurface(SDL_HWSURFACE, dest.w, dest.h, 32,
                                        0, 0, 0, 0);
            SDL_BlitSurface(spritesheet, &dest, image[i][j], &pos);
            SDL_SetColorKey(image[i][j], SDL_SRCCOLORKEY, 0x0000FF);
        }
    shell = SDL_LoadBMP("data/graphic/enemies/shell.bmp");
    if (!shell)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }

    SDL_SetColorKey(shell, SDL_SRCCOLORKEY, 0x0000FF);

    x_vel = -GOOMBA_SPEED;

    type = "koopa";
    gravity = KOOPA_GRAVITY;
}

Koopa::~Koopa()
{
    for (int i=0; i<2; ++i)
        for (int j=0; j<2; ++j)
            SDL_FreeSurface(image[i][j]);
    SDL_FreeSurface(shell);
}

void Koopa::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this);
    if (status == STAY_IN_SHELL or status == MOVING_IN_SHELL or kicked)
        SDL_BlitSurface(shell, NULL, screen, &pos);
    else
        SDL_BlitSurface(image[direction][state], NULL, screen, &pos);
}

void Koopa::update()
{
    if (status == STAY_IN_SHELL or status == MOVING_IN_SHELL)
        height = shell->h;

    gravity_effect();
    move(x_vel, y_vel);

    y += 2;
    if (m_map->collision(this))
        y_vel = 0;
    y -= 2;

    int s(sgn(x_vel));
    x += 2*s;
    if (m_map->collision(this) or x < 0)
        x_vel *= -1;
    x -= 2*s;

    if (x < 0) x = 0;
    if (timer.as_milliseconds() > GOOMBA_NEXT_ANIM)
    {
        timer.restart();
        state = (state+1)%2;
    }
    if (x_vel < 0) direction = left;
    if (x_vel > 0) direction =right;
}
