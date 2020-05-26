#ifndef MAP_HPP
#define MAP_HPP
#include <iostream>
#include <SDL.h>
#include <vector>
#include <string>
#include <fstream>
#include "obstacle.hpp"
#include "flag.hpp"
#include "player.hpp"
#include "sound.hpp"
#include "items.hpp"
#include "camera.hpp"
#include "text.hpp"

class Item;
class Object;
class Flag;
class Question_mark;
class Brick;
class Unbreakable;
class Pipe;
class Discovered;
class Debris;
class Player;
class Mario;
class Enemy;
class Sound;

///Classe contenant les objets du jeu et notament le niveau courant
class Map
{
public:
    ///Tailles des tuiles
	Uint16 tile_w, tile_h;

	///Nombre de tuile dans le spritesheet
	int tile_x, tile_y;

    Camera* camera;

	///Nombre de tuile qui compose la carte
	int world_x, world_y;

	///liste des obstacles
	Flag* _flag;
	std::vector< std::vector<Object*> > group;
	std::vector< std::vector<int> > m;
    std::vector< Item* > item_list;

	///Liste des enemies de Mario dans le jeu
    std::vector< Enemy* > enemy_list;
    //std::vector< Enemy* > deleted_enemy_list;

	///image contenant tout les sprites
	SDL_Surface* tileset;

	///@param carte_path indique le fichier vers la carte du niveau
	///@param config_path indique le fichier qui contient les informations sur le niveau
	Map(const std::string& carte_path, const std::string& config_path, bool *add_one_up);
	~Map();

    void rise_flag();
    bool flag_risen();
	///Affiche la map selon (x0, y0) coordonnées de scrolling
	void show(SDL_Surface* screen);

	///Mise-à-jour les objets dans le map
	void update();

	///Verifie et corrige les coordonnées de la caméra
	void clamp_camera_coord();

	//Centre le caméra sur le personnage indiqué
	void center_camera(Player* perso);

	///Bouger la caméra
	void move(int x_offset, int y_offset);

	///Verifie si le personnage passé en paramètre
	///entre en collision avec quelconque objet ou autre personnage
    ///@return -1 ecrase un enemie
    ///@return 0  aucune collision
    ///@return 1  collision
	int collision(Enemy* player);
	int collision(Mario* player, int flag = 0);
    bool collision(Item* mushroom);

    ///Obtenir une copie d'une partie de l'image contenue dans le tileset
    SDL_Surface* get_image(SDL_Rect rect);

    Sound* game_sound;
    std::vector<Debris*> brick_debris;
    std::vector<Text*> text_list;
    int *score;
private:
    bool show_flag;
    bool *one_up;
    SDL_Surface* castle_flag;
    SDL_Rect castle_flag_pos;
    void load_items();
    void load_enemies();
};

#endif
