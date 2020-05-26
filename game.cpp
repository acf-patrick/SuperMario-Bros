#include "game.hpp"
#include "enemies.hpp"
#include <sstream>

Game::Game(SDL_Surface* screen):
    Animation(screen), paused(false),
    aborting(false), one_up(true)
{
    TTF_Init();
    super_mario_bros = SDL_LoadBMP("data/graphic/super_mario_bros.bmp");
    if (!super_mario_bros)
    {
        std::cerr << SDL_GetError();
        exit(1);
    }
    SDL_SetColorKey(super_mario_bros, SDL_SRCCOLORKEY, 0x5c94fc);
    m_map = new Map("data/matrix", "data/config", &one_up);
    player = new Mario(m_map);
    m_map->score = &(player->score);
    player->init_position();
    player->setup_keys(keys);
    player->pause = &paused;

    sound = new Sound();
    m_map->game_sound = sound;
    show_fps = false;
    fps = new FPS(WIDTH-150, HEIGHT-20, "");
    text.push_back(new Score(&(player->score)));
    text.push_back(new Indicator());
    text.push_back(new Coin_counter(&(player->coin)));
}

Sound* Game::get_sound() { return sound; }

void Game::menu()
{
    bool persist(true);
    SDL_Rect pos = {(Sint16)((WIDTH - super_mario_bros->w)/2), 70};
    while (persist)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT)
        {
            running = false;
            return;
        }
        else if (event.type == SDL_KEYDOWN)
            persist = false;
        show();
        SDL_BlitSurface(super_mario_bros, NULL, screen, &pos);
        SDL_Flip(screen);
        text[2]->update();
    }
}

bool Game::run()
{
    fps->start();
    timer = new Timer();
    timer->score = &(player->score);

    menu();
    transition(false, true);
    sound->play("world1_1");

    while (running)
    {
        while (SDL_PollEvent(&event))
            handle_events();

        update();
        m_map->center_camera(player);
        show();
        SDL_Flip(screen);

        if (timer->out_of_time())
            sound->play("world1_1 speedup");
        SDL_Delay(15);
        if (aborting)
        {
            running = false;
            return true;
        }
    }
    return false;
}

void Game::handle_events()
{
    //Enemy* enemy(NULL);
    switch (event.type)
    {
    case SDL_QUIT:
        running = false;
        break;
    case SDL_MOUSEBUTTONUP:
        /*std::cout << (int)(event.button.x + m_map->camera->x0)/m_map->tile_w
                  <<", "
                  << (int)(event.button.y + m_map->camera->y0)/m_map->tile_h <<std::endl;
        if (event.button.button == SDL_BUTTON_LEFT)
            enemy = new Goomba(event.button.x + m_map->camera->x0, event.button.y + m_map->camera->y0, m_map);
        if (event.button.button == SDL_BUTTON_RIGHT)
            enemy = new Koopa(event.button.x + m_map->camera->x0, event.button.y + m_map->camera->y0, m_map);
        if (enemy)
            m_map->enemy_list.push_back(enemy);*/
        break;
    case SDL_KEYDOWN:
        keys[event.key.keysym.sym] = true;
        switch (event.key.keysym.sym)
        {
            case SDLK_a:
                player->shot();
                break;
            case SDLK_ESCAPE:
                aborting = true;
                break;
            case SDLK_RETURN:
                show_fps = !show_fps;
                break;
            case SDLK_SPACE:
                paused = !paused;
                SDL_SaveBMP(screen, "screenshot.bmp");
                break;
            case SDLK_SEMICOLON:
                sound->mute();
                break;
            default: ;
        }
        break;
    case SDL_KEYUP:
        keys[event.key.keysym.sym] = false;
        break;
    }
}

void Game::show()
{
    SDL_FillRect(screen, NULL, 0x5c94fc);

    m_map->show(screen);
    player->show(screen);

    if (show_fps) fps->show(screen);
    timer->show(screen);
    for (int i=0; i<(int)text.size(); ++i) text[i]->show(screen);
}

void Game::update()
{
    if (!paused)
    {
        m_map->update();
        timer->update();
    }
    fps->update();
    for (int i=0; i<(int)text.size(); ++i) text[i]->update();
    player->update();
    if (player->is_dead())
    {
        if (!player->get_hp())
        {
            aborting = true;
            transition(true);
            return;
        }
        timer->restart();
        player->rewind();
        reload_map();
        player->m_map = m_map;
        transition(false);
        player->coin = 0;
        sound->play("world1_1");
    }
    if (timer->over())
        player->die();
    if (player->arrived)
    {
        m_map->rise_flag();
        if (timer->countdown())
        {
            if (countdown_sound_timer.as_milliseconds() >= 25)
            {
                countdown_sound_timer.restart();
                sound->play("countdown");
            }
            player->score += 1;
        }
        else
        {
            SDL_Delay(1000);
            aborting = true;
            transition(true);
        }
    }
}

void Game::transition(bool gameover, bool loading)
{
    SDL_Rect pos = {250, 200};
    std::stringstream str;
    str << " x 0" << player->get_hp();

    SDL_FillRect(screen, NULL, 0x0);
    for (int i=0; i<(int)text.size(); ++i)
        text[i]->show(screen);
    timer->show(screen);
    if (!gameover or loading)
    {
        Text t(pos.x + 32, pos.y, str.str(), NULL, "font.ttf", 25);
        SDL_BlitSurface(player->get_image(), NULL, screen, &pos);
        t.show(screen);
    }
    else
    {
        sound->play("game_over");
        Text t(pos.x, pos.y, "GAME OVER", NULL, "font.ttf", 25);
        t.show(screen);
    }
    SDL_Flip(screen);
    SDL_Delay(3500);
}

void Game::reload_map()
{
    delete m_map;
    m_map = new Map("data/matrix", "data/config", &one_up);
    m_map->score = &(player->score);
}

Game::~Game()
{
    delete fps;
    //delete timer;
    //for (int i=0; i<(int)text.size(); ++i) delete text[i];
    delete player;
    delete m_map;
    delete sound;
    SDL_FreeSurface(super_mario_bros);
    TTF_Quit();
}
