#include <list>
#include <map>
#include <random>

#include "SDL.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "Scene.h"
#include "Object.h"

class MainScene : public Scene
{
private:
    Player player;
    PlayerProjectile playerProjectileTemplate;
    std::list<PlayerProjectile *> playerProjectiles;

    std::mt19937 randomGenerator;
    std::uniform_real_distribution<float> randomDistribution;

    Enemy enemyTemplate;
    std::list<Enemy *> enemies;
    EnemyProjectile enemyProjectileTemplate;
    std::list<EnemyProjectile *> enemyProjectiles;

    Explosion explosionTemplate;
    std::list<Explosion *> explosions;

    Item itemTemplate;
    std::list<Item *> items;

    Mix_Music *music;
    std::map<std::string, Mix_Chunk *> sounds;

    Background nearBackground;
    Background farBackground;

    SDL_Texture *healthTexture;

    TTF_Font *scoreFont;
    int score = 0;

public:
    MainScene(Game &game) : Scene(game) {}
    virtual void init() override;
    virtual void run(float deltaTime) override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event *event) override;
    void keyboardController(float deltaTime);
};
