#ifndef GAME_H
#define GAME_H

#include <list>
#include "SDL.h"
#include "Object.h"

class Game
{
private:
    Uint32 frameTime = 1000 / 60;
    int windowWidth = 375;
    int windowHeight = 667;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    Player player;
    PlayerProjectile playerProjectileTemplate;
    std::list<PlayerProjectile *> playerProjectiles;

public:
    Game(/* args */);
    ~Game();
    void init();
    void run();
    void keyboardControll(float deltaTime);
    void clean();
};

#endif