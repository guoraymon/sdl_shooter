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

    int finalScore = 0;

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
    SDL_Point renderText(std::string text, int y, bool isTitle = false, bool isCenter = false);
    SDL_Point renderTextPos(std::string text, int x, int y, bool isTitle = false);
    void changeScene(Scene *newScene);
    void setFinalScore(int score);
    int getFinalScore() { return finalScore; }
};

#endif