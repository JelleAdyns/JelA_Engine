#include "AnchoredRect.h"

#include "Engine.h"
#include "GameObject.h"
#include "Transform.h"

namespace jela
{
    AnchoredRect::AnchoredRect(Vector2f size):
        AnchoredRect{size.x,size.y}
    {}
    AnchoredRect::AnchoredRect(float width, float height):
        m_Size{width,height}
    {
    }
    void AnchoredRect::SetAnchor(Anchor anchor)
    {
        m_CurrAnchor = anchor;
        UpdatePos();
    }
    void AnchoredRect::SetPivot(Anchor pivot)
    {
        switch (pivot)
        {
        case Anchor::Left: m_Pivot = {0.f,0.5f}; break;
        case Anchor::Right: m_Pivot = {1.f,0.5f}; break;
        case Anchor::Up: m_Pivot = {0.5f,1.f}; break;
        case Anchor::Down: m_Pivot = {0.5f,0.f}; break;
        case Anchor::LeftUp: m_Pivot = {0.f,1.f}; break;
        case Anchor::RightUp: m_Pivot = {1.f,1.f}; break;
        case Anchor::LeftDown: m_Pivot = {0.f,0.f}; break;
        case Anchor::RightDown: m_Pivot = {1.f,0.f}; break;
        case Anchor::Center: m_Pivot = {0.5f,0.5f}; break;
        }

        UpdatePos();
    }
    void AnchoredRect::SetPivot(float xMapping, float yMapping)
    {
        SetPivot({xMapping, yMapping});
    }
    void AnchoredRect::SetPivot(Vector2f mapping)
    {
        m_Pivot = mapping;
        UpdatePos();
    }
    void AnchoredRect::SetRectSize(float width, float height)
    {
        SetRectSize({width, height});
    }
    void AnchoredRect::SetRectSize(Vector2f size)
    {
        m_Size.x = size.x;
        m_Size.y = size.y;
        UpdatePos();
    }
    Rectf AnchoredRect::GetRect()
    {
        UpdatePos();
        return Rectf{m_Pos,m_Size.x,m_Size.y};
    }
    void AnchoredRect::UpdatePos()
    {
        Vector2f pivot = -m_Pivot;
        if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM == false)
            pivot.y = 1 - m_Pivot.y;
        m_Pos = GetOwner()->Transform()->Position() + pivot * m_Size;
    }
    bool AnchoredRect::IsLeft(Anchor anchor)
    {
        return anchor == Anchor::Left || anchor == Anchor::LeftUp || anchor == Anchor::LeftDown;
    }
    bool AnchoredRect::IsRight(Anchor anchor)
    {
        return anchor == Anchor::Right || anchor == Anchor::RightUp || anchor == Anchor::RightDown;
    }
    bool AnchoredRect::IsUp(Anchor anchor)
    {
        return anchor == Anchor::Up || anchor == Anchor::RightUp || anchor == Anchor::LeftUp;
    }
    bool AnchoredRect::IsDown(Anchor anchor)
    {
        return anchor == Anchor::Down || anchor == Anchor::RightDown || anchor == Anchor::LeftDown;
    }
} // jela