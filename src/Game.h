#ifndef GAME_H
#define GAME_H

#include <list>
#include <random>
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

    std::mt19937 randomGenerator;
    std::uniform_real_distribution<float> randomDistribution;

    Enemy enemyTemplate;
    std::list<Enemy *> enemies;
    EnemyProjectile enemyProjectileTemplate;
    std::list<EnemyProjectile *> enemyProjectiles;

    Explosion explosionTemplate;
    std::list<Explosion *> explosions;

    Item itemTemplate;
    std::list<Item *> items;

public:
    Game(/* args */);
    ~Game();
    void init();
    void run();
    void keyboardControll(float deltaTime);
    void clean();
};

#endif