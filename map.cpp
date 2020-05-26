#include "map.hpp"
#include "enemies.hpp"
#include <algorithm>

Map::Map(const std::string& carte_path, const std::string& config_path, bool* add_one_up)
{
    one_up = add_one_up;
    show_flag = false;
    camera = new Camera(0, FIXED_CAMERA, WIDTH, HEIGHT);
    //Demarrer le timer pour tous les blocs quéstions
    Question_mark::timer.restart();
	std::ifstream file(config_path.c_str());
    std::string buffer;

    file >> buffer;
    file >> buffer;

    tileset = SDL_LoadBMP(buffer.c_str());
    if (!tileset)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }

    file >> buffer;
    file >> tile_x;
    file >> tile_y;

    file >> buffer;
    file >> world_x;
    file >> world_y;

    file >> buffer;

    file.close();
    file.open(carte_path.c_str());

    tile_w = tileset->w/tile_x;
    tile_h = tileset->h/tile_y;

    group.resize(world_y);
    m.resize(world_y);
    for (int i=0; i<world_y; ++i)
    {
        group[i].resize(world_x, NULL);
        m[i].resize(world_x);
    }
    for (int i=0; i<world_y; ++i)
    {
        for (int j=0; j<world_x; ++j)
        {
            int nbr(0);
            int x(j*tile_w), y(i*tile_h);
            file >> nbr;
            if (nbr>=tile_x*tile_y)
                throw std::string("Index out of range!");
            m[i][j] = nbr;
            if (nbr == PIPE_0 or nbr == PIPE_1 or nbr == PIPE_2 or nbr == PIPE_3)
                group[i][j] = new Pipe(x, y, this, nbr);
            switch (nbr)
            {
            case QUESTION:
                group[i][j] = new Question_mark(x, y, this);
                break;
            case BRICK:
                group[i][j] = new Brick(x, y, this);
                break;
            case UNBREAKABLE:
                group[i][j] = new Unbreakable(x, y, this);
                break;
            case GROUND_BRICK:
                group[i][j] = new Ground_brick(x, y, this);
                break;
            default: ;
            }
        }
    }
    _flag = new Flag(this);
    load_items();
    load_enemies();

    file.close();
    /**Loading all images*/

    Question_mark::load_image(this);
    Brick::load_image(this);
    Unbreakable::load_image(this);
    Ground_brick::load_image(this);
    Pipe::load_image(this);

    castle_flag = SDL_LoadBMP("data/graphic/items/castle_flag.bmp");
    SDL_SetColorKey(castle_flag, SDL_SRCCOLORKEY, 0x5c94fc);
    castle_flag_pos.x = 203*tile_w;
    castle_flag_pos.y = 7*tile_h;

    if (!(*add_one_up))
    {
        delete group[7][63];
        group[7][63] = NULL;
        m[7][63] = BLANK;
    }
}

Map::~Map()
{
    int i, j;
    for (i=0; i<(int)group.size(); ++i)
        for (j=0; j<(int)group[i].size(); ++j)
            delete group[i][j];
    for (i=0; i<(int)enemy_list.size(); ++i)
        delete enemy_list[i];
    delete _flag;

    SDL_FreeSurface(tileset);
    SDL_FreeSurface(castle_flag);
}

void Map::show(SDL_Surface* screen)
{
    int i, j;
    Enemy* enemy(NULL);

    SDL_Rect flag_pos = {(Sint16)(castle_flag_pos.x - camera->x0), (Sint16)(castle_flag_pos.y - camera->y0)},
             dest, pos;
    dest.w = tile_w;
    dest.h = tile_h;

    int x_min = camera->y0/tile_h,
        x_max = (camera->y0+HEIGHT)/tile_h,
        y_min = camera->x0/tile_w,
        y_max = (camera->x0+WIDTH)/tile_w;

    //Render background objects
    if (show_flag)
        SDL_BlitSurface(castle_flag, NULL, screen, &flag_pos);

    for (i=x_min; i<=x_max; ++i)
        for (j=y_min; j<=y_max; ++j)
        {
            pos.x = j*tile_w-camera->x0;
            pos.y = i*tile_h-camera->y0;
            dest.x = (Sint16)(m[i][j]%tile_x)*tile_w;
            dest.y = (Sint16)(m[i][j]/tile_x)*tile_h;

            if (i<world_y and j<world_x and i>=0 and j>=0)
                if (!group[i][j] and m[i][j] != BLANK)
                    //On affiche les objets qui n'interragissent pas
                    //Objets de fond
                    SDL_BlitSurface(tileset, &dest, screen, &pos);
        }

    //Render item objects
    for (int i=0; i<(int)item_list.size(); ++i)
        item_list[i]->show(screen);

    //Render blocs
    for (i=x_min; i<=x_max; ++i)
        for (j=y_min; j<=y_max; ++j)
            if (i<world_y and j<world_x and i>=0 and j>=0)
                if (group[i][j])
                    if (!group[i][j]->is_destroyed())
                    {
                        if (!group[i][j]->bumped and i==7 and j==63)
                            continue;
                        else
                            group[i][j]->show(screen);
                    }

    for (i=0; i<(int)enemy_list.size(); ++i)
    {
        enemy = enemy_list[i];
        if (camera->x0-WIDTH*0.5<(enemy->x+enemy->width*0.5) && (enemy->x+enemy->width*0.5)<camera->x0+WIDTH*3.5)
            enemy->show(screen);
    }
    for (i=0; i<(int)brick_debris.size(); ++i)
        if (brick_debris[i])
            brick_debris[i]->show(screen);
     _flag->show(screen);
    for (i=0; i<(int)text_list.size(); ++i)
        if (text_list[i])
            text_list[i]->show(screen);
}

