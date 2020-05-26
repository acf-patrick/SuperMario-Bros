#include "obstacle.hpp"

bool rects_collide(SDL_Rect rect1, SDL_Rect rect2)
{
    if (
        rect1.y+rect1.h <= rect2.y ||
        rect1.y >= rect2.y+rect2.h ||
        rect1.x+rect1.w <= rect2.x ||
        rect1.x >= rect2.x+rect2.w
        )
        return false;
    return true;
}

/////////////////[Classe de base]/////////////////
void Object::set_content(Item* new_content)
{
    delete content;
    content = new_content;
    none = false;
}

Object::Object(int x0, int y0, Map* m):
	x_vel(0), y_vel(0),
	m_map(m), content(NULL),
	type(GROUND_BRICK), wall(false),
	none(true), bumped(false), destroyed(false)
{
    x = x0;
    y = y0;
    width =  m->tile_w;
    height = m->tile_h;
}

Object::~Object()
{
	delete content;
}

void Object::update()
{}

void Object::show(SDL_Surface* screen)
{}

SDL_Rect Object::get_rect()
{
    SDL_Rect ret = {(Sint16)x, (Sint16)y, m_map->tile_w, m_map->tile_h};
    return ret;
}

void Object::destroy()
{
    wall = false;
    destroyed = true;
}

bool Object::is_destroyed() {return destroyed;}
/////////////////////////////////////////////////////

/////////////////[Bloc avec une point d'interrogation]/////////////////
SDL_Surface* Question_mark::images[3] = {NULL};
Clock Question_mark::timer;
int Question_mark::state(0);

void Question_mark::load_image(Map* m)
{
	for (int i=0; i<3; ++i)
	{
		SDL_Rect pos = {0, 0},
                 dest = {(Sint16)((i+QUESTION)*m->tile_w), 0*m->tile_h, m->tile_w, m->tile_h};
		images[i] = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
										0, 0, 0, 0);
		SDL_BlitSurface(m->tileset, &dest, images[i], &pos);
		SDL_SetColorKey(images[i], SDL_SRCCOLORKEY, 0x5f75ca);
	}
}

Question_mark::Question_mark(int _x, int _y, Map* m):
	Object(_x, _y, m), y0(_y)
{
	wall = true;
	type = QUESTION;
	content = new Coin(_x, _y, m);
	none = false;
}

void Question_mark::update_state()
{
    //Mettre à jour state pour changer d'image dans la méthode 'show'
    int wait = !state?3*BRICK_NEXT_ANIM:BRICK_NEXT_ANIM;
    if (timer.as_milliseconds() > wait)
    {
        timer.restart();
        state = (state+1)%3;
    }
}

void Question_mark::update()
{
    Item* item_to_delete(NULL);
    if (!content)
    {
        y_vel = 0;
        y = y0;
    }
    else
    {
        if (content->discovered)
        {
            if (content->type == "coin")
            {
                content->update();
                item_to_delete = content;
                if (item_to_delete->in_initial_position())
                {
                    m_map->text_list.push_back(new Text((int)x, (int)y, content->score, m_map->camera));
                    content = content->next;
                    delete content;
                    item_to_delete = NULL;
                }
            }
            else
            {
                content = NULL;
                //y_vel = 0;
            }
        }

        //Physique
        if (y_vel < BLOC_BUMPED)
            y_vel = 0;
        y_vel += BRICK_GRAVITY;
        y += y_vel;
        if (y >= y0)
        {
            y_vel = 0;
            y = y0;
        }
    }
}

void Question_mark::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this),
            dest = {(Sint16)(3*m_map->tile_w), 0, m_map->tile_w, m_map->tile_h};
    if (content)
    {
        if (content->discovered)
        {
            if (content->type == "coin")
            {
                content->show(screen);
                SDL_BlitSurface(m_map->tileset, &dest, screen, &pos);
            }
            else
                SDL_BlitSurface(images[state], NULL, screen, &pos);
        }
        else
            SDL_BlitSurface(images[state], NULL, screen, &pos);
    }
    else
    {
        if (!none)
            SDL_BlitSurface(m_map->tileset, &dest, screen, &pos);
        else
            SDL_BlitSurface(images[state], NULL, screen, &pos);
    }
}

