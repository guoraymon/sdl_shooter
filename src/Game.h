#ifndef GAME_H
#define GAME_H

#include <list>
#include <map>
#include <random>
#include <fstream>

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

    std::multimap<int, std::string, std::greater<int>> leaderboard;

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

    void insertLeaderboard(int score, const std::string &name)
    {
        leaderboard.insert(std::make_pair(score, name));
        if (leaderboard.size() > 8)
        {
            leaderboard.erase(--leaderboard.end());
        }
    }
    std::multimap<int, std::string, std::greater<int>> getLeaderboard() { return leaderboard; }

    void saveData()
    {
        std::ofstream file("save.dat");
        if (file.is_open())
        {
            for (const auto &entry : leaderboard)
            {
                file << entry.second << "," << entry.first << std::endl;
            }
            file.close();
        }
    }

    void loadData()
    {
        std::ifstream file("save.dat");
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                size_t delimiterPos = line.find(',');
                if (delimiterPos != std::string::npos)
                {
                    std::string name = line.substr(0, delimiterPos);
                    int score = std::stoi(line.substr(delimiterPos + 1));
                    leaderboard.insert(std::make_pair(score, name));
                }
            }
            file.close();
        }
    }
};

#endif