void Map::update()
{
    int i, j;
    Question_mark::update_state();
    _flag->update();

    Enemy* enemy(NULL);

    for (i=0; i<(int)item_list.size(); ++i)
        item_list[i]->update();
    for (i=0; i<(int)brick_debris.size(); ++i)
        if (brick_debris[i])
        {
            brick_debris[i]->update();
            if (brick_debris[i]->is_out())
            {
                delete brick_debris[i];
                brick_debris[i] = NULL;
            }
        }
    for (i=0; i<(int)text_list.size(); ++i)
    {
        if (text_list[i])
        {
            text_list[i]->update();
            if (text_list[i]->is_to_remove())
            {
                delete text_list[i];
                text_list[i] = NULL;
            }
        }
    }

    int x_min = camera->y0/tile_h,
        x_max = (camera->y0+HEIGHT)/tile_h,
        y_min = camera->x0/tile_w,
        y_max = (camera->x0+WIDTH)/tile_w;

    for (i=x_min; i<=x_max; ++i)
        for (j=y_min; j<=y_max; ++j)
            if (i<world_y and j<world_x and i>=0 and j>=0)
                if (group[i][j])
                {
                    if (group[i][j]->is_destroyed())
                    {
                        delete group[i][j];
                        group[i][j] = NULL;
                        m[i][j] = BLANK;
                    }
                    else
                        group[i][j]->update();
                }

    for (i=0; i<(int)enemy_list.size(); ++i)
    {
        enemy = enemy_list[i];
        if (camera->x0-WIDTH/4.<enemy->x+enemy->width && enemy->x<camera->x0+WIDTH*(9/8.))
            enemy->update();
        if ((enemy->squished and enemy->delete_timer.as_second() >= 5) or
                (enemy->kicked and enemy->y >= camera->y0+HEIGHT))
        {
            enemy_list.erase(std::remove(enemy_list.begin(), enemy_list.end(), enemy), enemy_list.end());
            delete enemy;
            enemy = NULL;
        }
    }
    if (show_flag)
        if (castle_flag_pos.y > 217)
            castle_flag_pos.y -= 1;
}

void Map::rise_flag(){ show_flag = true; }

void Map::move(int x_offset, int y_offset)
{
    camera->move(x_offset, y_offset);
    camera->adjust(world_x*tile_w, world_y*tile_h);
}

void Map::center_camera(Player* perso)
{
    if ((perso->x + (perso->width-WIDTH)/2 - camera->x0) > 0)
        camera->center_on(perso);
    camera->adjust(world_x*tile_w, world_y*tile_h);
}

