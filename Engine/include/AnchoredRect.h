#ifndef ANCHOREDRECT_H
#define ANCHOREDRECT_H
#include "Component.h"
#include "Structs.h"

namespace jela
{

    class Transform;
    class AnchoredRect final : public Component
    {
    public:

        enum class Anchor : uint8_t
        {
            Left,Right,Up,Down,
            LeftUp,RightUp,
            LeftDown,RightDown,
            Center
        };
        AnchoredRect(Vector2f size);
        AnchoredRect(float width, float height);


        void SetAnchor(Anchor);
        void SetPivot(Anchor);
        void SetPivot(float xMapping, float yMapping);
        void SetPivot(Vector2f mapping);
        void SetRectSize(float width, float height);
        void SetRectSize(Vector2f);

        Rectf GetRect();

    private:
        void UpdatePos();
        static bool IsLeft(Anchor);
        static bool IsRight(Anchor);
        static bool IsUp(Anchor);
        static bool IsDown(Anchor);
        Vector2f m_Pos{};
        Vector2f m_Size{};

        // is pos within rect size
        Vector2f m_Pivot{};
        Anchor m_CurrAnchor{Anchor::Center};

        // local transform is pos from anchor to pivot
    };

} // jela

#endif //ANCHOREDRECT_H
