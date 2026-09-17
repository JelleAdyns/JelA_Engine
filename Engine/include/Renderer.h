#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H
#include "Component.h"

namespace jela
{
    class Renderer : public Component
    {
    public:
        virtual void Draw() const = 0;

        Renderer() = default;
        ~Renderer() override = default;
        Renderer(const Renderer& other) = default;
        Renderer(Renderer&& other) noexcept = default;
        Renderer& operator=(const Renderer& other) = default;
        Renderer& operator=(Renderer&& other) noexcept = default;
    };
}


#endif //RENDERCOMPONENT_H