bool Map::collision(Item* mushroom)
{
    Object* object(NULL);
    int x_min = mushroom->x/tile_w,
        x_max = (mushroom->x+mushroom->width)/tile_w,
        y_min = mushroom->y/tile_h,
        y_max = (mushroom->y+mushroom->height)/tile_h;
    if (x_min<0 or y_min<0 or x_max>=world_x or y_max>=world_y)
        return true;
    for (int j=x_min; j<=x_max; ++j)
        for (int i=y_min; i<=y_max; ++i)
        {
            object = group[i][j];
            if (object)
                if (mushroom->collide_with(object) and object->wall)
                {
                    if (object->y_vel < 0)
                        mushroom->y_vel = -8;
                    return true;
                }
        }
    for (int i=0; i<(int)enemy_list.size(); ++i)
    {
        //Enfaite "mushroom" est ici consideré comme une boule de feu
        if (not enemy_list[i]->kicked and
             not enemy_list[i]->squished and
              enemy_list[i]->collide_with(mushroom) and
                mushroom->type == "fire")
        {
            mushroom->bumped = true;
            enemy_list[i]->die();
            text_list.push_back(new Text((int)enemy_list[i]->x, (int)enemy_list[i]->y, enemy_list[i]->score, camera));
            (*score) += enemy_list[i]->score;
            game_sound->play("shot");
            return true;
        }
    }
    return false;
}
int Map::collision(Mario* player, int flag)
{
    Enemy* enemy(NULL);
    Object* object(NULL);
    Item* item(NULL);
    int x_min = player->x/tile_w,
        x_max = (player->x+player->width)/tile_w,
        y_min = player->y/tile_h,
        y_max = (player->y+player->height)/tile_h;
    if (x_min<0 or y_min<0)
        return 0;
    if (x_max>=world_x or y_max>=world_y)
        return 1;

    //Checking interaction with blocs and items
    for (int j=x_min; j<=x_max ; ++j)
        for (int i=y_min; i<=y_max ; ++i)
        {
            object = group[i][j];
            if (object)
                if (player->collide_with(object) and object->wall)
                {
                    {
                        if(flag == UP and
                            object->type != PIPE_0 and
                            object->y_vel >= 0)
                        {
                            object->y_vel = BLOC_BUMPED;
                            object->bumped = true;
                            if (object->content)
                            {
                                if (!object->content->discovered)
                                {
                                    if (object->content->type == "coin")
                                    {
                                        object->content->y_vel = -17;
                                        object->content->timer.restart();
                                        game_sound->play("coin");
                                        //text_list.push_back(new Text(j*tile_w, i*tile_h, object->content->score));
                                        (*score) += object->content->score;
                                        player->coin++;
                                    }
                                    else
                                    {
                                        if (object->content->type == "power_up")
                                        {
                                            if (player->is_big())
                                                object->content = new Flower((int)object->content->x, (int)object->content->y, this);
                                            else
                                                object->content = new Mushroom((int)object->content->x, (int)object->content->y, this);
                                        }
                                        item_list.push_back(object->content);
                                        game_sound->play("mushroom");
                                    }
                                    object->content->discovered = true;
                                }
                                else
                                    game_sound->play("bloc_bumped");
                            }
                            else
                            {
                                if (player->is_big() and object->type == BRICK and object->none)
                                {
                                    brick_debris.push_back(new Debris((int)object->x ,(int)object->y, this));
                                    object->destroy();
                                    game_sound->play("brick_broken");
                                }
                                else
                                {
                                    if (object->type != UNBREAKABLE)
                                        game_sound->play("bloc_bumped");
                                }
                            }
                        }
                    }
                    return 1;
                }
        }
    for (int i=0; i<(int)enemy_list.size(); ++i)
    {
        enemy = enemy_list[i];
        if (!(enemy->squished or enemy->kicked ))
            if (!player->respawning and player->collide_with(enemy))
            {
                if (player->is_invincible())
                {
                    enemy->die();
                    text_list.push_back(new Text((int)enemy->x, (int)enemy->y, enemy->score, camera));
                    (*score) += enemy->score;
                    game_sound->play("shot");
                }
                else
                {
                    if (flag == DOWN and player->y_vel > 0)
                    {
                        if (enemy->type == "koopa")
                        {
                            if (!enemy->status_is(STAY_IN_SHELL))
                            {
                                enemy->x_vel = 0;
                                enemy->set_status(STAY_IN_SHELL);
                            }
                        }
                        else
                        {
                            enemy->squished = true;
                            text_list.push_back(new Text((int)enemy->x, (int)enemy->y, enemy->score, camera));
                            (*score) += enemy->score;
                            enemy->delete_timer.restart();
                        }
                        game_sound->play("stomp");
                    }
                    if (player->y_vel == 0)
                    {
                        if (enemy->status_is(STAY_IN_SHELL))
                        {
                            enemy->set_status(MOVING_IN_SHELL);
                            enemy->x_vel = sgn(player->x_vel)*SHELL_SPEED;
                        }
                        else
                            if (player->is_big())
                            {
                                player->shrink();
                                game_sound->play("pipe");
                            }
                            else
                                player->die();
                    }
                    return -1;
                }
            }
    }
    for (int i=0; i<(int)item_list.size(); ++i)
    {
        if (item_list[i]->collide_with(player))
        {
            if (item_list[i]->type == "1_up")
            {
                (*one_up) = false;
                player->add_hp();
                game_sound->play("one_up");
                text_list.push_back(new Text((int)item_list[i]->x, (int)item_list[i]->y, "1 UP", camera,  "emulogic.ttf", 10));
            }
            else
            {
                if (item_list[i]->type == "star")
                {
                    player->set_invincible();
                    game_sound->play("invincible");
                }
                else if (item_list[i]->type == "power_up")
                {
                    if (player->is_big())
                        player->allow_fire();
                    else
                        player->grow_up();
                    game_sound->play("power_up");
                }
                text_list.push_back(new Text((int)item_list[i]->x, (int)item_list[i]->y, item_list[i]->score, camera));
                (*score) += item_list[i]->score;
            }
            item = item_list[i];
            item_list.erase(std::remove(item_list.begin(), item_list.end(), item_list[i]), item_list.end());
            delete item;
            //item_list = remove_from(item_list, i);
        }
    }
    if (player->collide_with(_flag))
    {
        _flag->player_flag_boolean = player->adress();
        player->set_end();
        _flag->slide_down();
    }
    return 0;
}
int Map::collision(Enemy* player)
{
    Enemy* enemy(NULL);
    Object* object(NULL);
    int x_min = player->x/tile_w,
        x_max = (player->x+player->width)/tile_w,
        y_min = player->y/tile_h,
        y_max = (player->y+player->height)/tile_h;
    if (x_min<0 or y_min<0 or x_max>=world_x or y_max>=world_y)
        return 1;

    //Checking interaction with blocs and items
    for (int j=x_min; j<=x_max; ++j)
        for (int i=y_min; i<=y_max; ++i)
        {
            object = group[i][j];
            if (object)
                if (player->collide_with(object) and object->wall)
                {
                    if (player->kicked)
                        return 0;
                    if (object->y_vel < 0)
                    {
                        player->die();
                        text_list.push_back(new Text((int)player->x, (int)player->y, player->score, camera));
                        (*score) += player->score;
                        game_sound->play("shot");
                    }
                    //Koopa heurte quelque chose
                    else if(player->type == "koopa")
                        if (player->status != UNDEFINED and !player->y_vel)
                            game_sound->play("bloc_bumped");
                    return 1;
                }
        }

    //Checking interaction between/with enemies
    for (int i=0; i<(int)enemy_list.size(); ++i)
    {
        enemy = enemy_list[i];
        if (!(enemy->squished or enemy->kicked or enemy == player or player->kicked))
            if (player->collide_with(enemy))
            {
                if (player->type == "koopa" and player->status == MOVING_IN_SHELL)
                {
                    enemy->die();
                    text_list.push_back(new Text((int)enemy->x, (int)enemy->y, enemy->score, camera));
                    (*score) += enemy->score;
                    game_sound->play("shot");
                    return 0;
                }
                return -1;
            }
    }
    return 0;
}

