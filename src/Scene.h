#ifndef SCENE_H
#define SCENE_H

#include "SDL.h"

class Game;

class Scene
{
protected:
    Game &game;

public:
    Scene(Game &game);
    virtual void init() = 0;
    virtual void run(float deltaTime) = 0;
    virtual void clean() = 0;
    virtual void handleEvent(SDL_Event *event) = 0;
    virtual ~Scene() = default;
};

#endif