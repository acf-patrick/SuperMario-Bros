#include "game.hpp"
#include <SDL.h>
#include <ctime>
bool logo_shown(true);

int main(int argc, char** argv)
{
    srand(time(NULL));
    SDL_putenv("SDL_VIDEO_CENTERED=1");
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Surface* screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_DOUBLEBUF|SDL_HWSURFACE);
    SDL_WM_SetCaption("Super Mario Bros", NULL);
    if (!screen)
    {
        std::cerr << "Erreur lors de l'ouverture de la fenêtre principale!";
        exit(1);
    }
    Game* game = new Game(screen);
    if (logo_shown)
    {
        logo_shown = false;
        Show_logo(screen, game->get_sound()).run();
    }
    if (game->run())
    {
        delete game;
        return main(argc, argv);
    }
    delete game;

    SDL_Quit();
    return EXIT_SUCCESS;
}
