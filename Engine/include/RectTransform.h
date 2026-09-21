#ifndef ANCHOREDRECT_H
#define ANCHOREDRECT_H
#include "Component.h"
#include "Structs.h"
#include "Transform.h"

namespace jela
{

    class Transform;
    class RectTransform final : public Transform
    {
    public:

        enum class Anchor : uint8_t
        {
            Left,Right,Up,Down,
            LeftUp,RightUp,
            LeftDown,RightDown,
            Center,
            SIZE
        };
        RectTransform(Vector2f size);
        RectTransform(float width, float height);

        void Start() override;
        void SetWorldPos(float x, float y) override;
        void SetWorldPos(Vector2f newWorldPos) override;
        void SetLocalPos(float x, float y) override;
        void SetLocalPos(Vector2f newLocalPos) override;
        void SetAnchoredPos(float x, float y);
        void SetAnchoredPos(Vector2f newAnchoredPos);

        void SetAnchor(Anchor);
        void SetAnchor(float xMapping, float yMapping);
        void SetAnchor(Vector2f mapping);
        void SetPivot(Anchor);
        void SetPivot(float xMapping, float yMapping);
        void SetPivot(Vector2f mapping);
        void SetRectSize(float width, float height);
        void SetRectSize(Vector2f);

        Vector2f AnchoredPos() const;
        Vector2f GetSize() const;
        Rectf GetRect();

    private:
        Vector2f GetAnchorOffset(Vector2f anchor) const;
        void UpdateRectPos();
        constexpr static Vector2f GetAnchorMapping(Anchor);
        constexpr static bool IsLeft(Anchor);
        constexpr static bool IsRight(Anchor);
        constexpr static bool IsUp(Anchor);
        constexpr static bool IsDown(Anchor);

        static constexpr std::array<Vector2f, static_cast<std::size_t>(Anchor::SIZE)> m_AnchorMappings
        {
            Vector2f{0.f,0.5f},
            Vector2f{1.f,0.5f},
            Vector2f{0.5f,1.f},
            Vector2f{0.5f,0.f},
            Vector2f{0.f,1.f},
            Vector2f{1.f,1.f},
            Vector2f{0.f,0.f},
            Vector2f{1.f,0.f},
            Vector2f{0.5f,0.5f},
        };
        Vector2f m_RectPos{};
        Vector2f m_Size{};

        Vector2f m_Pivot{};
        Vector2f m_Anchor{};
        Vector2f m_AnchoredPosition{};
    };

} // jela

#endif //ANCHOREDRECT_H
