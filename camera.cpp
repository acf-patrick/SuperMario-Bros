#include "camera.hpp"

Camera::Camera(int _x, int _y, int _width, int _height):
    x0(_x), y0(_y),
    width(_width), height(_height)
{}
Camera::~Camera()
{}
SDL_Rect Camera::apply_to(Base* entity)
{
    SDL_Rect ret = {(Sint16)(entity->x - x0), (Sint16)(entity->y - y0)};
    return ret;
}
void Camera::adjust(int w_limit, int h_limit)
{
    if (x0<0) x0 = 0;
    if (y0<0) y0 = 0;
    if (x0+width > w_limit) x0 = w_limit-width;
    if (y0+height> h_limit) y0 = h_limit-height;
}
void Camera::move(int x_off, int y_off)
{
    x0 += x_off;
    y0 += y_off;
}
void Camera::center_on(Base* entity)
{
    x0 = (int)(entity->x + (entity->width-width)/2.);
}
