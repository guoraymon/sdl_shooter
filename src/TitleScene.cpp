#include "TitleScene.h"
#include "MainScene.h"
#include "Game.h"

void TitleScene::init()
{
    music = Mix_LoadMUS("assets/music/06_Battle_in_Space_Intro.ogg");
    Mix_PlayMusic(music, -1);
};

void TitleScene::run(float deltaTime)
{
    // Render title text
    std::string titleText = "太空战机";
    game.renderText(titleText, game.getWindowHeight() / 4, true, true);

    // Render instruction text
    std::string instructionText = "按 J 键开始游戏";
    game.renderText(instructionText, game.getWindowHeight() / 2, false, true);
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
}