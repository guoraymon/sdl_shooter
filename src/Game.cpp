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
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width /= 3;
    player.height /= 3;
    player.position.x = windowWidth / 2 - player.width / 2;
    player.position.y = windowHeight - player.height;

    playerProjectileTemplate.texture = IMG_LoadTexture(renderer, "assets/image/laser-1.png");
    SDL_QueryTexture(playerProjectileTemplate.texture, NULL, NULL, &playerProjectileTemplate.width, &playerProjectileTemplate.height);

    randomGenerator = std::mt19937(std::random_device{}());
    randomDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

    enemyTemplate.texture = IMG_LoadTexture(renderer, "assets/image/insect-1.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 2;
    enemyTemplate.height /= 2;

    enemyProjectileTemplate.texture = IMG_LoadTexture(renderer, "assets/image/bullet-1.png");
    SDL_QueryTexture(enemyProjectileTemplate.texture, NULL, NULL, &enemyProjectileTemplate.width, &enemyProjectileTemplate.height);

    explosionTemplate.texture = IMG_LoadTexture(renderer, "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.frameCount = explosionTemplate.width / explosionTemplate.height;
    explosionTemplate.width /= explosionTemplate.frameCount;
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

                bool isHit = false;
                for (auto enemyIt = enemies.begin(); enemyIt != enemies.end();)
                {
                    auto enemy = *enemyIt;
                    SDL_Rect enemyRect = {static_cast<int>(enemy->position.x), static_cast<int>(enemy->position.y), enemy->width, enemy->height};
                    if (SDL_HasIntersection(&enemyRect, &projectileRect))
                    {
                        isHit = true;
                        enemy->health -= projectile->damage;
                        if (enemy->health <= 0)
                        {
                            auto explosion = new Explosion(explosionTemplate);
                            explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
                            explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
                            explosion->lastFrameTime = SDL_GetTicks();
                            explosions.push_back(explosion);
                            delete enemy;
                            enemyIt = enemies.erase(enemyIt);
                        }
                        delete projectile;
                        it = playerProjectiles.erase(it);
                        break;
                    }
                    else
                    {
                        ++enemyIt;
                    }
                }
                if (!isHit)
                {
                    SDL_RenderCopy(renderer, projectile->texture, NULL, &projectileRect);
                    ++it;
                }
            }
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
                // Enemy shooting
                auto now = SDL_GetTicks();
                if (enemy->lastShootTime + enemy->coolDown <= now)
                {
                    auto enemyProjectile = new EnemyProjectile(enemyProjectileTemplate);
                    enemyProjectile->position.x = enemy->position.x + enemy->width / 2 - enemyProjectile->width / 2;
                    enemyProjectile->position.y = enemy->position.y + enemy->height;
                    // direction towards player
                    float dirX = (player.position.x + player.width / 2) - (enemy->position.x + enemy->width / 2);
                    float dirY = (player.position.y + player.height / 2) - (enemy->position.y + enemy->height / 2);
                    float length = sqrtf(dirX * dirX + dirY * dirY);
                    enemyProjectile->direction.x = dirX / length;
                    enemyProjectile->direction.y = dirY / length;
                    enemyProjectiles.push_back(enemyProjectile);
                    enemy->lastShootTime = now;
                }
                SDL_Rect enemyRect = {static_cast<int>(enemy->position.x), static_cast<int>(enemy->position.y), enemy->width, enemy->height};
                SDL_RenderCopy(renderer, enemy->texture, NULL, &enemyRect);
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
        // Update and render enemy projectiles
        for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end();)
        {
            auto projectile = *it;
            projectile->position.x += projectile->direction.x * projectile->speed * deltaTime;
            projectile->position.y += projectile->direction.y * projectile->speed * deltaTime;
            if (projectile->position.x < -projectile->width ||
                projectile->position.x > windowWidth ||
                projectile->position.y < -projectile->height ||
                projectile->position.y > windowHeight)
            {
                delete projectile;
                it = enemyProjectiles.erase(it);
            }
            else
            {
                SDL_Rect projectileRect = {static_cast<int>(projectile->position.x), static_cast<int>(projectile->position.y), projectile->width, projectile->height};
                auto playerRect = SDL_Rect{static_cast<int>(player.position.x), static_cast<int>(player.position.y), player.width, player.height};
                if (SDL_HasIntersection(&playerRect, &projectileRect))
                {
                    player.health -= projectile->damage;
                    if (player.health <= 0)
                    {
                        // Game over logic can be implemented here
                    }
                    delete projectile;
                    it = enemyProjectiles.erase(it);
                }
                else
                {
                    float angle = atan2f(projectile->direction.y, projectile->direction.x) * 180.0f / M_PI - 90.0f;
                    SDL_RenderCopyEx(renderer, projectile->texture, NULL, &projectileRect, angle, NULL, SDL_FLIP_NONE);
                    ++it;
                }
            }
        }

        // Update and render explosions
        for (auto it = explosions.begin(); it != explosions.end();)
        {
            auto explosion = *it;
            auto now = SDL_GetTicks();
            if (now - explosion->lastFrameTime >= explosion->frameDuration)
            {
                explosion->currentFrame++;
                explosion->lastFrameTime = now;
            }
            if (explosion->currentFrame >= explosion->frameCount)
            {
                delete explosion;
                it = explosions.erase(it);
            }
            else
            {
                SDL_Rect srcRect = {explosion->currentFrame * explosion->width, 0, explosion->width, explosion->height};
                SDL_Rect dstRect = {static_cast<int>(explosion->position.x), static_cast<int>(explosion->position.y), explosion->width, explosion->height};
                SDL_RenderCopy(renderer, explosion->texture, &srcRect, &dstRect);
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

    // Clean up enemies
    for (auto &enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
    SDL_DestroyTexture(enemyTemplate.texture);
    // Clean up enemy projectiles
    for (auto &projectile : enemyProjectiles)
    {
        delete projectile;
    }
    enemyProjectiles.clear();
    SDL_DestroyTexture(enemyProjectileTemplate.texture);

    // Clean up player
    SDL_DestroyTexture(player.texture);
    // Clean up player projectiles
    for (auto &projectile : playerProjectiles)
    {
        delete projectile;
    }
    playerProjectiles.clear();
    SDL_DestroyTexture(playerProjectileTemplate.texture);

    // Clean up explosions
    for (auto &explosion : explosions)
    {
        delete explosion;
    }
    explosions.clear();
    SDL_DestroyTexture(explosionTemplate.texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
