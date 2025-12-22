#include "EndScene.h"
#include "MainScene.h"
#include "Game.h"

void EndScene::init()
{
}

void EndScene::run(float deltaTime)
{
    if (isTyping)
    {
        game.renderText("Game Over", game.getWindowHeight() / 2 - 100, true, true);
        game.renderText("Final Score: " + std::to_string(game.getFinalScore()), game.getWindowHeight() / 2 - 10, false, true);
        game.renderText("Please input your name:", game.getWindowHeight() / 2 + 20, false, true);
        SDL_Point point;
        if (name != "")
        {
            point = game.renderText(name, game.getWindowHeight() / 2 + 50, false, true);
        }
        else
        {
            point = SDL_Point{(game.getWindowWidth() - 10) / 2, game.getWindowHeight() / 2 + 50};
        }
        blinkingCursorTimer += deltaTime;
        if (blinkingCursorTimer >= 0.5f)
        {
            game.renderTextPos("_", point.x, game.getWindowHeight() / 2 + 50, false);
            if (blinkingCursorTimer >= 1.0f)
            {
                blinkingCursorTimer = 0.0f;
            }
        }
    }
    else
    {
        auto offsetY = game.getWindowHeight() * 0.2;
        game.renderText("Leaderboard", offsetY, false, true);
        offsetY += 40;
        auto i = 1;
        for (auto &entry : game.getLeaderboard())
        {
            game.renderTextPos(std::to_string(i) + ". " + entry.second, 50, offsetY);
            game.renderTextPos(std::to_string(entry.first), game.getWindowWidth() - 100, offsetY);
            offsetY += 30;
            i++;
        }
        game.renderText("Press 'R' to restart", game.getWindowHeight() / 2 + 100, false, true);
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
                auto last_char = name.back();
                if ((last_char & 0b10000000) == 0b10000000)
                {
                    name.pop_back();
                    while ((name.back() & 0b11000000) != 0b11000000)
                    {
                        name.pop_back();
                    }
                }
                name.pop_back();
            }
            else if (event->key.keysym.sym == SDLK_RETURN || event->key.keysym.sym == SDLK_KP_ENTER)
            {
                isTyping = false;
                game.insertLeaderboard(game.getFinalScore(), name);
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
