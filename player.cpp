#include "player.hpp"
#include <float.h>
#include <algorithm>

int sgn(double x)
{
    if (x<0) return -1;
    if (x>0) return 1;
    return 0;
}

////////////////////[Personnage]////////////////////
Player::Player():
    score(0), gravity(GRAVITY_EFFECT)
{}
Player::~Player()
{}

bool Player::collide_with(Object* objet)
{
    SDL_Rect rect = {(Sint16)x, (Sint16)y, width, height};
    return rects_collide(rect, objet->get_rect());
}

bool Player::collide_with(Player* player)
{
    SDL_Rect rect1 = {(Sint16)x, (Sint16)y, width, height},
             rect2 = {(Sint16)player->x, (Sint16)player->y, player->width, player->height};
    return rects_collide(rect1, rect2);
}

bool Player::collide_with(Item* item)
{
    SDL_Rect rect1 = {(Sint16)x, (Sint16)y, width, height},
             rect2 = {(Sint16)item->x, (Sint16)item->y, item->width, item->height};
    return rects_collide(rect1, rect2);
}

bool Player::collide_with(Flag* flag)
{
    SDL_Rect rect = {(Sint16)x, (Sint16)y, width, height};
    return rects_collide(rect, flag->get_rect());
}

void Player::update()
{}

void Player::show(SDL_Surface* screen)
{}

void Player::setup(Map* m, int x0, int y0)
{
    m_map = m;
    x = x0;
    y = y0;
    x_vel = 0;
    y_vel = 0;
    state = 0;
}


void Player::affine(int x_offset, int y_offset)
{
    for (int i=0; i<abs(x_offset); ++i)
        if (!try_move(sgn(x_offset), 0))
            break;
    for (int i=0; i<abs(y_offset); ++i)
        if (!try_move(0, sgn(y_offset)))
            break;
}

void Player::move(int x_offset, int y_offset)
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

void Player::gravity_effect()
{
    if (y_vel==0) y_vel = 1;
    else y_vel += gravity;
}

void Player::die() {}
///////////////////////////////////////////////////////////////////

////////////////////[Mario]////////////////////
void Mario::setup_boolean()
{
    flag_down = false;
    respawning = false;
    dead = false;
    dying = false;
    big = false;
    fire = false;
    allow_jump = false;
    growing = false;
    becoming_small = false;
    having_fire = false;
    arrived = false;
    invincible = false;
    end = false;
}
Mario::Mario(Map* m):
    x_acc(0),
    having_fire_state(BIG), cnt_to_fire(0),
    cnt_to_big(0), cnt_to_small(0), life(3)
{
    setup_boolean();
    coin = 0;
    cur_ind = 0;
    direction = right;
    state = STOP;
    setup(m, 100, 300);

    load_image();

    type = "mario";
}
Mario::~Mario()
{
    for (int i=0; i<2; ++i)
    {
        SDL_FreeSurface(transition[i]);
        SDL_FreeSurface(small_end[i]);
        SDL_FreeSurface(big_end[i]);
        SDL_FreeSurface(powered_end[i]);
        for (int j=0; j<2; ++j)
            for (int k=0; k<6; ++k)
                SDL_FreeSurface(image[i][j][k]);
    }
    SDL_FreeSurface(death);
}

int Mario::get_hp() { return life; }
bool Mario::is_dead() { return dead; }
SDL_Surface* Mario::get_image() { return image[LITTLE][right][STOP]; }

void Mario::init_position()
{
    x = 32;
    y = 384;
}

void Mario::jump()
{
    allow_jump = false;
    gravity = JUMP_GRAVITY;
    y += 1;
    if (m_map->collision(this))
    {
        //Autoriser le saut car on a un appuie
        y_vel = JUMP_IMPULSE;
        m_map->game_sound->play(big?"big_jump":"small_jump");
        initial_h = y-1;
    }
    y -= 1;
}

void Mario::shot()
{
    if (fire and timer_for_next_fireball.as_milliseconds() >= 600)
    {
        Fireball* flamme = new Fireball(x, y, m_map);
        flamme->x_vel = direction?-FIRE_SPEED:FIRE_SPEED;
        fireball_list.push_back(flamme);
        timer_for_next_fireball.restart();
    }
}

void Mario::update_size()
{
    width = image[big][direction][STOP]->w;
    height = image[big][direction][STOP]->h;
}

void Mario::update_frame()
{
    if (x_vel)
        if (timer.as_milliseconds() > 350/fabs(x_vel))
        {
            timer.restart();
            if (y_vel >= 0) state = (state+1)%3+1;
        }
    if (!(int)x_vel) state = STOP;
    if (y_vel < 0)
    {
        state = JUMP;
        //allow_jump = false;
    }
}

