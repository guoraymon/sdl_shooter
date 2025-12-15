#include "TitleScene.h"
#include "MainScene.h"
#include "Game.h"

void TitleScene::init()
{
    music = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    Mix_PlayMusic(music, -1);

    titleFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 64);
    textFont = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 32);
};

void TitleScene::run(float deltaTime)
{
    // Render title text
    std::string titleText = "太空战机";
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *titleSurface = TTF_RenderUTF8_Solid(titleFont, titleText.c_str(), white);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(game.getRenderer(), titleSurface);
    SDL_Rect titleRect = {game.getWindowWidth() / 2 - titleSurface->w / 2, game.getWindowHeight() / 4 - titleSurface->h / 2, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(game.getRenderer(), titleTexture, NULL, &titleRect);
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Render instruction text
    std::string instructionText = "按 J 键开始游戏";
    SDL_Surface *instructionSurface = TTF_RenderUTF8_Solid(textFont, instructionText.c_str(), white);
    SDL_Texture *instructionTexture = SDL_CreateTextureFromSurface(game.getRenderer(), instructionSurface);
    SDL_Rect instructionRect = {game.getWindowWidth() / 2 - instructionSurface->w / 2, game.getWindowHeight() / 2 - instructionSurface->h / 2, instructionSurface->w, instructionSurface->h};
    SDL_RenderCopy(game.getRenderer(), instructionTexture, NULL, &instructionRect);
    SDL_FreeSurface(instructionSurface);
    SDL_DestroyTexture(instructionTexture);
}

void TitleScene::handleEvent(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_j)
    {
        auto mainScene = new MainScene(game);
        game.changeScene(mainScene);
    }
}

void TitleScene::clean()
{
    Mix_HaltMusic();
    Mix_FreeMusic(music);

    TTF_CloseFont(titleFont);
    TTF_CloseFont(textFont);
}