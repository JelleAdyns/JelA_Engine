#ifndef COMMAND_H
#define COMMAND_H

namespace jela
{
    class GameObject;

    class Command
    {
    public:
        virtual void Execute() const = 0;
    protected:
        ~Command() = default;
    };

    class GameObjectCommand : public Command
    {
    public:
        virtual ~GameObjectCommand() = default;

        GameObjectCommand(const GameObjectCommand&) = default;
        GameObjectCommand(GameObjectCommand&&) noexcept = default;
        GameObjectCommand& operator= (const GameObjectCommand&) = default;
        GameObjectCommand& operator= (GameObjectCommand&&) noexcept = default;
    protected:
        GameObjectCommand(GameObject* pGameObject) :
            m_pGameObject{ pGameObject }
        {}
        GameObject* GetGameObject() const { return m_pGameObject; }
    private:
        GameObject* m_pGameObject;
    };
}

#endif //COMMAND_H
