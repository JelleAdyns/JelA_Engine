#include "JelA/Components/LineStylizer.h"

namespace jela
{
    const StrokeStyle& LineStylizer::GetStrokeStyle() const
    {
        return m_StrokeStyle;
    }
    void LineStylizer::SetOuterCapStyles(Cap startCap, Cap endCap)
    {
        m_StrokeStyle.SetStartCap(startCap);
        m_StrokeStyle.SetEndCap(endCap);
    }
    void LineStylizer::SetOuterCapStyles(Cap capStyle)
    {
        m_StrokeStyle.SetStartCap(capStyle);
        m_StrokeStyle.SetEndCap(capStyle);
    }
    void LineStylizer::SetDashCapStyle(Cap dashCapStyle)
    {
        m_StrokeStyle.SetCapStyle(dashCapStyle);
    }
    void LineStylizer::SetAllCapStyles(Cap capStyle)
    {
        m_StrokeStyle.SetStartCap(capStyle);
        m_StrokeStyle.SetEndCap(capStyle);
        m_StrokeStyle.SetCapStyle(capStyle);
    }
    void LineStylizer::SetJoinStyle(Join joinStyle)
    {
        m_StrokeStyle.SetJoinStyle(joinStyle);
    }
    void LineStylizer::SetDashStyle(Dash dash)
    {
        m_StrokeStyle.SetDashStyle(dash);
    }
    void LineStylizer::SetTransformStyle(TransformStyle transformStyle)
    {
        m_StrokeStyle.SetTransformStyle(transformStyle);
    }
    void LineStylizer::SetStyle(Dash dashStyle, Join joinStyle, TransformStyle transformStyle)
    {
        SetDashStyle(dashStyle);
        SetJoinStyle(joinStyle);
        SetTransformStyle(transformStyle);
    }
    void LineStylizer::SetStyle(Cap allCapStyles, Dash dashStyle, Join joinStyle, TransformStyle transformStyle)
    {
        SetStyle(allCapStyles, allCapStyles, allCapStyles, dashStyle, joinStyle, transformStyle);
    }
    void LineStylizer::SetStyle(Cap startCap, Cap endCap, Cap capStyle, Dash dashStyle, Join joinStyle, TransformStyle transformStyle)
    {
        SetOuterCapStyles(startCap, endCap);
        SetDashCapStyle(capStyle);
        SetStyle(dashStyle, joinStyle, transformStyle);
    }
    void LineStylizer::SetMiterLimit(float miterLimit)
    {
        m_StrokeStyle.SetMiterLimit(miterLimit);
    }
    void LineStylizer::SetDashOffset(float dashOffset)
    {
        m_StrokeStyle.SetDashOffset(dashOffset);
    }
    void LineStylizer::SetCustomDashes(const std::vector<float>& values)
    {
        m_StrokeStyle.SetCustomDashes(values);
    }
    LineStylizer::Cap LineStylizer::GetStartCapStyle() const
    {
        return m_StrokeStyle.GetStartCapStyle();
    }
    LineStylizer::Cap LineStylizer::GetEndCapStyle() const
    {
        return m_StrokeStyle.GetEndCapStyle();
    }
    LineStylizer::Cap LineStylizer::GetCapStyle() const
    {
        return m_StrokeStyle.GetCapStyle();
    }
    LineStylizer::Join LineStylizer::GetJoinStyle() const
    {
        return m_StrokeStyle.GetJoinStyle();
    }
    LineStylizer::Dash LineStylizer::GetDashStyle() const
    {
        return m_StrokeStyle.GetDashStyle();
    }
    LineStylizer::TransformStyle LineStylizer::GetTransformStyle() const
    {
        return m_StrokeStyle.GetTransformStyle();
    }
    float LineStylizer::GetMiterLimit() const
    {
        return m_StrokeStyle.GetMiterLimit();
    }
    float LineStylizer::GetDashOffset() const
    {
        return m_StrokeStyle.GetDashOffset();
    }
    std::vector<float> LineStylizer::GetCustomDashes() const
    {
        return m_StrokeStyle.GetCustomDashes();
    }
}
