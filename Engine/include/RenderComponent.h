#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H
#include "Component.h"

namespace jela
{
    class RenderComponent : public Component
    {
    public:
        virtual void Draw() const = 0;

        RenderComponent();
        ~RenderComponent() override = default;
        RenderComponent(const RenderComponent& other) = default;
        RenderComponent(RenderComponent&& other) noexcept = default;
        RenderComponent& operator=(const RenderComponent& other) = default;
        RenderComponent& operator=(RenderComponent&& other) noexcept = default;
    };
}


#endif //RENDERCOMPONENT_H
