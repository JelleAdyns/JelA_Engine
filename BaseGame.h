#ifndef BASEGAME_H
#define BASEGAME_H

class Texture;
class BaseGame
{
public:
    BaseGame() {}
    virtual ~BaseGame() {};

    BaseGame(const BaseGame& other) = delete;
    BaseGame(BaseGame&& other) noexcept = delete;
    BaseGame& operator=(const BaseGame& other) = delete;
    BaseGame& operator=(BaseGame&& other) noexcept = delete;

	virtual void Initialize() = 0;
	virtual void Draw() = 0;
	virtual void Tick(float elapsedSec) = 0;
};

#endif // !BASEGAME_H