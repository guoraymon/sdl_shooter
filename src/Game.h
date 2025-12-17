#ifndef GAME_H
#define GAME_H

#include <list>
#include <map>
#include <random>

#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "Object.h"

class Scene;

class Game
{
private:
    Uint32 frameTime = 1000 / 60;
    int windowWidth = 375;
    int windowHeight = 667;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;

    TTF_Font *titleFont;
    TTF_Font *textFont;

    Scene *currentScene = nullptr;

public:
    Game();
    ~Game();
    void init();
    void run();
    void clean();
    SDL_Renderer *getRenderer() { return renderer; }
    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }
    Uint32 getFrameTime() { return frameTime; }
    void renderText(const std::string &text, int y, bool isTitle = false, bool isCenter = false);
    void changeScene(Scene *newScene);
};

#endif