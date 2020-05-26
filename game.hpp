#ifndef GAME_HPP
#define GAME_HPP
#include <iostream>
#include <vector>
#include <SDL.h>
#include "animation.hpp"
#include "map.hpp"
#include "player.hpp"
#include "sound.hpp"
#include "text.hpp"
#include "time.hpp"
#include "camera.hpp"

/*animation.cpp camera.cpp enemies.cpp entity.cpp flag.cpp game.cpp items.cpp map.cpp obstacle.cpp player.cpp sound.cpp text.cpp time.cpp animation.hpp camera.hpp enemies.hpp entity.hpp flag.hpp game.hpp items.hpp map.hpp obstacle.hpp player.hpp sound.hpp text.hpp time.hpp const.hpp*/

///Classe principale
class Game: public Animation
{
public:
    Game(SDL_Surface* screen);

    Sound* get_sound();
    bool run();
    ~Game();
private:
    bool paused;
    bool show_fps;
    bool aborting;
    bool one_up;
    SDL_Surface* super_mario_bros;
    std::vector<Text*> text;
    Timer* timer;
    FPS* fps;
    Map* m_map;
    Mario* player;
    Sound* sound;
    Clock countdown_sound_timer;
    void menu();
    void transition(bool gameover, bool loading = false);
    void handle_events();
    void show();
    void update();
    void reload_map();
};
#endif // GAME_HPP
