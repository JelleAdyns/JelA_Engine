#ifndef STROKESTYLE_H
#define STROKESTYLE_H

#include "JelA/Misc/Defines.h"

namespace jela
{
    class RenderManager;

    class StrokeStyle final
    {
    public:

        enum class Cap : uint8_t
        {
            Flat = D2D1_CAP_STYLE_FLAT,
            Square = D2D1_CAP_STYLE_SQUARE,
            Round = D2D1_CAP_STYLE_ROUND,
            Triangle = D2D1_CAP_STYLE_TRIANGLE,
        };

        enum class Join : uint8_t
        {
            Miter = D2D1_LINE_JOIN_MITER,
            Bevel = D2D1_LINE_JOIN_BEVEL,
            Round = D2D1_LINE_JOIN_ROUND,
            MiterOrBevel = D2D1_LINE_JOIN_MITER_OR_BEVEL,
        };

        enum class Dash : uint8_t
        {
            Solid = D2D1_DASH_STYLE_SOLID,
            Dashed = D2D1_DASH_STYLE_DASH,
            Dot = D2D1_DASH_STYLE_DOT,
            DashDot = D2D1_DASH_STYLE_DASH_DOT,
            DashDotDot = D2D1_DASH_STYLE_DASH_DOT_DOT,
            Custom = D2D1_DASH_STYLE_CUSTOM,
        };
        enum class TransformStyle : uint8_t
        {
            Normal = D2D1_STROKE_TRANSFORM_TYPE_NORMAL,
            Fixed = D2D1_STROKE_TRANSFORM_TYPE_FIXED,
            Hairline = D2D1_STROKE_TRANSFORM_TYPE_HAIRLINE,
        };

        StrokeStyle();
        explicit StrokeStyle(
            Dash dashStyle,
            Cap startCap = Cap::Flat,
            Cap endCap = Cap::Flat,
            Cap dashCap = Cap::Flat,
            Join joinStyle = Join::Miter,
            TransformStyle transformStyle = TransformStyle::Normal,
            float miterLimit = 10.f,
            float dashOffset = 0.f
            );
        explicit StrokeStyle(
            Dash dashStyle,
            TransformStyle transformStyle = TransformStyle::Normal,
            Join joinStyle = Join::Miter,
            Cap startCap = Cap::Flat,
            Cap endCap = Cap::Flat,
            Cap dashCap = Cap::Flat,
            float miterLimit = 10.f,
            float dashOffset = 0.f
            );
        ~StrokeStyle();

        StrokeStyle(const StrokeStyle& other);
        StrokeStyle(StrokeStyle&& other) noexcept;
        StrokeStyle& operator=(const StrokeStyle& other);
        StrokeStyle& operator=(StrokeStyle&& other) noexcept;

        void swap(StrokeStyle& other) noexcept;

        void SetStartCap(Cap startCap);
        void SetEndCap(Cap endCap);
        void SetCapStyle(Cap dashCap);
        void SetJoinStyle(Join joinStyle);
        void SetDashStyle(Dash dash);
        void SetTransformStyle(TransformStyle transformStyle);
        void SetMiterLimit(float miterLimit);
        void SetDashOffset(float dashOffset);
        void SetCustomDashes(const std::vector<float>& values);

        Cap GetStartCapStyle() const;
        Cap GetEndCapStyle() const;
        Cap GetCapStyle() const;
        Join GetJoinStyle() const;
        Dash GetDashStyle() const;
        TransformStyle GetTransformStyle() const;
        float GetMiterLimit() const;
        float GetDashOffset() const;
        std::vector<float> GetCustomDashes() const;

        class RendererDoor
        {
            friend RenderManager;
            static ID2D1StrokeStyle1* GetStyle(const StrokeStyle& style);
        };

        void UpdateState();
    private:
        ID2D1StrokeStyle1* GetStyle() const;
        bool CreateStrokeStyle();
        void CopyDashesBuffer(const float* dashesToCopy, std::uint8_t size);

        ID2D1StrokeStyle1* m_pStrokeStyle{};

        std::unique_ptr<const float[]> m_pCustomDashes {nullptr};
        std::uint8_t m_CustomDashesCount{0};

        bool m_NeedsToChange{false};

        Cap m_StartCap{Cap::Flat};
        Cap m_EndCap{Cap::Flat};
        Cap m_DashCap{Cap::Flat};
        Join m_LineJoin {Join::Miter};
        Dash m_Style {Dash::Solid};
        TransformStyle m_Transform {TransformStyle::Normal};
        float m_MiterLimit {10.f};
        float m_DashOffset {0.f};
    };
}
#endif //STROKESTYLE_H
