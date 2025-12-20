#include "EndScene.h"
#include "MainScene.h"
#include "Game.h"

void EndScene::init()
{
}

void EndScene::run(float deltaTime)
{
    game.renderText("Game Over", game.getWindowHeight() / 2 - 100, true, true);
    game.renderText("Final Score: " + std::to_string(game.getFinalScore()), game.getWindowHeight() / 2 - 10, false, true);
    game.renderText("Please input your name:", game.getWindowHeight() / 2 + 20, false, true);
    if (isTyping)
    {
        game.renderText(name + "_", game.getWindowHeight() / 2 + 50, false, true);
    }
    else
    {
        game.renderText(name, game.getWindowHeight() / 2 + 50, false, true);
        game.renderText("Press 'R' to restart", game.getWindowHeight() / 2 + 80, false, true);
    }
}

void EndScene::clean()
{
}

void EndScene::handleEvent(SDL_Event *event)
{
    if (isTyping)
    {
        if (event->type == SDL_TEXTINPUT)
        {
            name += event->text.text;
        }
        else if (event->type == SDL_KEYDOWN)
        {
            if (event->key.keysym.sym == SDLK_BACKSPACE && !name.empty())
            {
                name.pop_back();
            }
            else if (event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_KP_ENTER)
            {
                isTyping = false;
            }
        }
    }
    else
    {
        if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_r)
        {
            auto mainScene = new MainScene(game);
            game.changeScene(mainScene);
        }
    }
}
