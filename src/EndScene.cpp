#include "EndScene.h"
#include "MainScene.h"
#include "Game.h"

void EndScene::init()
{
}

void EndScene::run(float deltaTime)
{
    game.renderText("Game Over", game.getWindowHeight() / 2 - 50, true, true);
    game.renderText("Press R to Restart", game.getWindowHeight() / 2 + 20, false, true);
}

void EndScene::clean()
{
}

void EndScene::handleEvent(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_r)
    {
        auto mainScene = new MainScene(game);
        game.changeScene(mainScene);
    }
}
