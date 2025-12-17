#include "SDL_mixer.h"

#include "Scene.h"

class TitleScene : public Scene
{
private:
    Mix_Music *music;

public:
    TitleScene(Game &game) : Scene(game) {}
    virtual void init() override;
    virtual void run(float deltaTime) override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event *event) override;
};
