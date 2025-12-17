#include "Scene.h"

class EndScene : public Scene
{
private:
public:
    EndScene(Game &game) : Scene(game) {}
    virtual void init() override;
    virtual void run(float deltaTime) override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event *event) override;
};