SDL_Surface* Map::get_image(SDL_Rect rect)
{
    SDL_Surface* ret = SDL_CreateRGBSurface(SDL_HWSURFACE, tile_w, tile_h, 32,
                                            0, 0, 0, 0);
    SDL_Rect pos = {0, 0};
    SDL_BlitSurface(tileset, &rect, ret, &pos);
    return ret;
}

void Map::load_items()
{
    //Adding coins
    group[8][93]->set_content(six_coins(93*tile_w, 8*tile_h, this));
    //Adding Powerups
    group[8][21]->set_content(new Powerup(21*tile_w, 8*tile_h, this));
    group[7][63]->set_content(new Life(63*tile_w, 7*tile_h, this));

    group[8][77]->set_content(new Powerup(77*tile_w, 8*tile_h, this));
    group[8][100]->set_content(new Star(100*tile_w, 8*tile_h, this));
    group[4][108]->set_content(new Powerup(108*tile_w, 4*tile_h, this));
}

void Map::load_enemies()
{
    enemy_list.push_back(new Goomba(800 , 374, this));
    enemy_list.push_back(new Goomba(1540, 374, this));
    enemy_list.push_back(new Goomba(1715, 374, this));
    enemy_list.push_back(new Goomba(1788, 374, this));
    enemy_list.push_back(new Goomba(2855, 48 , this));
    enemy_list.push_back(new Goomba(2902, 48 , this));
    enemy_list.push_back(new Goomba(3395, 374, this));
    enemy_list.push_back(new Goomba(3429, 374, this));
    enemy_list.push_back(new  Koopa(3679, 374, this));
    enemy_list.push_back(new Goomba(4186, 374, this));
    enemy_list.push_back(new Goomba(4238, 374, this));
    enemy_list.push_back(new Goomba(4555, 374, this));
    enemy_list.push_back(new Goomba(4590, 374, this));
    enemy_list.push_back(new Goomba(5834, 374, this));
    enemy_list.push_back(new Goomba(5895, 374, this));
}
