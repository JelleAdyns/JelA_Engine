#include "Transform.h"
#include "GameObject.h"

namespace jela
{
    Transform::Transform(Vector2f position, float rotation, Vector2f scale):
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
    void Transform::SetWorldPos(Vector2f newWorldPos)
    {
        m_LocalTransform.position = newWorldPos - (WorldPosition() - Position());
        SetTransformDirty();
    }
    void Transform::SetLocalPos(float x, float y)
    {
        SetLocalPos({x,y});
    }
    void Transform::SetLocalPos(Vector2f newLocalPos)
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
    void Transform::SetLocalScale(Vector2f newLocalPos)
    {
        m_LocalTransform.scale = newLocalPos;
        SetTransformDirty();
    }
    void Transform::SetTransformDirty()
    {
        m_IsTransformDirty = true;
        std::ranges::for_each(GetOwner()->Children(), [](const GameObject* child){ child->Transform()->SetTransformDirty(); });
    }
    Vector2f Transform::WorldPosition()
    {
        if (m_IsTransformDirty) UpdateWorldTransform();
        return m_WorldTransform.position;
    }
    float Transform::WorldRotation()
    {
        if (m_IsTransformDirty) UpdateWorldTransform();
        return m_WorldTransform.rotation;
    }
    Vector2f Transform::WorldScale()
    {
        if (m_IsTransformDirty) UpdateWorldTransform();
        return m_WorldTransform.scale;
    }
    void Transform::UpdateWorldTransform()
    {
        const GameObject* parent = GetOwner()->Parent();

        m_WorldTransform.position = m_LocalTransform.position + (parent ? parent->Transform()->WorldPosition() : Vector2f{});
        m_WorldTransform.rotation = m_LocalTransform.rotation; //+ (parent ? parent->Transform()->WorldRotation() : 0);
        m_WorldTransform.scale = m_LocalTransform.scale * (parent ? parent->Transform()->WorldScale() : Vector2f{1.f,1.f});
        m_IsTransformDirty = false;
    }
} // jela