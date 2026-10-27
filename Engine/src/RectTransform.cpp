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
    void RectTransform::SetWorldPos(const Vector2f& newWorldPos)
    {
        const Vector2f anchorOffset = Position() - m_AnchoredPosition;
        Transform::SetWorldPos(newWorldPos);
        m_AnchoredPosition = Position() - anchorOffset;
    }
    void RectTransform::SetLocalPos(float x, float y)
    {
        SetLocalPos({x,y});
    }
    void RectTransform::SetLocalPos(const Vector2f& newLocalPos)
    {
        const Vector2f anchorOffset = Position() - m_AnchoredPosition;
        Transform::SetLocalPos(newLocalPos);
        m_AnchoredPosition = Position() - anchorOffset;
    }
    void RectTransform::SetAnchoredPos(float x, float y)
    {
        SetAnchoredPos({x,y});
    }
    void RectTransform::SetAnchoredPos(const Vector2f& newAnchoredPos)
    {
        m_AnchoredPosition = newAnchoredPos;
    }

    void RectTransform::SetAnchor(Anchor anchor)
    {
        SetAnchor(GetAnchorMapping(anchor));
    }
    void RectTransform::SetAnchor(float xMapping, float yMapping)
    {
        SetAnchor({xMapping, yMapping});
    }
    void RectTransform::SetAnchor(const Vector2f& mapping)
    {
        const Vector2f prevOffset = GetAnchorOffset(m_Anchor);
        m_Anchor = mapping;
        const Vector2f newOffset = GetAnchorOffset(m_Anchor);
        Vector2f anchorDifference = (newOffset - prevOffset);

        SetAnchoredPos(m_AnchoredPosition - anchorDifference);
        UpdateRectPos();
        SetTransformDirty();
    }

    void RectTransform::SetPivot(Anchor pivot)
    {
        SetPivot(GetAnchorMapping(pivot));
    }
    void RectTransform::SetPivot(float xMapping, float yMapping)
    {
        SetPivot({xMapping, yMapping});
    }
    void RectTransform::SetPivot(const Vector2f& mapping)
    {
        const Vector2f prevMap = (m_Pivot - Vector2f{0.5,0.5}).Scale(m_Size);
        m_Pivot = mapping;
        const Vector2f newMap = (m_Pivot - Vector2f{0.5,0.5}).Scale(m_Size);
        Vector2f pivotDifference = (newMap - prevMap);

        SetLocalPos(Position() + pivotDifference);
        for (const auto pChild : GetOwner()->Children())
        {
            auto* pTransform = pChild->GetComponent<Transform>();
            pTransform->SetLocalPos(pTransform->Position() - pivotDifference);
        }
        UpdateRectPos();
        SetTransformDirty();
    }

    void RectTransform::SetRectSize(float width, float height)
    {
        SetRectSize({width, height});
    }
    void RectTransform::SetRectSize(const Vector2f& size)
    {
        m_Size.x = size.x;
        m_Size.y = size.y;
        SetPivot(m_Pivot);
    }
    Vector2f RectTransform::AnchoredPos() const
    {
        return m_AnchoredPosition;
    }
    Vector2f RectTransform::GetSize() const
    {
        return m_Size;
    }
    Vector2f RectTransform::GetRectPos()
    {
        UpdateRectPos();
        return m_RectPos;
    }
    Rectf RectTransform::GetRect()
    {
        UpdateRectPos();
        return Rectf{m_RectPos,m_Size.x,m_Size.y};
    }

    Vector2f RectTransform::GetAnchorOffset(const Vector2f& anchor) const
    {
        if (const auto pParent = GetOwner()->Parent();
           pParent)
        {
            if (const auto rectTransform = pParent->GetComponent<RectTransform>(); rectTransform)
                return (anchor - Vector2f{0.5,0.5}).Scale(rectTransform->GetSize());
        }

        return {};
    }
    void RectTransform::UpdateRectPos()
    {
        Vector2f pivot = m_Pivot;
        m_RectPos = - pivot.Scale(m_Size);
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