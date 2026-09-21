#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H
#include "Component.h"
#include "Structs.h"
#include "Scene.h"

namespace jela
{
    class Transform : public Component
    {
    public:
        static constexpr std::size_t MAX_AMOUNT = Scene::GetMaxObjects();

        Transform() = default;
        Transform(Vector2f position, float rotation, Vector2f scale);
        Transform(Vector2f position, float rotation, float scaleX, float scaleY);
        Transform(Vector2f position, float rotation, float scale);
        Transform(Vector2f position, float rotation);
        explicit Transform(Vector2f position);
        Transform(float positionX, float positionY, float rotation, float scaleX, float scaleY);
        Transform(float positionX, float positionY, float rotation, float scale);
        Transform(float positionX, float positionY, float rotation);
        Transform(float positionX, float positionY);
        Transform(float positionX, float positionY, float rotation, Vector2f scale);
        Transform(float positionX, float positionY, Vector2f scale);
        Transform(float rotation, Vector2f scale);

        Vector2f Position() const { return m_LocalTransform.position; };
        float Rotation() const { return m_LocalTransform.rotation; };
        Vector2f Scale() const { return m_LocalTransform.scale; };

        virtual void SetWorldPos(float x, float y);
        virtual void SetWorldPos(Vector2f newWorldPos);
        virtual void SetLocalPos(float x, float y);
        virtual void SetLocalPos(Vector2f newLocalPos);
        void SetLocalRot(float angle);
        void SetLocalScale(float scale);
        void SetLocalScale(float scaleX, float scaleY);
        void SetLocalScale(Vector2f newLocalPos);
        void SetTransformDirty();
        Vector2f WorldPosition();
        float WorldRotation();
        Vector2f WorldScale();

    private:

        struct TRS
        {
            Vector2f position{};
            float rotation{};
            Vector2f scale{1.f, 1.f};
        };

        TRS m_LocalTransform{};
        TRS m_WorldTransform{};

        bool m_IsTransformDirty{false};

        void UpdateWorldTransform();

    };
} // jela

#endif //TRANSFORMCOMPONENT_H
