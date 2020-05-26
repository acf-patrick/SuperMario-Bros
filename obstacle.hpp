#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP
#include <SDL.h>
#include "map.hpp"
#include "items.hpp"
#include "const.hpp"
#include "time.hpp"
#include "entity.hpp"

///Verifie si deux réctangles entre en collision
bool rects_collide(SDL_Rect rect1, SDL_Rect rect2);

class Map;
class Item;

///Classe de base pour les objets de la carte de jeu
class Object: public Base
{
public:
    void set_content(Item* new_content);

    ///Vitesse
	float x_vel, y_vel;

	///Carte à laquelle l'objet appartient
	Map* m_map;

	///Contenu de l'objet:
	///	Champignon, etoile, fleurs, etc...
	Item* content;

	int type;

	bool wall;
	bool none;
	bool bumped;

	Object(int x0, int y0, Map* m);
	virtual ~Object();

	///Met-à-jour l'objet à chaque frame
	virtual void update();

	///Affiche l'objet
	virtual void show(SDL_Surface* screen);

	///Boite englobante de l'objet
	SDL_Rect get_rect();
	virtual void destroy();
	bool is_destroyed();
protected:
    bool destroyed;
};

///Brique avec point d'interrogation
class Question_mark: public Object
{
public:
	Question_mark(int _x, int _y, Map* m);

	///Images à afficher lors de l'animation
	static SDL_Surface* images[3];
	static Clock timer;

	///Hauteur initiale du bloc
	int y0;

	static void load_image(Map* m);
	static void update_state();
	void update();
	void show(SDL_Surface* screen);
private:
	///Pour savoir quelle image afficher à un moment donné
	static int state;
};

///Brique
class Brick: public Object
{
public:
	Brick(int _x, int _y, Map* m);
	int y0;
	static SDL_Surface* image;
	static void load_image(Map* m);

	void update();
	void show(SDL_Surface* screen);
	void destroy();
private:
    bool destroying;
    SDL_Surface* debris;
};


///Brique incassable
class Unbreakable: public Object
{
public:
	Unbreakable(int _x, int _y, Map* m);
	void show(SDL_Surface* screen);
	static SDL_Surface* image;
	static void load_image(Map* m);
};

///Brique représentant le sol
class Ground_brick: public Object
{
public:
	Ground_brick(int _x, int _y, Map* m);
	void show(SDL_Surface* screen);

	static SDL_Surface* image;
	static void load_image(Map* m);
};

///Pipes
///Cette classe ne represente pas le tube en entier mais seulement
///une de ses 4 parties
class Pipe: public Object
{
public:
    Pipe(int _x, int _y, Map* m, int part);
    int m_part; ///indique quelle partie du tube doit être affichée
    void show(SDL_Surface* screen);

    static SDL_Surface* images[4];
    static void load_image(Map* m);
};

class Debris
{
public:
    Debris(int _x, int _y, Map* m);
    ~Debris();
    bool is_out();
    void show(SDL_Surface* screen);
    void update();
private:
    Map *m_map;
    SDL_Surface* image;
    float x[4], y[4];
    float x_vel[4], y_vel[4];
    float gravity;
};

#endif