///////////////////////////////////////////////////////////////////////

/////////////////[Brique]/////////////////
SDL_Surface* Brick::image = NULL;

void Brick::load_image(Map* m)
{
	SDL_Rect pos = {0, 0},
			dest = {(Sint16)(BRICK*m->tile_w), 0, m->tile_w, m->tile_h};
	image = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
								0, 0, 0, 0);
	SDL_BlitSurface(m->tileset, &dest, image, &pos);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x5f75ca);
}

Brick::Brick(int _x, int _y, Map* m):
	Object(_x, _y, m), y0(_y),
	destroying(false)
{
	wall = true;
	type = BRICK;
	debris = SDL_LoadBMP("data/graphic/items/debris.bmp");
	SDL_SetColorKey(debris, SDL_SRCCOLORKEY, 0x0000ff);
}

void Brick::destroy()
{
    destroying = true;
    destroyed = true;
    //wall = false;
}

void Brick::update()
{
    Item* item_to_delete(NULL);
    if (content)
    {
        if (content->discovered)
        {
            if (content->type == "coin")
            {
                content->update();
                item_to_delete = content;
                if (item_to_delete->in_initial_position())
                {
                    m_map->text_list.push_back(new Text((int)x, (int)y, content->score, m_map->camera));
                    content = content->next;
                    delete item_to_delete;
                    item_to_delete = NULL;
                }
            }
            else
            {
                type = UNBREAKABLE;
                content = NULL;
            }
        }
    }
    else if (!none)
        y_vel = 0;
	//Rien à modifier pour l'image...
	//Physique
	y_vel += BRICK_GRAVITY;
	y += y_vel;
	if (y >= y0)
	{
		y_vel = 0;
		y = y0;
	}
}
void Brick::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this),
            dest = {(Sint16)(3*m_map->tile_w), 0, m_map->tile_w, m_map->tile_h};
    if (content)
    {
        content->show(screen);
        SDL_BlitSurface(image, NULL, screen, &pos);
    }
    else
    {
        if (!none)
            SDL_BlitSurface(m_map->tileset, &dest, screen, &pos);
        else
            SDL_BlitSurface(image, NULL, screen, &pos);
    }
}
/////////////////////////////////////////////

/////////////////[Brique incassable]/////////////////
SDL_Surface* Unbreakable::image = NULL;

void Unbreakable::load_image(Map* m)
{
	SDL_Rect dest = {0, (Sint16)m->tile_h, m->tile_w, m->tile_h},
			  pos = {0, 0};
	image = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
								0, 0, 0, 0);
	SDL_BlitSurface(m->tileset, &dest, image, &pos);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x5f75ca);
}

Unbreakable::Unbreakable(int _x, int _y, Map* m):
	Object(_x, _y, m)
{
    wall = true;
    type = UNBREAKABLE;
}

void Unbreakable::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this);
	SDL_BlitSurface(image, NULL, screen, &pos);
}
/////////////////////////////////////////////////////

/////////////////[Sol]/////////////////
SDL_Surface* Ground_brick::image = NULL;

void Ground_brick::load_image(Map* m)
{
	SDL_Rect dest = {0, 0, m->tile_w, m->tile_h},
			  pos = {0, 0};
	image = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
								0, 0, 0, 0);
	SDL_BlitSurface(m->tileset, &dest, image, &pos);
	SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x5f75ca);
}

Ground_brick::Ground_brick(int _x, int _y, Map* m):
	Object(_x, _y, m)
{
    wall = true;
}

