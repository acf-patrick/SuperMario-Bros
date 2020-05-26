#ifndef ENTITY
#define ENTITY
#include <vector>
#include <SDL.h>

class Base
{
public:
    Base();
    virtual ~Base();
    virtual void update();
    virtual void show(SDL_Surface* screen);
    float x, y;
    Uint16 width, height;
};


#endif // ENTITY
