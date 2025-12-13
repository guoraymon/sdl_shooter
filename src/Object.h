#ifndef OBJECT_H
#define OBJECT_H
#include <SDL.h>

struct Player
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    Uint32 coolDown = 500;
    Uint32 lastShootTime = 0;
    int health = 3;
};

struct PlayerProjectile
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 400;
    int damage = 1;
};

struct Enemy
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 200;
    Uint32 coolDown = 1000;
    Uint32 lastShootTime = 0;
    int health = 2;
};

struct EnemyProjectile
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 400;
    SDL_FPoint direction = {0, 0};
    int damage = 1;
};

struct Explosion
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int frameCount = 0;
    int currentFrame = 0;
    Uint32 frameDuration = 50; // milliseconds per frame
    Uint32 lastFrameTime = 0;
};

struct Item
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 100;
    SDL_FPoint direction = {0, 0};
    int bounceCount = 3;
    enum class ItemType
    {
        Health,
        Shield,
    } type;
};

struct Background
{
    SDL_Texture *texture = nullptr;
    SDL_FPoint position = {0, 0};
    int width = 0;
    int height = 0;
    int speed = 0;
};

#endif