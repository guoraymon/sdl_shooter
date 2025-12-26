#include <SDL_image.h>

#include "MainScene.h"
#include "TitleScene.h"
#include "EndScene.h"
#include "Game.h"

void MainScene::init()
{
    player.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/SpaceShip.png");
    SDL_QueryTexture(player.texture, NULL, NULL, &player.width, &player.height);
    player.width /= 3;
    player.height /= 3;
    player.position.x = game.getWindowWidth() / 2 - player.width / 2;
    player.position.y = game.getWindowHeight() - player.height;
    player.velocity.x = 0.0f;  // 初始化速度为0
    player.velocity.y = 0.0f;

    playerProjectileTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/laser-1.png");
    SDL_QueryTexture(playerProjectileTemplate.texture, NULL, NULL, &playerProjectileTemplate.width, &playerProjectileTemplate.height);

    randomGenerator = std::mt19937(std::random_device{}());
    randomDistribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

    enemyTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/insect-1.png");
    SDL_QueryTexture(enemyTemplate.texture, NULL, NULL, &enemyTemplate.width, &enemyTemplate.height);
    enemyTemplate.width /= 2;
    enemyTemplate.height /= 2;

    enemyProjectileTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bullet-1.png");
    SDL_QueryTexture(enemyProjectileTemplate.texture, NULL, NULL, &enemyProjectileTemplate.width, &enemyProjectileTemplate.height);

    explosionTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/effect/explosion.png");
    SDL_QueryTexture(explosionTemplate.texture, NULL, NULL, &explosionTemplate.width, &explosionTemplate.height);
    explosionTemplate.frameCount = explosionTemplate.width / explosionTemplate.height;
    explosionTemplate.width /= explosionTemplate.frameCount;

    // Load item texture
    itemTemplate.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/bonus_life.png");
    SDL_QueryTexture(itemTemplate.texture, NULL, NULL, &itemTemplate.width, &itemTemplate.height);

    // Set up backgrounds
    // near background
    nearBackground.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/Stars-A.png");
    nearBackground.width = game.getWindowWidth();
    nearBackground.height = game.getWindowHeight();
    nearBackground.speed = 50;
    // far background
    farBackground.texture = IMG_LoadTexture(game.getRenderer(), "assets/image/Stars-B.png");
    farBackground.width = game.getWindowWidth();
    farBackground.height = game.getWindowHeight();
    farBackground.speed = 20;

    // Load health texture
    healthTexture = IMG_LoadTexture(game.getRenderer(), "assets/image/Health UI Black.png");

    // Load and play background music
    music = Mix_LoadMUS("assets/music/03_Racing_Through_Asteroids_Loop.ogg");
    Mix_PlayMusic(music, -1);

    // Load sound effects
    sounds["player_shoot"] = Mix_LoadWAV("assets/sound/laser_shoot4.wav");
    sounds["enemy_shoot"] = Mix_LoadWAV("assets/sound/xs_laser.wav");
    sounds["player_explosion"] = Mix_LoadWAV("assets/sound/explosion1.wav");
    sounds["enemy_explosion"] = Mix_LoadWAV("assets/sound/explosion3.wav");
    sounds["item_pickup"] = Mix_LoadWAV("assets/sound/eff5.wav");
    sounds["hit"] = Mix_LoadWAV("assets/sound/eff11.wav");

    // Load font for score display
    scoreFont = TTF_OpenFont("assets/font/VonwaonBitmap-12px.ttf", 24);
}

