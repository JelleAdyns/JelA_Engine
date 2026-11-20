#include "JelA/Components/LineStylizer.h"

namespace jela
{
    const StrokeStyle& LineStylizer::GetStrokeStyle() const
    {
        return m_StrokeStyle;
    }
    StrokeStyle& LineStylizer::GetStrokeStyle()
    {
        return m_StrokeStyle;
    }
}
