#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <SDL.h>
#include "entity.hpp"

class Camera
{
public:
    Camera(int _x, int _y, int _width, int _height);
    ~Camera();
    void adjust(int w_limit, int h_limit);
    void move(int x_off, int y_off);
    void center_on(Base* entity);
    SDL_Rect apply_to(Base* entity);
    int x0, y0;
    int width, height;
};
#endif