void MainScene::run(float deltaTime)
{
    keyboardController(deltaTime);

    // Render player
    if (player.health > 0)
    {
        SDL_Rect playerRect = {static_cast<int>(player.position.x), static_cast<int>(player.position.y), player.width, player.height};
        SDL_RenderCopy(game.getRenderer(), player.texture, NULL, &playerRect);

        // Update and render player projectiles
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
                        // play hit sound
                        Mix_PlayChannel(-1, sounds["hit"], 0);
                        if (enemy->health <= 0)
                        {
                            auto explosion = new Explosion(explosionTemplate);
                            explosion->position.x = enemy->position.x + enemy->width / 2 - explosion->width / 2;
                            explosion->position.y = enemy->position.y + enemy->height / 2 - explosion->height / 2;
                            explosion->lastFrameTime = SDL_GetTicks();
                            explosions.push_back(explosion);
                            // Drop item with 50% chance
                            if (randomDistribution(randomGenerator) < 0.5f)
                            {
                                auto item = new Item(itemTemplate);
                                item->position.x = enemy->position.x + enemy->width / 2 - item->width / 2;
                                item->position.y = enemy->position.y + enemy->height / 2 - item->height / 2;
                                // Random direction
                                float angle = randomDistribution(randomGenerator) * 2.0f * M_PI;
                                item->direction.x = cosf(angle);
                                item->direction.y = sinf(angle);
                                items.push_back(item);
                            }
                            score += 10;
                            delete enemy;
                            enemyIt = enemies.erase(enemyIt);
                            // play enemy explosion sound
                            Mix_PlayChannel(-1, sounds["enemy_explosion"], 0);
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
                    SDL_RenderCopy(game.getRenderer(), projectile->texture, NULL, &projectileRect);
                    ++it;
                }
            }
        }
    }

    // Update and render enemies
    for (auto it = enemies.begin(); it != enemies.end();)
    {
        auto enemy = *it;
        enemy->position.y += enemy->speed * deltaTime;
        if (enemy->position.y > game.getWindowHeight())
        {
            delete enemy;
            it = enemies.erase(it);
        }
        else
        {
            if (player.health > 0)
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
                    // Play enemy shoot sound
                    Mix_PlayChannel(-1, sounds["enemy_shoot"], 0);
                }
            }
            SDL_Rect enemyRect = {static_cast<int>(enemy->position.x), static_cast<int>(enemy->position.y), enemy->width, enemy->height};
            SDL_RenderCopy(game.getRenderer(), enemy->texture, NULL, &enemyRect);
            ++it;
        }
    }

    if (player.health > 0)
    {
        // Spawn enemies
        if (randomDistribution(randomGenerator) < 0.02f)
        {
            auto enemy = new Enemy(enemyTemplate);
            enemy->position.x = randomDistribution(randomGenerator) * (game.getWindowWidth() - enemy->width);
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
                projectile->position.x > game.getWindowWidth() ||
                projectile->position.y < -projectile->height ||
                projectile->position.y > game.getWindowHeight())
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
                        Mix_PlayChannel(-1, sounds["player_explosion"], 0);
                        endTime = SDL_GetTicks();
                        game.setFinalScore(score);
                        return;
                    }
                    delete projectile;
                    it = enemyProjectiles.erase(it);
                }
                else
                {
                    float angle = atan2f(projectile->direction.y, projectile->direction.x) * 180.0f / M_PI - 90.0f;
                    SDL_RenderCopyEx(game.getRenderer(), projectile->texture, NULL, &projectileRect, angle, NULL, SDL_FLIP_NONE);
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
                SDL_RenderCopy(game.getRenderer(), explosion->texture, &srcRect, &dstRect);
                ++it;
            }
        }

        // Update and render items
        for (auto it = items.begin(); it != items.end();)
        {
            auto item = *it;
            item->position.x += item->direction.x * item->speed * deltaTime;
            item->position.y += item->direction.y * item->speed * deltaTime;
            // Bounce off the left wall
            if (item->position.x < 0 && item->bounceCount > 0)
            {
                item->direction.x = -item->direction.x;
                item->bounceCount--;
            }
            // Bounce off the right wall
            if (item->position.x + item->width > game.getWindowWidth() && item->bounceCount > 0)
            {
                item->direction.x = -item->direction.x;
                item->bounceCount--;
            }
            // Bounce off the top wall
            if (item->position.y < 0 && item->bounceCount > 0)
            {
                item->direction.y = -item->direction.y;
                item->bounceCount--;
            }
            // Bounce off the bottom wall
            if (item->position.y + item->height > game.getWindowHeight() && item->bounceCount > 0)
            {
                item->direction.y = -item->direction.y;
                item->bounceCount--;
            }

            if (item->position.x < -item->width ||
                item->position.x > game.getWindowWidth() ||
                item->position.y < -item->height ||
                item->position.y > game.getWindowHeight())
            {
                delete item;
                it = items.erase(it);
            }
            else
            {
                SDL_Rect itemRect = {static_cast<int>(item->position.x), static_cast<int>(item->position.y), item->width, item->height};
                auto playerRect = SDL_Rect{static_cast<int>(player.position.x), static_cast<int>(player.position.y), player.width, player.height};
                if (SDL_HasIntersection(&playerRect, &itemRect))
                {
                    player.health += 1; // Increase player health
                    delete item;
                    it = items.erase(it);
                    // Play item pickup sound
                    Mix_PlayChannel(-1, sounds["item_pickup"], 0);
                }
                else
                {
                    SDL_RenderCopy(game.getRenderer(), item->texture, NULL, &itemRect);
                    ++it;
                }
            }
        }
    }

    // Render backgrounds
    nearBackground.position.y += nearBackground.speed * deltaTime;
    if (nearBackground.position.y >= nearBackground.height)
    {
        nearBackground.position.y = 0;
    }
    SDL_Rect nearRect1 = {0, static_cast<int>(nearBackground.position.y), nearBackground.width, nearBackground.height};
    SDL_RenderCopy(game.getRenderer(), nearBackground.texture, NULL, &nearRect1);
    SDL_Rect nearRect2 = {0, static_cast<int>(nearBackground.position.y - nearBackground.height), game.getWindowWidth(), nearBackground.height};
    SDL_RenderCopy(game.getRenderer(), nearBackground.texture, NULL, &nearRect2);

    farBackground.position.y += farBackground.speed * deltaTime;
    if (farBackground.position.y >= farBackground.height)
    {
        farBackground.position.y = 0;
    }
    SDL_Rect farRect1 = {0, static_cast<int>(farBackground.position.y), farBackground.width, farBackground.height};
    SDL_RenderCopy(game.getRenderer(), farBackground.texture, NULL, &farRect1);
    SDL_Rect farRect2 = {0, static_cast<int>(farBackground.position.y - farBackground.height), farBackground.width, farBackground.height};
    SDL_RenderCopy(game.getRenderer(), farBackground.texture, NULL, &farRect2);

    // Render player health
    SDL_SetTextureColorMod(healthTexture, 255, 255, 255);
    for (int i = 0; i < player.health; ++i)
    {
        SDL_Rect healthRect = {10 + i * (32 + 5), 10, 32, 32};
        SDL_RenderCopy(game.getRenderer(), healthTexture, NULL, &healthRect);
    }
    SDL_SetTextureColorMod(healthTexture, 100, 100, 100);
    for (int i = player.health; i < player.maxHealth; ++i)
    {
        SDL_Rect healthRect = {10 + i * (32 + 5), 10, 32, 32};
        SDL_RenderCopy(game.getRenderer(), healthTexture, NULL, &healthRect);
    }

    // Render score
    SDL_Color white = {255, 255, 255, 255};
    std::string scoreText = "Score: " + std::to_string(score);
    SDL_Surface *scoreSurface = TTF_RenderUTF8_Solid(scoreFont, scoreText.c_str(), white);
    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(game.getRenderer(), scoreSurface);
    SDL_Rect scoreRect = {game.getWindowWidth() - scoreSurface->w - 10, 10, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(game.getRenderer(), scoreTexture, NULL, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    // Check for game over
    if (player.health <= 0)
    {
        auto now = SDL_GetTicks();
        if (now - endTime >= endDelay)
        {
            auto endScene = new EndScene(game);
            game.changeScene(endScene);
            return;
        }
    }
}

void MainScene::handleEvent(SDL_Event *event)
{
    if (event->type == SDL_KEYDOWN && event->key.keysym.sym == SDLK_ESCAPE)
    {
        auto titleScene = new TitleScene(game);
        game.changeScene(titleScene);
    }
}

void MainScene::keyboardController(float deltaTime)
{
    auto keyboardState = SDL_GetKeyboardState(NULL);

    // 计算输入方向
    float inputX = 0.0f;
    float inputY = 0.0f;

    if (keyboardState[SDL_SCANCODE_W])
    {
        inputY = -1.0f;
    }
    if (keyboardState[SDL_SCANCODE_S])
    {
        inputY = 1.0f;
    }
    if (keyboardState[SDL_SCANCODE_A])
    {
        inputX = -1.0f;
    }
    if (keyboardState[SDL_SCANCODE_D])
    {
        inputX = 1.0f;
    }

    // 根据输入应用加速度
    if (inputX != 0.0f)
    {
        player.velocity.x += inputX * player.acceleration * deltaTime;
        // 限制最大速度
        if (player.velocity.x > player.maxSpeed)
        {
            player.velocity.x = player.maxSpeed;
        }
        else if (player.velocity.x < -player.maxSpeed)
        {
            player.velocity.x = -player.maxSpeed;
        }
    }

    if (inputY != 0.0f)
    {
        player.velocity.y += inputY * player.acceleration * deltaTime;
        // 限制最大速度
        if (player.velocity.y > player.maxSpeed)
        {
            player.velocity.y = player.maxSpeed;
        }
        else if (player.velocity.y < -player.maxSpeed)
        {
            player.velocity.y = -player.maxSpeed;
        }
    }

    // 如果没有输入，应用减速
    if (inputX == 0.0f)
    {
        // 应用水平减速
        if (player.velocity.x > 0)
        {
            player.velocity.x -= player.deceleration * deltaTime;
            if (player.velocity.x < 0)
                player.velocity.x = 0;
        }
        else if (player.velocity.x < 0)
        {
            player.velocity.x += player.deceleration * deltaTime;
            if (player.velocity.x > 0)
                player.velocity.x = 0;
        }
    }

    if (inputY == 0.0f)
    {
        // 应用垂直减速
        if (player.velocity.y > 0)
        {
            player.velocity.y -= player.deceleration * deltaTime;
            if (player.velocity.y < 0)
                player.velocity.y = 0;
        }
        else if (player.velocity.y < 0)
        {
            player.velocity.y += player.deceleration * deltaTime;
            if (player.velocity.y > 0)
                player.velocity.y = 0;
        }
    }

    // 更新位置
    player.position.x += player.velocity.x * deltaTime;
    player.position.y += player.velocity.y * deltaTime;

    // 边界检查
    if (player.position.x < 0)
    {
        player.position.x = 0;
        player.velocity.x = 0; // 碰到边界时停止移动
    }
    if (player.position.x > game.getWindowWidth() - player.width)
    {
        player.position.x = game.getWindowWidth() - player.width;
        player.velocity.x = 0; // 碰到边界时停止移动
    }
    if (player.position.y < 0)
    {
        player.position.y = 0;
        player.velocity.y = 0; // 碰到边界时停止移动
    }
    if (player.position.y > game.getWindowHeight() - player.height)
    {
        player.position.y = game.getWindowHeight() - player.height;
        player.velocity.y = 0; // 碰到边界时停止移动
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
            // Play shoot sound
            Mix_PlayChannel(-1, sounds["player_shoot"], 0);
        }
    }
}

void MainScene::clean()
{
    // Clean up player
    SDL_DestroyTexture(player.texture);
    // Clean up player projectiles
    for (auto &projectile : playerProjectiles)
    {
        delete projectile;
    }
    playerProjectiles.clear();
    SDL_DestroyTexture(playerProjectileTemplate.texture);

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

    // Clean up explosions
    for (auto &explosion : explosions)
    {
        delete explosion;
    }
    explosions.clear();
    SDL_DestroyTexture(explosionTemplate.texture);

    // Clean up items
    for (auto &item : items)
    {
        delete item;
    }
    items.clear();
    SDL_DestroyTexture(itemTemplate.texture);

    // Clean up backgrounds
    SDL_DestroyTexture(nearBackground.texture);
    SDL_DestroyTexture(farBackground.texture);

    // Clean up health texture
    SDL_DestroyTexture(healthTexture);

    // Clean up sounds
    for (auto &pair : sounds)
    {
        Mix_FreeChunk(pair.second);
    }
    sounds.clear();
    Mix_HaltMusic();
    Mix_FreeMusic(music);

    // Clean up score font
    TTF_CloseFont(scoreFont);
}