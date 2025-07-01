#ifndef GAME_H
#define GAME_H

#include "Engine.h"
#include "BaseGame.h"

class Game final: public jela::BaseGame
{
public:
    Game() = default;
    virtual ~Game() = default;

    Game(const Game& other) = delete;
    Game(Game&& other) noexcept = delete;
    Game& operator=(const Game& other) = delete;
    Game& operator=(Game&& other) noexcept = delete;

    virtual void Initialize() override;
    virtual void Cleanup() override;
    virtual void Draw() const override;
    virtual void Tick() override;
    virtual void KeyDown(int virtualKeycode) override;
    virtual void KeyDownThisFrame(int virtualKeycode) override;
    virtual void KeyUp(int virtualKeycode) override;
    virtual void HandleControllerInput() override;
    virtual void MouseDown(bool isLeft, int x, int y) override;
    virtual void MouseUp(bool isLeft, int x, int y) override;
    virtual void MouseMove(int x, int y, int keyDown) override;
    virtual void MouseWheelTurn(int x, int y, int turnDistance, int keyDown) override;
private:

    class TestTexture
    {
    public:
        TestTexture()
        {
            ENGINE.ResourceMngr()->GetTexture(_T("NES - Bubble Bobble - Level Tiles.png"), m_Texture);
            ENGINE.ResourceMngr()->GetTexture(_T("Wesley.png"), m_Texture);
           
        }
        void Draw() const
        {
            ENGINE.DrawTexture(m_Texture.pObject , 50,0);
        }
    private:
        jela::ReferencePtr<jela::Texture> m_Texture { nullptr };
        //std::unique_ptr<jela::Texture*, jela::ResourceManager::ReferenceDeleter<jela::Texture>> m_refTexture { nullptr };
        //jela::Texture* m_Texture { nullptr };
    };

    TestTexture* m_testTexture{nullptr};
    jela::ReferencePtr<jela::Texture> m_Texture{ nullptr};
    jela::ReferencePtr<jela::Font> m_Font{ nullptr};
    jela::ReferencePtr<jela::Font> m_Font2{ nullptr};
    tstring mouseText{};
    //const jela::Texture& m_Texture{ jela::ResourceManager::GetInstance().GetTexture(_T("Wesley.png"))};
    std::unique_ptr<jela::TextFormat> pTextFormat{ std::make_unique<jela::TextFormat>(8,false, false, jela::TextFormat::HorAllignment::Center,jela::TextFormat::VertAllignment::Center) };
};

#endif // !GAME_H