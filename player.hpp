#ifndef PLAYER_HPP
#define PLAYER_HPP
/*
            _ Mario
    Player_/         _ Goomba
           \_ Enemy_/
                    \_ Koopa
*/
#include <SDL.h>
#include <cmath>
#include "time.hpp"
#include "obstacle.hpp"
#include "map.hpp"
#include "const.hpp"
#include "items.hpp"
#include "entity.hpp"
#include "flag.hpp"

int sgn(double x);

class Item;
class Fireball;
class Object;
class Flag;
class Map;

///Classe de base pour les personnages du jeu
class Player: public Base
{
public:
    enum{ left = 1, right = 0 };
    Player();
    virtual ~Player();

    ///Detection des collisions
    bool collide_with(Object* objet);
    bool collide_with(Player* player);
    bool collide_with(Item* item);
    bool collide_with(Flag* flag);

    ///Mettre-à-jour à chaque frame
    virtual void update();

    ///Affiche le personnage
    virtual void show(SDL_Surface* screen);

    ///Initialise les données du personnage
    ///@param m La carte à laquelle il appartient
    ///@param (x0, y0) sa position initiale
    void setup(Map* m, int x0, int y0);

    ///Essayer le déplacement
    ///Si le déplacement peut être fait, il est immédiatement exécuté
    ///@return false si le déplacement a échoué sinon true
    virtual bool try_move(int x_offset, int y_offset) = 0;

    ///Corrige le vécteur déplacement passé en paramètre
    void affine(int x_offset, int y_offset);

    void move(int x_offset, int y_offset);

    ///Simuler l'éffet de la force de gravité
    void gravity_effect();

    virtual void die();

    Map* m_map;

    ///Vitesse du personnage
    float x_vel, y_vel;

    ///Diréction du personnage
    int direction;

    ///Objet pour réguler l'animation
    Clock timer;

    std::string type;

    int state;
    int status;
    int score;
protected:
    float gravity;
};

///Classe qui représente notre personnage principale ,c'est-à-dire Mario
class Mario: public Player
{
public:
    Mario (Map* m);
    ~Mario();

    void shot();
    void init_position();
    int velocity_sgn();
    bool try_move(int x_offset, int y_offset);

    void add_hp();

    ///@see Player::show
    virtual void show(SDL_Surface* screen);

    ///@see Player::update
    void update();

    int coin;
    ///Indique quelle image afficher
    int state;
    bool respawning;
    bool arrived;
    bool *pause;
    float x_acc;
    int get_hp();
    SDL_Surface* get_image();
    void set_invincible();
    void grow_up();
    void shrink();
    void die();
    void allow_fire();
    void rewind();
    void setup_keys(bool *KEYS);
    void set_end();
    bool is_big();
    bool is_dead();
    bool is_invincible();
    bool *adress();
private:
    enum
    {
        STOP, WALK_1, WALK_2, WALK_3, JUMP, DRIFT, TRANSITION,
        LITTLE = 0, BIG, POWER, SMALL_TRANSITION, BIG_TRANSITION
    };
    float initial_h;
    bool flag_down;
    bool dead;
    bool dying;
    bool big;
    bool fire;
    bool allow_jump;
    bool growing;
    bool becoming_small;
    bool having_fire;
    bool end;
    bool *keys;
    int transformation_h;
    int having_fire_state, invincible_state;
    int cnt_to_fire, cnt_to_big, cnt_to_small;
    Clock invincible_timer, transformation_timer, respawn_timer, regulateur, death_timer, timer_for_next_fireball;
    bool invincible;
    std::vector<Fireball*> fireball_list;
    SDL_Surface *image[5][2][6], *transition[2];
    SDL_Surface *current_image, *death;
    SDL_Surface *small_end[2], *big_end[2], *powered_end[2];
    int cur_ind;
    unsigned int life;

    ///Cette méthode permettra au personnage de sauter
    void jump();
    ///Charge les images de mario
    void load_image();
    void setup_boolean();

    void update_size();
    void update_frame();
    void check_invincibility_timer();
    void check_boundaries();
    void update_fireball_list();
    void update_direction();
    void handle_keys();
    void handle_y_vel();
    void finish();
    SDL_Surface* image_to_show();

    void transition_to_big();
    void transition_to_small();
    void transition_to_fire();
    void dying_animation();
    void respawn();
};

///Classe de base pour tous les enemies dans le jeu
class Enemy: public Player
{
public:
    Enemy(int x0, int y0, Map* m);
    virtual ~Enemy();
    void die();

    virtual void jumped();
    virtual void show(SDL_Surface* screen);
    virtual void update();

    ///écraser ou toujours en vie
    bool squished;
    bool try_move(int x_offset, int y_offset);

    Clock delete_timer;

    virtual void set_status(int);
    bool status_is(int);

    bool kicked;

};
#endif // PLAYER_HPP
