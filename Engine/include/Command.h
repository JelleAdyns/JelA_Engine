#ifndef COMMAND_H
#define COMMAND_H

namespace jela
{
    class GameObject;

    class Command
    {
    public:
        Command() = default;
        virtual ~Command() = default;
        Command(const Command&) = default;
        Command(Command&&) noexcept = default;
        Command& operator= (const Command&) = default;
        Command& operator= (Command&&) noexcept = default;
        virtual void Execute() const = 0;
    };

    class GameObjectCommand : public Command
    {
    public:
        ~GameObjectCommand() override = default;

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