void Ground_brick::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this);
	SDL_BlitSurface(image, NULL, screen, &pos);
}
////////////////////////////////////////

/////////////////[Tube]/////////////////
SDL_Surface* Pipe::images[4] = {NULL};

void Pipe::load_image(Map* m)
{
    SDL_Rect pos = {0, 0},
            dest = {(Sint16)((PIPE_0%m->tile_x)*m->tile_w), (Sint16)((PIPE_0/m->tile_x)*m->tile_h),
                    m->tile_w, m->tile_h};
    images[0] = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
                                    0, 0, 0, 0);
    SDL_BlitSurface(m->tileset, &dest, images[0], &pos);

    dest.x = (Sint16)((PIPE_1%m->tile_x)*m->tile_w);
    dest.y = (Sint16)((PIPE_1/m->tile_x)*m->tile_h);
    images[1] = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
                                    0, 0, 0, 0);
    SDL_BlitSurface(m->tileset, &dest, images[1], &pos);

    dest.x = (Sint16)((PIPE_2%m->tile_x)*m->tile_w);
    dest.y = (Sint16)((PIPE_2/m->tile_x)*m->tile_h);
    images[2] = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
                                    0, 0, 0, 0);
    SDL_BlitSurface(m->tileset, &dest, images[2], &pos);

    dest.x = (Sint16)((PIPE_3%m->tile_x)*m->tile_w);
    dest.y = (Sint16)((PIPE_3/m->tile_x)*m->tile_h);
    images[3] = SDL_CreateRGBSurface(SDL_HWSURFACE, m->tile_w, m->tile_h, 32,
                                    0, 0, 0, 0);
    SDL_BlitSurface(m->tileset, &dest, images[3], &pos);

    for (int i=0; i<4; ++i)
        SDL_SetColorKey(images[i], SDL_SRCCOLORKEY, 0x5f75ca);
}

Pipe::Pipe(int _x, int _y, Map* m, int part):
    Object(_x, _y, m)
{
    wall = true;
    type = PIPE_0;
    if (part == PIPE_0) m_part = 0;
    if (part == PIPE_1) m_part = 1;
    if (part == PIPE_2) m_part = 2;
    if (part == PIPE_3) m_part = 3;
}

void Pipe::show(SDL_Surface* screen)
{
    SDL_Rect pos = m_map->camera->apply_to(this);
	SDL_BlitSurface(images[m_part], NULL, screen, &pos);
}
//////////////////////////////////////////////////////

Debris::Debris(int _x, int _y, Map* m):
    m_map(m), gravity(0.4)
{
    image = SDL_LoadBMP("data/graphic/items/debris.bmp");
    if (!image)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, 0x0000ff);
    for (int i=0; i<4; ++i)
    {
        if (!(i%2)) x_vel[i] = -2;
        else x_vel[i] = 2;
        if (i<2) y_vel[i] = -8;
        else y_vel[i] = 0;
        x[i] = _x+m_map->tile_w*0.5 - 8;
        y[i] = _y+m_map->tile_h*0.5 - 8;
    }
}

Debris::~Debris()
{
    SDL_FreeSurface(image);
}

bool Debris::is_out()
{
    for (int i=0; i<4; ++i)
        if (y[i] <= HEIGHT)
            return false;
    return true;
}

void Debris::show(SDL_Surface* screen)
{
    SDL_Rect pos, dest;
    for (int i=0; i<4; ++i)
    {
        pos.x = (Sint16)(x[i]-m_map->camera->x0);
        pos.y = (Sint16)(y[i]-m_map->camera->y0);
        dest.x = i%2;
        dest.y = i/2;
        dest.w = 16;
        dest.h = 16;
        SDL_BlitSurface(image, &dest, screen, &pos);
    }
}

void Debris::update()
{
    for (int i=0; i<4; ++i)
    {
        x[i] += x_vel[i];
        y[i] += y_vel[i];
        y_vel[i] += gravity;
    }
}