void Mario::handle_keys()
{
    float acc(big?0.125:MARIO_ACC);
    if (!keys) return;
    if (keys[SDLK_LEFT])
    {
        if (x_vel > 2)
        {
            state = DRIFT;
            x_vel -= .05;
        }
        else
            x_vel -= acc;
    }
    else if (keys[SDLK_RIGHT])
    {
        if (x_vel < -2)
        {
            state = DRIFT;
            x_vel += .05;
        }
        else
            x_vel += acc;
    }
    if (!keys[SDLK_q])
    {
        allow_jump = true;
        gravity = GRAVITY_EFFECT;
    }
    else
    {
        if (allow_jump)
            jump();
    }
    if (fabs(x_vel) > MARIO_MAX_SPEED) x_vel = MARIO_MAX_SPEED*sgn(x_vel);
    x_vel *= MARIO_DECC;
}

void Mario::check_invincibility_timer()
{
    if (invincible)
    {
        if (invincible_timer.as_second() > 11)
        {
            invincible = false;
            m_map->game_sound->play("world1_1");
        }
        if (transformation_timer.as_milliseconds() > 60)
        {
            transformation_timer.restart();
            if (big)
                invincible_state = (invincible_state == BIG_TRANSITION)?BIG:BIG_TRANSITION;
            else if (fire)
                invincible_state = (invincible_state == BIG_TRANSITION)?POWER:BIG_TRANSITION;
            else
                invincible_state = (invincible_state == SMALL_TRANSITION)?LITTLE:SMALL_TRANSITION;
            current_image = image[invincible_state][direction][state];
        }
    }
}

void Mario::update_fireball_list()
{
    Fireball* fireball(NULL);
    for (int i=0; i<(int)fireball_list.size(); ++i)
        if (m_map->camera->x0< fireball_list[i]->x+12 and fireball_list[i]->x <m_map->camera->x0+WIDTH)
        {
            fireball_list[i]->update();
            if (fireball_list[i]->bumped)
            {
                fireball = fireball_list[i];
                fireball_list.erase(std::remove(fireball_list.begin(), fireball_list.end(), fireball_list[i]), fireball_list.end());
                delete fireball;
                //fireball_list = remove_from(fireball_list, i);
            }
        }
}

void Mario::update_direction()
{
    if (x_vel < 0) direction = left;
    else if (x_vel > 0) direction = right;
}

void Mario::handle_y_vel()
{
    y += 2;
    //y a-t-il quelque chose en dessous?
    int ret(m_map->collision(this, DOWN));
    if (ret < 0)
        y_vel = -4;
    else if (ret > 0)
        y_vel = 0;
    else
        state = JUMP;
    y -= 4;
    //y a-t-il quelque chose au dessus?
    if (m_map->collision(this, UP))
        y_vel = PUSH_DOWN;
    y += 2;
    if (!((int)y_vel))
        gravity = GRAVITY_EFFECT;
}

void Mario::check_boundaries()
{
     int s(sgn(x_vel));
    x += 2*s;
    if (m_map->collision(this))
        x_vel = 0;
    x -= 2*s;
    if (x < m_map->camera->x0)
    {
        x = m_map->camera->x0;
        x_vel = 0;
    }
}

void Mario::transition_to_small()
{
    int delay(10);
    SDL_Surface* images[3] = {image[LITTLE][direction][state],image[BIG][direction][state], NULL};
    if (transformation_timer.as_milliseconds() >= delay)
    {
        transformation_timer.restart();
        current_image = (cnt_to_small%2)?images[2]:images[rand()%3];
        cnt_to_small ++;
    }
    if (cnt_to_small >= 600/delay)
    {
        (*pause) = false;
        becoming_small = false;
        big = false;
        respawning = true;
        respawn_timer.restart();
        regulateur.restart();
    }
}

void Mario::respawn()
{
    if (regulateur.as_milliseconds() >= 30)
    {
        regulateur.restart();
        current_image = (current_image)?NULL:image[LITTLE][direction][state];
    }
}

void Mario::rewind()
{
    init_position();
    setup_boolean();
}

void Mario::transition_to_big()
{
    SDL_Surface* images[6] = { transition[direction], transition[direction], transition[direction],
                               image[LITTLE][direction][STOP], image[LITTLE][direction][STOP],
                               image[BIG][direction][STOP]};
    if (transformation_timer.as_milliseconds() >= TRANSITION_DELAY)
    {
        transformation_timer.restart();
        current_image = images[rand()%6];
        cnt_to_big ++;
    }
    if (cnt_to_big == 1000/TRANSITION_DELAY)
    {
        (*pause) = false;
        growing = false;
        big = true;
        y -= 32;
    }
}

