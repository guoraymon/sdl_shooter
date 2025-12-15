#ifndef SCENE_H
#define SCENE_H

class Game;

class Scene
{
protected:
    Game &game;

public:
    Scene(Game &game);
    virtual void init() {}
    virtual void run() {}
    virtual void clean() {}
};

#endif