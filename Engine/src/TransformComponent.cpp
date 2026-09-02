#include "../include/TransformComponent.h"

namespace jela
{
    TransformComponent::TransformComponent(Vector2f position, float rotation, Vector2f scale):
        m_LocalTransform{position, rotation, scale}
    {}
    TransformComponent::TransformComponent(Vector2f position, float rotation, float scaleX, float scaleY):
            TransformComponent{position, rotation, {scaleX,scaleY}}
    {}
    TransformComponent::TransformComponent(Vector2f position, float rotation, float scale):
            TransformComponent{position, rotation, {scale,scale}}
    {}
    TransformComponent::TransformComponent(Vector2f position, float rotation):
        TransformComponent{position, rotation, {1.f,1.f}}
    {}
    TransformComponent::TransformComponent(Vector2f position):
        TransformComponent{position, 0.f, {1.f,1.f}}
    {}
    TransformComponent::TransformComponent(float positionX, float positionY, float rotation, float scaleX, float scaleY):
        TransformComponent{{positionX, positionY}, rotation, {scaleX,scaleY}}
    {}
    TransformComponent::TransformComponent(float positionX, float positionY, float rotation, float scale):
        TransformComponent{{positionX, positionY}, rotation, {scale,scale}}
    {}
    TransformComponent::TransformComponent(float positionX, float positionY, float rotation):
        TransformComponent{{positionX, positionY}, rotation, {1.f,1.f}}
    {}
    TransformComponent::TransformComponent(float positionX, float positionY):
        TransformComponent{{positionX, positionY}, 0.f, {1.f,1.f}}
    {}
    TransformComponent::TransformComponent(float positionX, float positionY, float rotation, Vector2f scale):
        TransformComponent{{positionX, positionY}, rotation, scale}
    {}
    TransformComponent::TransformComponent(float positionX, float positionY, Vector2f scale):
        TransformComponent{{positionX, positionY}, 0.f, scale}
    {}
    TransformComponent::TransformComponent(float rotation, Vector2f scale):
        TransformComponent{{}, rotation, scale}
    {}
    void TransformComponent::SetLocalPos(float x, float y)
    {
        SetLocalPos({x,y});
    }
    void TransformComponent::SetLocalPos(Vector2f newLocalPos)
    {
        m_LocalTransform.position = newLocalPos;
        SetPosDirty();
    }
    void TransformComponent::SetPosDirty()
    {
        m_IsPosDirty = true;
        std::ranges::for_each(GetOwner()->Children(), [](const GameObject* child){ child->Transform()->SetPosDirty(); });
    }
    Vector2f TransformComponent::WorldPosition()
    {
        if (m_IsPosDirty) UpdateWorldPosition();
        return m_WorldTransform.position;
    }
    void TransformComponent::UpdateWorldPosition()
    {
        const GameObject* parent = GetOwner()->Parent();

        m_WorldTransform.position = m_LocalTransform.position + (parent ? parent->Transform()->WorldPosition() : Vector2f{});
        m_IsPosDirty = false;
    }
} // jela