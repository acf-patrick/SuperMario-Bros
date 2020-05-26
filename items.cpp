#include "items.hpp"
#include "const.hpp"
#include "player.hpp"

Coin* six_coins(int x0, int y0, Map* m)
{
    Coin* ret = new Coin(x0, y0, m);
    ret->next = new Coin(x0, y0, m);
    ret->next->next = new Coin(x0, y0, m);
    ret->next->next->next = new Coin(x0, y0, m);
    ret->next->next->next->next = new Coin(x0, y0, m);
    ret->next->next->next->next->next = new Coin(x0, y0, m);

    return ret;
}

//////////[Basic object]//////////
Item::Item(int x0, int y0, Map* m):
    x_vel(0), y_vel(0),
    initial_height(y0), discovered(false),
    eaten(false),  bumped(false),
    m_map(m), image(NULL),
    type("basic_item"), next(NULL), appear(false)
{
    x = x0;
    y = y0;
}

Item::~Item()
{
    SDL_FreeSurface(image);
}

void Item::update()
{}

bool Item::in_initial_position()
{
    return y == initial_height;
}

void Item::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this);
    if (image)
      SDL_BlitSurface(image, NULL, screen, &pos);
}

bool Item::collide_with(Player* mario)
{
    return mario->collide_with(this);
}

bool Item::collide_with(Object* object)
{
    SDL_Rect item_rect = {(Sint16)x, (Sint16)y, width, height};
    return rects_collide(item_rect, object->get_rect());
}
//////////[Coins found in question bloc and bricks]//////////
Coin::Coin(int x0, int y0, Map* m):
    Item(x0+7, y0+7, m), state(0)
{
    image = SDL_LoadBMP("data/graphic/items/coins.bmp");
    if (!image)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x0000FF);
    width  = image->w/4;
    height = image->h;
    type = "coin";
    score = 200;
}

void Coin::update()
{
    if (timer.as_milliseconds() >= 30)
    {
        timer.restart();
        state = (state+1)%4;
    }
    y_vel += 1.25;
    y += y_vel;
    if (y > initial_height)
    {
        y_vel = 0;
        y = initial_height;
    }
}

void Coin::show(SDL_Surface* screen)
{
    SDL_Rect dest = {(Sint16)(state*m_map->tile_w), 0, width, height},
             pos = m_map->camera->apply_to(this);
    SDL_BlitSurface(image, &dest, screen, &pos);
}
/////////////////////////////////////////////////////:
Powerup::Powerup(int x0, int y0, Map* m):
    Item(x0, y0, m)
{
    type = "power_up";
    score = 1000;
}

void Powerup::move(int x_offset, int y_offset)
{
    if (x_offset >= m_map->tile_w or y_offset >= m_map->tile_h)
    {
        move(x_offset/2, y_offset/2);
        move(x_offset-x_offset/2, y_offset-y_offset/2);
        return;
    }
    if (try_move(x_offset, y_offset))
        return;
    affine(x_offset, y_offset);
}

void Powerup::affine(int x_offset, int y_offset)
{
    for (int i=0; i<abs(x_offset); ++i)
        if (!try_move(sgn(x_offset), 0))
            break;
    for (int i=0; i<abs(y_offset); ++i)
        if (!try_move(0, sgn(y_offset)))
            break;
}

bool Powerup::try_move(int x_offset, int y_offset)
{
    x += x_offset;
    y += y_offset;
    if (!m_map->collision(this))
        return true;
    x -= x_offset;
    y -= y_offset;
    return false;
}
/////////////////////////////////////////
Mushroom::Mushroom(int x0, int y0, Map* m):
    Powerup(x0, y0, m)
{
    eaten = false;

    image = SDL_LoadBMP("data/graphic/items/mushroom.bmp");
    if (!image)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x0000FF);
    width  = image->w;
    height = image->h;
    x_vel = 3;
}

void Mushroom::update()
{
    if (y < initial_height-height)
        appear = true;
    if (discovered)
    {
        if (appear)
        {
            if (y_vel == 0) y_vel = 1;
            else y_vel += 0.4;
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
        }
        else
            y -= 1;
    }
}