void Mario::transition_to_fire()
{
    if (transformation_timer.as_milliseconds() >= TRANSITION_DELAY_FIRE)
    {
        transformation_timer.restart();
        current_image = image[having_fire_state][direction][state];
        if (having_fire_state == BIG) having_fire_state = BIG_TRANSITION;
        else if (having_fire_state == BIG_TRANSITION) having_fire_state = POWER;
        else if (having_fire_state == POWER) having_fire_state = BIG;
        cnt_to_fire ++;
    }
    if (cnt_to_fire == 1000/TRANSITION_DELAY_FIRE)
    {
        (*pause) = false;
        having_fire = false;
        fire = true;
    }
}

void Mario::dying_animation()
{
    if (y > HEIGHT + m_map->camera->y0)
    {
        (*pause) = false;
        dying = false;
        dead = true;
        gravity = GRAVITY_EFFECT;
        return;
    }
    if (death_timer.as_milliseconds() > 600)
    {
        y += y_vel;
        gravity = (y_vel<0)?0.1:0.12;
        gravity_effect();
    }
}

bool* Mario::adress() { return &flag_down; }

SDL_Surface* Mario::image_to_show()
{
    if (big)
        return fire?powered_end[cur_ind]:big_end[cur_ind];
    return small_end[cur_ind];
}

void Mario::finish()
{
    if (203*m_map->tile_w < x and x < 204*m_map->tile_w)
        arrived = true;
    if (y+height < 11*m_map->tile_h)
    {
        y += 4;
        if (regulateur.as_milliseconds() >= 50)
        {
            regulateur.restart();
            cur_ind = (cur_ind+1)%2;
        }
        current_image = image_to_show();
    }
    else
        if (flag_down)
        {
            update_size();
            x += 2;
            if (!m_map->collision(this))
                y += 2;
            if (timer.as_milliseconds() > 110)
            {
                timer.restart();
                state = (state+1)%3+1;
            }
            current_image = fire?image[POWER][direction][state]:image[big][direction][state];
        }
}

void Mario::set_end()
{
    if (!end)
    {
        int s(((int)(12*m_map->tile_h - (y+height))/m_map->tile_h)*100);
        end = true;
        current_image = image_to_show();
        regulateur.restart();
        score += s;
        m_map->text_list.push_back(new Text((int)x, (int)y, s, m_map->camera));
    }
}

void Mario::show(SDL_Surface* screen)
{
    if (arrived) return;

    SDL_Rect pos = m_map->camera->apply_to(this);
    SDL_BlitSurface(NULL, NULL, screen, &pos);
    if (growing or
         having_fire or
          invincible or
            becoming_small or
             respawning or
              dying or
               end)
    {
         if (current_image)
        {
            if (growing)
                pos.y = (Sint16)(transformation_h-current_image->h+32-m_map->camera->y0);
            SDL_BlitSurface(current_image, NULL, screen, &pos);
        }
    }
    else
        SDL_BlitSurface(image[fire?POWER:big][direction][state], NULL, screen, &pos);
    for (int i=0; i<(int)fireball_list.size(); ++i)
        if (fireball_list[i])
            fireball_list[i]->show(screen);
}

void Mario::update()
{
    if (!arrived)
    {
        if (respawning)
        {
            if (respawn_timer.as_second() < 3)
                respawn();
            else
                respawning = false;
        }
        if (growing) transition_to_big();
        else if (becoming_small) transition_to_small();
        else if (having_fire) transition_to_fire();
        else if (dying) dying_animation();
        else if (end) finish();
        else
        {
            if (*pause) return;
            update_size();
            update_frame();
            handle_keys();
            check_invincibility_timer();
            update_fireball_list();
            update_direction();
            gravity_effect();
            move(x_vel, y_vel);
            handle_y_vel();
            check_boundaries();
            if ( y-10 >= m_map->camera->y0 + HEIGHT )
            {
                m_map->game_sound->play("death");
                life --;
                SDL_Delay(3000);
                dead = true;
            }
        }
    }
}

