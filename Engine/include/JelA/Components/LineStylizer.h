#ifndef LINESTYLIZER_H
#define LINESTYLIZER_H
#include "Component.h"
#include "JelA/StrokeStyle.h"

namespace jela
{
    class LineStylizer final : public Component
    {

        using Cap = StrokeStyle::Cap;
        using Join = StrokeStyle::Join;
        using Dash = StrokeStyle::Dash;
        using TransformStyle = StrokeStyle::TransformStyle;

    public:
        const StrokeStyle& GetStrokeStyle() const;

        void SetOuterCapStyles(Cap startCap, Cap endCap);
        void SetOuterCapStyles(Cap capStyle);
        void SetDashCapStyle(Cap dashCapStyle);
        void SetAllCapStyles(Cap capStyle);
        void SetJoinStyle(Join joinStyle);
        void SetDashStyle(Dash dash);
        void SetTransformStyle(TransformStyle transformStyle);
        void SetStyle(Dash dashStyle, Join joinStyle, TransformStyle transformStyle = TransformStyle::Normal);
        void SetStyle(Cap allCapStyles, Dash dashStyle, Join joinStyle = Join::Miter, TransformStyle transformStyle = TransformStyle::Normal);
        void SetStyle(Cap startCap, Cap endCap, Cap capStyle, Dash dashStyle, Join joinStyle = Join::Miter, TransformStyle transformStyle = TransformStyle::Normal);
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
    private:

        StrokeStyle m_StrokeStyle{};
    };

}

#endif //LINESTYLIZER_H
