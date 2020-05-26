#ifndef ENEMIES_HPP
#define ENEMIES_HPP
#include "map.hpp"
#include "player.hpp"

class Map;

class Goomba: public Enemy
{
public:
    Goomba(int x0, int y0, Map* m);
    ~Goomba();
    void update();
    void show(SDL_Surface* screen);
private:
    SDL_Surface* image[4];
};
class Koopa: public Enemy
{
public:
    Koopa(int x0, int y0, Map* m);
    ~Koopa();
    void update();
    void show(SDL_Surface* screen);
private:
    SDL_Surface *image[2][2], *shell;
};
#endif // ENEMIES_HPP
