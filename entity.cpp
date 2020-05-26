#include "entity.hpp"

Base::Base():
    x(0), y(0),
    width(0), height(0)
{}
Base::~Base()
{}
void Base::update() {}
void Base::show(SDL_Surface* screen) {}