Life::Life(int x0, int y0, Map* m):
    Mushroom(x0, y0, m)
{
    image = SDL_LoadBMP("data/graphic/items/1_up.bmp");
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x0000ff);
    type = "1_up";
}
////////////////////////////////////////
Flower::Flower(int x0, int y0, Map* m):
    Powerup(x0, y0, m), state(0)
{
    image = SDL_LoadBMP("data/graphic/items/flower.bmp");
    if (!image)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x0000ff);
    width = image->w/4;
    height = image->h;
}

void Flower::update()
{
    if (timer.as_milliseconds() >= 30)
    {
        timer.restart();
        state = (state+1)%4;
    }
    if (discovered)
    {
        if (y < initial_height-height)
            y_vel = 0;
        else
            y_vel = -1;
    }
    y += y_vel;
}

void Flower::show(SDL_Surface* screen)
{
    SDL_Rect dest = {(Sint16)(state*width), 0, width, height},
              pos = m_map->camera->apply_to(this);

    SDL_BlitSurface(image, &dest, screen, &pos);
}
////////////////////////////////////////

Star::Star(int x0, int y0, Map* m):
    Powerup(x0, y0, m), state(0)
{
    image = SDL_LoadBMP("data/graphic/items/star.bmp");

    if (!image)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    width = image->w/4;
    height = image->h;
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x5c94fc);
    x_vel = 4;
    type = "star";
}

void Star::show(SDL_Surface* screen)
{
    SDL_Rect dest = {(Sint16)(state*width), 0, width, height},
              pos = m_map->camera->apply_to(this);

    SDL_BlitSurface(image, &dest, screen, &pos);
}

void Star::update()
{
    bool already_in(false);

    if (timer.as_milliseconds() >= 150)
    {
        timer.restart();
        state = (state+1)%4;
    }
    if (y < initial_height-height)
        appear = true;
    if (discovered)
    {
        if (appear)
        {
            y_vel+=GRAVITY_EFFECT;
            move(x_vel, y_vel);

            y += 2;
            if (m_map->collision(this))
            {
                y_vel = -STAR_VEL;
                already_in = true;
            }
            y -= 4;
            if (!already_in and m_map->collision(this))
                y_vel = STAR_VEL;
            y += 2;

            int s(sgn(x_vel));
            x += 2*s;
            if (m_map->collision(this) or x < 0)
                x_vel *= -1;
            x -= 2*s;
        }
        else
            y -= 1;
    }
}
////////////////////////////////////////
Fireball::Fireball(int x0, int y0, Map* m):
    Powerup(x0, y0, m), state(0), frame(0), firework(false)
{
    image = SDL_LoadBMP("data/graphic/items/fireball.bmp");
    bump  = SDL_LoadBMP("data/graphic/items/firework.bmp");
    if (!image or !bump)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    width = image->w/2;
    height = image->h/2;
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x0000ff);
    SDL_SetColorKey(bump, SDL_SRCCOLORKEY, 0x0000ff);
    type = "fire";
}

void Fireball::show(SDL_Surface* screen)
{
    SDL_Rect dest = {(Sint16)((state%2)*width), (Sint16)((state/2)*height), width, height},
            dest2 = {(Sint16)(frame*32), 0, 32, 32},
              pos = m_map->camera->apply_to(this);
    if (firework)
        SDL_BlitSurface(bump, &dest2, screen, &pos);
    else
        SDL_BlitSurface(image, &dest, screen, &pos);
}

void Fireball::update()
{
    if (firework and timer.as_milliseconds() >= 50)
    {
        frame++;
        timer.restart();
    }
    if (frame>=3)
    {
        bumped=true;
        return;
    }
    if (!bumped and !firework)
    {
        if (timer.as_milliseconds() >= 1)
        {
            timer.restart();
            state = (state+1)%4;
        }
        y_vel += 2*GRAVITY_EFFECT;
        move(x_vel, y_vel);
        y += 2;
        if (m_map->collision(this))
            y_vel = -8;
        y -= 2;
        int s(sgn(x_vel));
        x += 2*s;
        if (m_map->collision(this) or x < 0)
        {
            if (!bumped)
            {
                bumped = true;
                m_map->game_sound->play("bloc_bumped");
            }
        }
        x -= 2*s;
    }
    if (bumped)
    {
        firework = true;
        timer.restart();
        bumped = false;
    }
}
