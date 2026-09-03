#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H
#include "Component.h"
#include "Structs.h"
#include "Scene.h"

namespace jela
{
    class TransformComponent final : public Component
    {
    public:
        static constexpr std::size_t MAX_AMOUNT = Scene::GetMaxObjects();

        TransformComponent() = default;
        TransformComponent(Vector2f position, float rotation, Vector2f scale);
        TransformComponent(Vector2f position, float rotation, float scaleX, float scaleY);
        TransformComponent(Vector2f position, float rotation, float scale);
        TransformComponent(Vector2f position, float rotation);
        explicit TransformComponent(Vector2f position);
        TransformComponent(float positionX, float positionY, float rotation, float scaleX, float scaleY);
        TransformComponent(float positionX, float positionY, float rotation, float scale);
        TransformComponent(float positionX, float positionY, float rotation);
        TransformComponent(float positionX, float positionY);
        TransformComponent(float positionX, float positionY, float rotation, Vector2f scale);
        TransformComponent(float positionX, float positionY, Vector2f scale);
        TransformComponent(float rotation, Vector2f scale);

        void Start() override {};
        void Update() override {};

        Vector2f Position() const { return m_LocalTransform.position; };
        float Rotation() const { return m_LocalTransform.rotation; };
        Vector2f Scale() const { return m_LocalTransform.scale; };

        void SetLocalPos(float x, float y);
        void SetLocalPos(Vector2f newLocalPos);
        void SetPosDirty();
        Vector2f WorldPosition();
    private:

        struct Transform
        {
            Vector2f position{};
            float rotation{};
            Vector2f scale{1.f, 1.f};
        };

        Transform m_LocalTransform{};
        Transform m_WorldTransform{};
        bool m_IsPosDirty{false};

        void UpdateWorldPosition();
    };
} // jela

#endif //TRANSFORMCOMPONENT_H
