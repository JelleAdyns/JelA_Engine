#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // _DEBUG

#include "JelA/Engine.h"
#include "JelA/Components/Movement.h"
#include "JelA/Components/Rotator.h"
#include "JelA/Components/TextureImage.h"
#include "JelA/Components/RectTransform.h"
#include "JelA/Components/Renderers/PrimitiveRenderers.h"
#include "JelA/Components/Sprite.h"
#include "JelA/Components/Renderers/TextRenderer.h"
#include "JelA/Components/Renderers/TextureRenderer.h"

jela::Engine ENGINE{};

class TestScene final : public jela::Scene
{
    public:
    void Load() override
    {

        auto& staticFinn = AddGameObject();
        auto& grandChildFinn = AddGameObject();
        auto& movingWesley = AddGameObject();
        auto& parentFinn = AddGameObject();
        auto& text = AddGameObject();

        const auto texture = staticFinn.AddComponent<jela::TextureImage>(_T("Finn.png"));
        staticFinn.AddComponent<jela::TextureRenderer>();
        staticFinn.AddComponent<jela::RectTransform>(texture->GetTexture()->GetSize());
        staticFinn.AddComponent<jela::TextureSetter>();
        texture->UseTextureSize();
        staticFinn.Transform()->SetLocalPos(20,60);

        auto& staticCircle = AddGameObject();
        staticCircle.AddComponent<jela::CirclefRenderer>(jela::Circlef{jela::Point2f{}, 10});
        staticCircle.SetParent(staticFinn, false);

        const auto texture1 = grandChildFinn.AddComponent<jela::TextureImage>(_T("Finn.png"));
        grandChildFinn.AddComponent<jela::TextureRenderer>();
        //grandChildFinn.AddComponent<jela::Rotator>(30.f);
        texture1->UseTextureSize();
        grandChildFinn.AddComponent<jela::RectTransform>(texture1->GetTexture()->GetSize());;
        grandChildFinn.Transform()->SetLocalPos(50,75);


        const auto textComp = text.AddComponent<jela::TextRenderer>();
        textComp->SetTextFormat(12, true, false, jela::TextFormat::HorAllignment::Center, jela::TextFormat::VertAllignment::Top);
        textComp->SetFont(_T("castlevania-3.ttf"), true);
        textComp->SetText(_T("castlevania-3.ttf"));
        textComp->SetDrawRect(true);
        text.SetParent(movingWesley, false);text.AddComponent<jela::RectTransform>(200.f,200.f);
        //movingWesley.AddComponent<jela::Rotator>(30.f);

        const auto texture2 = movingWesley.AddComponent<jela::TextureImage>(_T("Wesley.png"));
        movingWesley.AddComponent<jela::Sprite>(8,4,0.2f, true, true);

        movingWesley.AddComponent<jela::Movement>();
        movingWesley.AddComponent<jela::TextureSetter>();
        movingWesley.AddComponent<jela::RectTransform>(texture2->GetTexture()->GetSize());
        movingWesley.AddComponent<jela::TextureRenderer>();
        grandChildFinn.SetParent(movingWesley, false);
        texture2->SetSourceRectSize(texture2->GetTexture()->GetWidth() / 8, texture2->GetTexture()->GetHeight() / 4);

        auto& movingCircle = AddGameObject();
        const auto circle = movingCircle.AddComponent<jela::CirclefRenderer>(jela::Circlef{jela::Point2f{-50,0}, 10});
        circle->SetFill(true);
        circle->SetLineThickness(3.f);
        circle->SetWidth(10.f);
        movingCircle.SetParent(movingWesley, false);

        const auto texture3 = parentFinn.AddComponent<jela::TextureImage>(_T("Finn.png"));
        texture3->UseTextureSize();
        parentFinn.AddComponent<jela::TextureRenderer>();
        parentFinn.AddComponent<jela::Rotate>();
        parentFinn.AddComponent<jela::RectTransform>(texture3->GetTexture()->GetSize());
        movingWesley.SetParent(parentFinn, false);
        parentFinn.Transform()->SetLocalPos(500,200);
        //parentFinn.Transform()->SetLocalScale(0.5f,2.f);
    }
};

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE /*hPrevInstance*/,
    _In_ LPTSTR    /*lpCmdLine*/,
    _In_ int       /*nCmdShow*/)
{  
    int result{-1};

    if (ENGINE.Init(hInstance, _T("Resources/"), 16 * 40, 9 * 40, RGB(0,0,255)))
    {
        ENGINE.SceneMngr()->SetScene<TestScene>();

        result = ENGINE.Run();
    }

    ENGINE.Shutdown();

    return result;
}
