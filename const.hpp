#ifndef CONST_HPP
#define CONST_HPP
#include <SDL.h>

const SDL_Color WHITE = {255, 255, 255},
                RED   = {255, 0, 0};

const int WIDTH = 600,
          HEIGHT = 470,
          FIXED_CAMERA = 0,
          PUSH_DOWN = 4,
          MARIO_NEXT_ANIM = 150,
          GOOMBA_NEXT_ANIM = 200,
          BRICK_NEXT_ANIM = 150,
          GOOMBA_SPEED = 2,
          SHELL_SPEED = 7,
          KOOPA_IMPULSE_UP = -3,
          MARIO_MAX_SPEED = 8,
          QUESTION = 24,
          BRICK = 1,
          UNBREAKABLE = 33,
          GROUND_BRICK = 0,
          PIPE_0 = 264,
          PIPE_1 = 265,
          PIPE_2 = 297,
          PIPE_3 = 298,
          BLANK = 29,
          STAY_IN_SHELL = 1,
          MOVING_IN_SHELL = 2,
          UNDEFINED = -1,
          COIN_HEIGHT = -13,
          FIRE_SPEED = 8,
          STAR_VEL = 7,
          UP = -1,
          DOWN = 1,
          TRANSITION_DELAY = 30,
          TRANSITION_DELAY_FIRE = 80;

const float GRAVITY_EFFECT = 0.35,
            JUMP_GRAVITY = 0.105,
            GRAVITY = 1,
            KOOPA_GRAVITY = 0.27,
            MARIO_DECC = 0.98,
            MARIO_ACC = 0.15,
            BLOC_BUMPED = -5,
            BRICK_GRAVITY = 0.8,
            JUMP_IMPULSE = -6,
            DECELERATION = 0.2;

#endif // CONST_HPP
