#include <SDL.h>
#include <SDL_image.h>

#include "Game.h"
#include "Object.h"
#include "MainScene.h"

Game::Game()
{
}

Game::~Game()
{
    Game::clean();
}

void Game::init()
{
    // SDL init
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Init Error: %s\n", SDL_GetError());
    }
    // SDL_image init
    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG))
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_Image Error: %s\n", SDL_GetError());
    }
    // SDL_ttf init
    if (TTF_Init() != 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_ttf Error: %s\n", SDL_GetError());
    }

    // SDL_mixer init
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_mix Error: %s\n", SDL_GetError());
    }
    Mix_AllocateChannels(32);
    Mix_VolumeMusic(MIX_MAX_VOLUME / 4);
    Mix_Volume(-1, MIX_MAX_VOLUME / 8);

    // Create window
    window = SDL_CreateWindow("Hello world!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    currentScene = new MainScene(*this);
    currentScene->init();
}

void Game::run()
{
    currentScene->run();
}

void Game::clean()
{
    currentScene->clean();
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
