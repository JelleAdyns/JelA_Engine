#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H
#include "Component.h"
#include "Engine.h"

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

    class ShapeRenderer : public Renderer
    {
    public:
        void SetLineThickness(float lineThickness) {m_LineThickness = std::max(1.f,lineThickness);}
        float GetLineThickness() const {return m_LineThickness;}
        void SetColor(Color color) { m_Color = color; }
        Color GetColor() const { return m_Color; }

        void Draw() const override = 0;
    protected:
        float m_LineThickness{1.f};
        Color m_Color{Color::WHITE};
    };

    template <typename T>
    concept cIsShape = cAnyOf<T, Rectf, RoundedRectf, Ellipsef, Circlef>;

    template <cIsShape Primitive>
    class PrimitiveRenderer : public ShapeRenderer
    {
    public:

        PrimitiveRenderer() = default;
        explicit PrimitiveRenderer(const Primitive& primitive): m_Primitive{primitive} {}

        void SetOffset(const Vector2f& offset)
        {
            if constexpr (std::is_same_v<Primitive, Rectf> || std::is_same_v<Primitive, RoundedRectf>)
                m_Primitive.SetPos(offset);
            else if constexpr (std::is_same_v<Primitive, Ellipsef> || std::is_same_v<Primitive, Circlef>)
            {
                m_Primitive.center.x = offset.x;
                m_Primitive.center.y = offset.y;
            }
        }
        void SetWidth(float width)
        {
            width = std::max(1.f, width);

            if constexpr (std::is_same_v<Primitive, Rectf> || std::is_same_v<Primitive, RoundedRectf>) m_Primitive.width = width;
            else if constexpr (std::is_same_v<Primitive, Ellipsef>) m_Primitive.radiusX = width/2;
            else if constexpr (std::is_same_v<Primitive, Circlef>) m_Primitive.rad = width/2;
        }
        void SetHeight(float height)
        {
            height = std::max(1.f, height);

            if constexpr (std::is_same_v<Primitive, Rectf> || std::is_same_v<Primitive, RoundedRectf>) m_Primitive.height = height;
            else if constexpr (std::is_same_v<Primitive, Ellipsef>) m_Primitive.radiusY = height/2;
            else if constexpr (std::is_same_v<Primitive, Circlef>) m_Primitive.rad = height/2;
        }

        void SetPrimitive(const Primitive& primitive) const {m_Primitive = primitive;}
        void SetFill(bool fill) {m_Fill = fill;}

        const Primitive& GetPrimitive() const {return m_Primitive;}
        bool IsFilled() const {return m_Fill;}

        void Draw() const override = 0;
    protected:
        Primitive m_Primitive{};
        bool m_Fill{false};
    };
}


#endif //RENDERCOMPONENT_H
