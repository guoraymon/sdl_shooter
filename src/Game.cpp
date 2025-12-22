#include <SDL.h>
#include <SDL_image.h>

#include "Game.h"
#include "Object.h"
#include "TitleScene.h"

Game::Game()
{
}

Game::~Game()
{
    saveData();
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

    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);

    loadData();
    changeScene(new TitleScene(*this));
}

void Game::run()
{
    float deltaTime = 0;
    while (true)
    {
        auto frameStart = SDL_GetTicks();

        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                break;
            }
            currentScene->handleEvent(&event);
        }

        SDL_RenderClear(renderer);

        currentScene->run(deltaTime);

        SDL_RenderPresent(renderer);

        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;
        if (diff < getFrameTime())
        {
            SDL_Delay(getFrameTime() - diff);
            deltaTime = getFrameTime() / 1000.0f;
        }
        else
        {
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::changeScene(Scene *newScene)
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }
    currentScene = newScene;
    currentScene->init();
}

SDL_Point Game::renderText(std::string text, int y, bool isTitle, bool isCenter)
{
    TTF_Font *font = isTitle ? titleFont : textFont;
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    int x = isCenter ? (windowWidth - textSurface->w) / 2 : 0;
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return SDL_Point{textRect.x + textRect.w, y};
}

SDL_Point Game::renderTextPos(std::string text, int x, int y, bool isTitle)
{
    TTF_Font *font = isTitle ? titleFont : textFont;
    SDL_Color color = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderUTF8_Solid(font, text.c_str(), color);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    return SDL_Point{textRect.x + textRect.w, y};
}

void Game::setFinalScore(int score)
{
    finalScore = score;
}

void Game::clean()
{
    if (currentScene != nullptr)
    {
        currentScene->clean();
        delete currentScene;
    }

    TTF_CloseFont(titleFont);
    TTF_CloseFont(textFont);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
