#ifndef ITEMS_HPP
#define ITEMS_HPP

/*
        _ Coin
       /
Item _/             _Flower
      \            / _ Mushroom
       \_ Powerup_/_/
                  \ \_ Fireball
                   \_ Star
*/


#include <iostream>
#include <SDL.h>
#include "map.hpp"
#include "time.hpp"
#include "player.hpp"
#include "obstacle.hpp"
#include "entity.hpp"

class Map;
class Player;
class Object;

class Item: public Base
{
public:
    ///@param position initale
    Item(int x0, int y0, Map* m);
    virtual ~Item();
    int score;

    ///Vitesse
    float x_vel, y_vel;

    int initial_height;

    ///L'item a déjà été révélé
    bool discovered;
    bool eaten;
    bool bumped;

    Map* m_map;

    ///Image à afficher
    SDL_Surface* image;

    std::string type;

    Clock timer;

    ///Pointer to the next item
    Item* next;

    ///Met à jour l'items
    virtual void update();
    virtual void show(SDL_Surface* screen);
    bool collide_with(Object* object);
    bool collide_with(Player* mario);
    bool in_initial_position();

protected:
    bool appear;
};

class Coin: public Item
{
public:
    Coin(int x0, int y0, Map* m);
    void update();
    void show(SDL_Surface* screen);

    Sint16 state;
};
Coin* six_coins(int, int, Map*);

class Powerup: public Item
{
public:
    Powerup(int, int, Map*);
protected:
    void move(int x_offset, int y_offset);
    void affine(int x_offset, int y_offset);
    bool try_move(int x_offset, int y_offset);
};

class Mushroom: public Powerup
{
public:
    Mushroom(int x0, int y0, Map* m);
    void update();
};
class Life: public Mushroom
{
public:
    Life(int x0, int y0, Map* m);
};

class Flower: public Powerup
{
public:
    Flower(int x0, int y0, Map* m);
    void update();
    void show(SDL_Surface* screen);
    Sint16 state;
};

class Star: public Powerup
{
public:
    Star(int x0, int y0, Map* m);
    void update();
    void show(SDL_Surface* screen);
    Sint16 state;
};

class Fireball: public Powerup
{
public:
    Fireball(int x0, int y0, Map* m);
    void update();
    void show(SDL_Surface* screen);
    Sint16 state;
private:
    int frame;
    bool firework;
    SDL_Surface* bump;
};

#endif // ITEMS_HPP
