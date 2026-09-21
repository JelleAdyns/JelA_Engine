#include "RectTransform.h"

#include "Engine.h"
#include "GameObject.h"
#include "Transform.h"

namespace jela
{
    RectTransform::RectTransform(Vector2f size):
        RectTransform{size.x,size.y}
    {}
    RectTransform::RectTransform(float width, float height):
        m_Size{width,height}
    {
        m_Pivot = GetAnchorMapping(Anchor::Center);
        m_Anchor = GetAnchorMapping(Anchor::Center);
    }
    void RectTransform::Start()
    {
        SetAnchor(m_Anchor);
        SetPivot(m_Pivot);
    }

    void RectTransform::SetWorldPos(float x, float y)
    {
        SetWorldPos({x,y});
    }
    void RectTransform::SetWorldPos(Vector2f newWorldPos)
    {
        const Vector2f anchorOffset = Position() - m_AnchoredPosition;
        Transform::SetWorldPos(newWorldPos);
        m_AnchoredPosition = Position() - anchorOffset;
    }
    void RectTransform::SetLocalPos(float x, float y)
    {
        SetLocalPos({x,y});
    }
    void RectTransform::SetLocalPos(Vector2f newLocalPos)
    {
        const Vector2f anchorOffset = Position() - m_AnchoredPosition;
        Transform::SetLocalPos(newLocalPos);
        m_AnchoredPosition = Position() - anchorOffset;
    }
    void RectTransform::SetAnchoredPos(float x, float y)
    {
        SetAnchoredPos({x,y});
    }
    void RectTransform::SetAnchoredPos(Vector2f newAnchoredPos)
    {
        const Vector2f prevPos = m_AnchoredPosition;
        m_AnchoredPosition = newAnchoredPos;
        //Transform::SetLocalPos(Position() + (newAnchoredPos - prevPos));
    }

    void RectTransform::SetAnchor(Anchor anchor)
    {
        SetAnchor(GetAnchorMapping(anchor));
    }
    void RectTransform::SetAnchor(float xMapping, float yMapping)
    {
        SetAnchor({xMapping, yMapping});
    }
    void RectTransform::SetAnchor(Vector2f mapping)
    {
        const Vector2f prevOffset = GetAnchorOffset(m_Anchor);
        m_Anchor = mapping;
        const Vector2f newOffset = GetAnchorOffset(m_Anchor);
        SetAnchoredPos(m_AnchoredPosition - (newOffset - prevOffset));
        UpdateRectPos();
    }

    void RectTransform::SetPivot(Anchor pivot)
    {
        SetPivot(GetAnchorMapping(pivot));
    }
    void RectTransform::SetPivot(float xMapping, float yMapping)
    {
        SetPivot({xMapping, yMapping});
    }
    void RectTransform::SetPivot(Vector2f mapping)
    {
        const Vector2f prevMap = (m_Pivot - Vector2f{0.5,0.5}) * m_Size;
        m_Pivot = mapping;
        const Vector2f newMap = (m_Pivot - Vector2f{0.5,0.5}) * m_Size;
        const Vector2f pivotDifference = (newMap - prevMap);
        SetLocalPos(Position() + pivotDifference);
        for (const auto pChild : GetOwner()->Children())
        {
            auto* pTransform = pChild->GetComponent<Transform>();
            pTransform->SetLocalPos(pTransform->Position() - pivotDifference);
        }
        UpdateRectPos();

    }

    void RectTransform::SetRectSize(float width, float height)
    {
        SetRectSize({width, height});
    }
    void RectTransform::SetRectSize(Vector2f size)
    {
        m_Size.x = size.x;
        m_Size.y = size.y;
        UpdateRectPos();
    }
    Vector2f RectTransform::AnchoredPos() const
    {
        return m_AnchoredPosition;
    }
    Vector2f RectTransform::GetSize() const
    {
        return m_Size;
    }
    Rectf RectTransform::GetRect()
    {
        UpdateRectPos();
        return Rectf{m_RectPos,m_Size.x,m_Size.y};
    }
    Vector2f RectTransform::GetAnchorOffset(Vector2f anchor) const
    {
        if (const auto pParent = GetOwner()->Parent();
           pParent)
        {
            if (const auto rectTransform = pParent->GetComponent<RectTransform>(); rectTransform)
                return (anchor - Vector2f{0.5,0.5}) * rectTransform->GetSize();
        }

        return {};
    }
    void RectTransform::UpdateRectPos()
    {
        Vector2f pivot = m_Pivot;
        if constexpr (USE_MATHEMATICAL_COORDINATESYSTEM == false) pivot.y = 1-pivot.y;
        m_RectPos = Position() - pivot * m_Size;
    }
    constexpr Vector2f RectTransform::GetAnchorMapping(Anchor anchor)
    {
        assert((anchor != Anchor::SIZE));
        return m_AnchorMappings[static_cast<int>(anchor)];
    }
    constexpr bool RectTransform::IsLeft(Anchor anchor)
    {
        return anchor == Anchor::Left || anchor == Anchor::LeftUp || anchor == Anchor::LeftDown;
    }
    constexpr bool RectTransform::IsRight(Anchor anchor)
    {
        return anchor == Anchor::Right || anchor == Anchor::RightUp || anchor == Anchor::RightDown;
    }
    constexpr bool RectTransform::IsUp(Anchor anchor)
    {
        return anchor == Anchor::Up || anchor == Anchor::RightUp || anchor == Anchor::LeftUp;
    }
    constexpr bool RectTransform::IsDown(Anchor anchor)
    {
        return anchor == Anchor::Down || anchor == Anchor::RightDown || anchor == Anchor::LeftDown;
    }
} // jela