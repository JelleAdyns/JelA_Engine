#include "Transform.h"
#include "GameObject.h"
#include "Defines.h"

namespace jela
{
    Transform::Transform(Vector2f position, float rotation, Vector2f scale):
        m_WorldTransform{},
        m_LocalTransform{position, rotation, scale}
    {}
    Transform::Transform(Vector2f position, float rotation, float scaleX, float scaleY):
        Transform{position, rotation, {scaleX,scaleY}}
    {}
    Transform::Transform(Vector2f position, float rotation, float scale):
        Transform{position, rotation, {scale,scale}}
    {}
    Transform::Transform(Vector2f position, float rotation):
        Transform{position, rotation, {1.f,1.f}}
    {}
    Transform::Transform(Vector2f position):
        Transform{position, 0.f, {1.f,1.f}}
    {}
    Transform::Transform(float positionX, float positionY, float rotation, float scaleX, float scaleY):
        Transform{{positionX, positionY}, rotation, {scaleX,scaleY}}
    {}
    Transform::Transform(float positionX, float positionY, float rotation, float scale):
        Transform{{positionX, positionY}, rotation, {scale,scale}}
    {}
    Transform::Transform(float positionX, float positionY, float rotation):
        Transform{{positionX, positionY}, rotation, {1.f,1.f}}
    {}
    Transform::Transform(float positionX, float positionY):
        Transform{{positionX, positionY}, 0.f, {1.f,1.f}}
    {}
    Transform::Transform(float positionX, float positionY, float rotation, Vector2f scale):
        Transform{{positionX, positionY}, rotation, scale}
    {}
    Transform::Transform(float positionX, float positionY, Vector2f scale):
        Transform{{positionX, positionY}, 0.f, scale}
    {}
    Transform::Transform(float rotation, Vector2f scale):
        Transform{{}, rotation, scale}
    {}
    void Transform::SetWorldPos(float x, float y)
    {
        SetWorldPos({x,y});
    }
    void Transform::SetWorldPos(const Vector2f& newWorldPos)
    {
        Vector2f worldOffset = newWorldPos - WorldPosition();

        if (const GameObject* pParent = GetOwner()->Parent())
        {
            const auto parentWorld = pParent->Transform()->WorldTransform();
            const float x = Vector2f::Dot(worldOffset, parentWorld.GetXAxis() );
            const float y = Vector2f::Dot(worldOffset, parentWorld.GetYAxis() );

            worldOffset = {x, y};
        }
        m_LocalTransform.position += worldOffset;
        SetTransformDirty();
    }
    void Transform::SetLocalPos(float x, float y)
    {
        SetLocalPos({x,y});
    }
    void Transform::SetLocalPos(const Vector2f& newLocalPos)
    {
        m_LocalTransform.position = newLocalPos;
        SetTransformDirty();
    }
    void Transform::SetLocalRot(float angle)
    {
        m_LocalTransform.rotation = angle;
        SetTransformDirty();
    }
    void Transform::SetLocalScale(float scale)
    {
        SetLocalScale({scale, scale});
    }
    void Transform::SetLocalScale(float scaleX, float scaleY)
    {
        SetLocalScale({scaleX, scaleY});
    }
    void Transform::SetLocalScale(const Vector2f& newLocalPos)
    {
        m_LocalTransform.scale = newLocalPos;
        SetTransformDirty();
    }
    void Transform::SetTransformDirty()
    {
        m_IsTransformDirty = true;
        std::ranges::for_each(GetOwner()->Children(), [](const GameObject* child){ child->Transform()->SetTransformDirty(); });
    }
    Matrix3X3 Transform::LocalTransform() const
    {
        return Matrix3X3::Scale(m_LocalTransform.scale) *
            Matrix3X3::Rotation((USE_MATHEMATICAL_COORDINATESYSTEM ? 1.f : -1.f) * m_LocalTransform.rotation) *
                Matrix3X3::Translation(m_LocalTransform.position);
    }
    const Matrix3X3& Transform::WorldTransform()
    {
        if (m_IsTransformDirty) UpdateWorldTransform();
        return m_WorldTransform;
    }
    Vector2f Transform::WorldPosition()
    {
        if (m_IsTransformDirty) UpdateWorldTransform();
        return m_WorldTransform.GetTranslation();
    }
    float Transform::WorldRotation() const
    {
        float rot = m_LocalTransform.rotation;
        if (const GameObject* pParent = GetOwner()->Parent())
            rot += pParent->Transform()->WorldRotation();

        return rot;
    }
    Vector2f Transform::WorldScale()
    {
        if (m_IsTransformDirty) UpdateWorldTransform();
        return {m_WorldTransform.GetXAxis().Length(), m_WorldTransform.GetYAxis().Length()};
    }
    void Transform::UpdateWorldTransform()
    {
        m_WorldTransform = LocalTransform();

        if (const GameObject* parent = GetOwner()->Parent())
            m_WorldTransform *= parent->Transform()->WorldTransform() ;

        m_IsTransformDirty = false;
    }
} // jela