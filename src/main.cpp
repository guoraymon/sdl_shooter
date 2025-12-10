#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include "Game.h"

int main()
{
    Game game;
    game.init();
    game.run();
    return 0;
}
