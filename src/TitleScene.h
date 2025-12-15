#include "SDL_mixer.h"
#include "SDL_ttf.h"

#include "Scene.h"

class TitleScene : public Scene
{
private:
    Mix_Music *music;

    TTF_Font *titleFont;
    TTF_Font *textFont;

public:
    TitleScene(Game &game) : Scene(game) {}
    virtual void init() override;
    virtual void run(float deltaTime) override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event *event) override;
};
