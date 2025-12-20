#include <string>

#include "Scene.h"

class EndScene : public Scene
{
private:
    bool isTyping = true;
    std::string name;
public:
    EndScene(Game &game) : Scene(game) {}
    virtual void init() override;
    virtual void run(float deltaTime) override;
    virtual void clean() override;
    virtual void handleEvent(SDL_Event *event) override;
};