void Mario::setup_keys(bool* KEYS)
{
    keys = KEYS;
}
void Mario::load_image()
{
    std::string dir,
                etat[6] = {"stand.bmp", "moving0.bmp", "moving1.bmp", "moving2.bmp", "jumping.bmp", "sprint.bmp"};

    for (int k=0; k<5; ++k)
    {
        for (int i=0; i<2; ++i)
        {
            switch (k)
            {
            case LITTLE:
                if (i) dir = "data/graphic/mario/little/left/";
                else dir = "data/graphic/mario/little/right/";
                break;
            case BIG:
                if (i) dir = "data/graphic/mario/big/left/";
                else dir = "data/graphic/mario/big/right/";
                break;
            case POWER:
                if (i) dir = "data/graphic/mario/big/left/powered/";
                else dir = "data/graphic/mario/big/right/powered/";
                break;
            case SMALL_TRANSITION:
                if (i) dir = "data/graphic/mario/little/left/transition/";
                else dir = "data/graphic/mario/little/right/transition/";
                break;
            case BIG_TRANSITION:
                if (i) dir = "data/graphic/mario/big/left/transition/";
                else dir = "data/graphic/mario/big/right/transition/";
                break;
            default: ;
            }
            for (int j=0; j<6; ++j)
            {
                image[k][i][j] = SDL_LoadBMP((dir + etat[j]).c_str());
                if (!image[k][i][j])
                {
                    std::cerr << SDL_GetError();
                    exit(1);
                }
                if (k == BIG_TRANSITION or k == SMALL_TRANSITION)
                    SDL_SetColorKey(image[k][i][j], SDL_SRCCOLORKEY, 0xff00ff);
                else
                    SDL_SetColorKey(image[k][i][j], SDL_SRCCOLORKEY, 0xffffff);
            }
        }
    }
    transition[left] = SDL_LoadBMP("data/graphic/mario/transition/left.bmp");
    SDL_SetColorKey(transition[left], SDL_SRCCOLORKEY, 0xffffff);
    transition[right] = SDL_LoadBMP("data/graphic/mario/transition/right.bmp");
    SDL_SetColorKey(transition[right], SDL_SRCCOLORKEY, 0xffffff);
    death = SDL_LoadBMP("data/graphic/mario/mario_die.bmp");
    SDL_SetColorKey(death, SDL_SRCCOLORKEY, 0x0);

    small_end[0] = SDL_LoadBMP("data/graphic/mario/little/end0.bmp");
    small_end[1] = SDL_LoadBMP("data/graphic/mario/little/end1.bmp");
    big_end[0] = SDL_LoadBMP("data/graphic/mario/big/end0.bmp");
    big_end[1] = SDL_LoadBMP("data/graphic/mario/big/end1.bmp");
    powered_end[0] = SDL_LoadBMP("data/graphic/mario/big/powered_end0.bmp");
    powered_end[1] = SDL_LoadBMP("data/graphic/mario/big/powered_end1.bmp");

    for (int i=0; i<2; ++i)
    {
        SDL_SetColorKey(small_end[i], SDL_SRCCOLORKEY, 0xffffff);
        SDL_SetColorKey(big_end[i], SDL_SRCCOLORKEY, 0xffffff);
        SDL_SetColorKey(powered_end[i], SDL_SRCCOLORKEY, 0xffffff);
    }
}
void Mario::set_invincible()
{
    invincible = true;
    invincible_state = big?BIG_TRANSITION:SMALL_TRANSITION;
    invincible_timer.restart();
    transformation_timer.restart();
    current_image = image[big][direction][state];
}
bool Mario::try_move(int x_offset, int y_offset)
{
    x += x_offset;
    y += y_offset;
    if (!m_map->collision(this))
        return true;
    x -= x_offset;
    y -= y_offset;

    return false;
}
bool Mario::is_invincible() { return invincible; }
bool Mario::is_big()        { return big; }
void Mario::add_hp()        { life++; }
void Mario::allow_fire()
{
    (*pause) = true;
    having_fire = true;
    cnt_to_fire = 0;
    transformation_timer.restart();
    current_image = image[BIG][direction][state];
}
void Mario::grow_up()
{
    (*pause) = true;
    growing = true;
    cnt_to_big = 0;
    transformation_h = (int)y;
    transformation_timer.restart();
    current_image = transition[direction];
}
void Mario::shrink()
{
    (*pause) = true;
    becoming_small = true;
    cnt_to_small = 0;
    fire = false;
    transformation_h = (int)y;
    transformation_timer.restart();
}

void Mario::die()
{
    if (!dying)
    {
        m_map->game_sound->play("death");
        y_vel = -5;
        (*pause) = true;
        dying = true;
        life --;
        current_image = death;
        death_timer.restart();
    }
}
////////////////////[ENEMIES]////////////////////
Enemy::Enemy(int x0, int y0, Map* m):
    kicked(false)
{
    squished = false;
    kicked = false;
    direction = left;
    status = UNDEFINED;
    setup(m, x0, y0);
    score = 100;

    type = "enemy";
}
Enemy::~Enemy()
{}
void Enemy::jumped()
{
    squished = true;
}
void Enemy::show(SDL_Surface* screen)
{}
void Enemy::update()
{}
void Enemy::die()
{
    kicked = true;
    y_vel = -8;
    x_vel = 0;
}
void Enemy::set_status(int new_status) {status = new_status;}
bool Enemy::status_is(int s) {return status == s;}
bool Enemy::try_move(int x_offset, int y_offset)
{
    x += x_offset;
    y += y_offset;
    if (!m_map->collision(this))
        return true;
    x -= x_offset;
    y -= y_offset;
    return false;
}
