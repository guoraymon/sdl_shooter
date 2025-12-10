#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"
#include "Object.h"

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

    // Create window
    window = SDL_CreateWindow("Hello world!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG))
    // {
    //     std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
    //     return 1;
    // }
    // SDL_Texture *texture = IMG_LoadTexture(renderer, "assets/image/bg.png");

    // if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    // {
    //     std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
    //     return 1;
    // }
    // Mix_Music *music = Mix_LoadMUS("assets/music/03_Racing_Through_Through_Asteroids_Loop.ogg");
    // Mix_PlayMusic(music, -1);

    // if (TTF_Init() != 0)
    // {
    //     std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
    //     return 1;
    // }
    // TTF_Font *font = TTF_OpenFont("assets/font/VonwaonBitmap-16px.ttf", 24);
    // SDL_Color color = {255, 255, 255, 255};
    // SDL_Surface *surface = TTF_RenderUTF8_Solid(font, "你好, SDL!", color);
    // SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, surface);

    player.texture = IMG_LoadTexture(renderer, "assets/image/SpaceShip.png");
    player.width = 64;
    player.height = 64;
    player.position.x = windowWidth / 2 - player.width / 2;
    player.position.y = windowHeight - player.height;

    playerProjectileTemplate.texture = IMG_LoadTexture(renderer, "assets/image/laser-1.png");
    playerProjectileTemplate.width = 81;
    playerProjectileTemplate.height = 126;

    randomGenerator = std::mt19937(std::random_device{}());
    randomDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

    enemyTemplate.texture = IMG_LoadTexture(renderer, "assets/image/insect-1.png");
    enemyTemplate.width = 64;
    enemyTemplate.height = 64;
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
        }

        keyboardControll(deltaTime);

        SDL_RenderClear(renderer);

        SDL_Rect rect = {100, 100, 200, 200};
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

        // SDL_Rect dstrect = {200, 200, 200, 200};
        // SDL_RenderCopy(renderer, texture, NULL, &dstrect);

        // SDL_Rect textRect = {300, 300, surface->w, surface->h};
        // SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

        SDL_Rect playerRect = {static_cast<int>(player.position.x), static_cast<int>(player.position.y), player.width, player.height};
        SDL_RenderCopy(renderer, player.texture, NULL, &playerRect);

        for (auto it = playerProjectiles.begin(); it != playerProjectiles.end();)
        {
            auto projectile = *it;
            projectile->position.y -= projectile->speed * deltaTime;
            if (projectile->position.y < -projectile->height)
            {
                delete projectile;
                it = playerProjectiles.erase(it);
            }
            else
            {
                SDL_Rect projectileRect = {static_cast<int>(projectile->position.x), static_cast<int>(projectile->position.y), projectile->width, projectile->height};
                SDL_RenderCopy(renderer, projectile->texture, NULL, &projectileRect);
                ++it;
            }
        }

        // Spawn enemies
        if (randomDistribution(randomGenerator) < 0.02f)
        {
            auto enemy = new Enemy(enemyTemplate);
            enemy->position.x = randomDistribution(randomGenerator) * (windowWidth - enemy->width);
            enemy->position.y = -enemy->height;
            enemies.push_back(enemy);
        }
        // Update and render enemies
        for (auto it = enemies.begin(); it != enemies.end();)
        {
            auto enemy = *it;
            enemy->position.y += enemy->speed * deltaTime;
            if (enemy->position.y > windowHeight)
            {
                delete enemy;
                it = enemies.erase(it);
            }
            else
            {
                SDL_Rect enemyRect = {static_cast<int>(enemy->position.x), static_cast<int>(enemy->position.y), enemy->width, enemy->height};
                SDL_RenderCopy(renderer, enemy->texture, NULL, &enemyRect);
                ++it;
            }
        }

        SDL_RenderPresent(renderer);

        auto frameEnd = SDL_GetTicks();
        auto diff = frameEnd - frameStart;
        if (diff < frameTime)
        {
            SDL_Delay(frameTime - diff);
            deltaTime = frameTime / 1000.0f;
        }
        else
        {
            deltaTime = diff / 1000.0f;
        }
    }
}

void Game::keyboardControll(float deltaTime)
{
    auto keyboardState = SDL_GetKeyboardState(NULL);
    if (keyboardState[SDL_SCANCODE_W])
    {
        if (player.position.y > 0)
        {
            player.position.y -= player.speed * deltaTime;
        }
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
        if (player.position.x > 0)
        {
            player.position.x -= player.speed * deltaTime;
        }
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        if (player.position.y < windowHeight - player.height)
        {
            player.position.y += player.speed * deltaTime;
        }
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        if (player.position.x < windowWidth - player.width)
        {
            player.position.x += deltaTime * player.speed;
        }
    }

    if (keyboardState[SDL_SCANCODE_J])
    {
        auto now = SDL_GetTicks();
        if (player.lastShootTime + player.coolDown <= now)
        {
            auto playerProjectile = new PlayerProjectile(playerProjectileTemplate);
            playerProjectile->position.x = player.position.x + player.width / 2 - playerProjectile->width / 2;
            playerProjectile->position.y = player.position.y;
            playerProjectiles.push_back(playerProjectile);
            player.lastShootTime = now;
        }
    }
}

void Game::clean()
{
    // SDL_DestroyTexture(texture);
    // IMG_Quit();

    // Mix_FreeMusic(music);
    // Mix_CloseAudio();
    // Mix_Quit();

    // SDL_FreeSurface(surface);
    // SDL_DestroyTexture(textTexture);
    // TTF_CloseFont(font);
    // TTF_Quit();

    for (auto &enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
    SDL_DestroyTexture(enemyTemplate.texture);

    for (auto &projectile : playerProjectiles)
    {
        delete projectile;
    }
    playerProjectiles.clear();
    SDL_DestroyTexture(playerProjectileTemplate.texture);

    SDL_DestroyTexture(player.texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
