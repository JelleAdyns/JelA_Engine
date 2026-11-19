#include "JelA/StrokeStyle.h"
#include <memory>


#include "JelA/Engine.h"

namespace jela
{
    StrokeStyle::StrokeStyle():
        StrokeStyle{Dash::Solid, Cap::Flat}
    {}
    StrokeStyle::StrokeStyle(
        Dash dashStyle,
        Cap startCap,
        Cap endCap,
        Cap dashCap,
        Join joinStyle,
        TransformStyle transformStyle,
        float miterLimit,
        float dashOffset
        ):
        m_StartCap{startCap},
        m_EndCap{endCap},
        m_DashCap{dashCap},
        m_LineJoin{joinStyle},
        m_Style{dashStyle},
        m_Transform{transformStyle},
        m_MiterLimit{miterLimit},
        m_DashOffset{dashOffset}
    {
        CreateStrokeStyle(nullptr, 0);
    }
    StrokeStyle::StrokeStyle(Dash dashStyle, TransformStyle transformStyle, Join joinStyle, Cap startCap, Cap endCap, Cap dashCap, float miterLimit, float dashOffset):
        StrokeStyle{dashStyle, startCap, endCap, dashCap, joinStyle, transformStyle, miterLimit, dashOffset}
    {}
    StrokeStyle::~StrokeStyle()
    {
        SafeRelease(&m_pStrokeStyle);
    }
    StrokeStyle::StrokeStyle(const StrokeStyle& other):
        m_CustomDashesCount{other.m_CustomDashesCount},
        m_NeedsToChange{other.m_NeedsToChange},
        m_StartCap{other.m_StartCap},
        m_EndCap{other.m_EndCap},
        m_DashCap{other.m_DashCap},
        m_LineJoin{other.m_LineJoin},
        m_Style{other.m_Style},
        m_Transform{other.m_Transform},
        m_MiterLimit{other.m_MiterLimit},
        m_DashOffset{other.m_DashOffset}
    {
        m_pCustomDashes = nullptr;

        if (m_CustomDashesCount > 0)
        {
            const auto p = new float[m_CustomDashesCount]{};
            for (uint8_t i = 0; i < m_CustomDashesCount; ++i) p[i] = other.m_pCustomDashes[i];
            m_pCustomDashes.reset(p);
        }

        UpdateState();
    }
    StrokeStyle::StrokeStyle(StrokeStyle&& other) noexcept:
        m_pStrokeStyle{std::exchange(other.m_pStrokeStyle, nullptr)},
        m_pCustomDashes{std::exchange(other.m_pCustomDashes, nullptr)},
        m_CustomDashesCount{std::exchange(other.m_CustomDashesCount, std::uint8_t{})},
        m_NeedsToChange{std::exchange(other.m_NeedsToChange, false)},
        m_StartCap{std::exchange(other.m_StartCap, Cap::Flat)},
        m_EndCap{std::exchange(other.m_EndCap, Cap::Flat)},
        m_DashCap{std::exchange(other.m_DashCap, Cap::Flat)},
        m_LineJoin{std::exchange(other.m_LineJoin, Join::Miter)},
        m_Style{std::exchange(other.m_Style, Dash::Solid)},
        m_Transform{std::exchange(other.m_Transform, TransformStyle::Normal)},
        m_MiterLimit{std::exchange(other.m_MiterLimit, 0.f)},
        m_DashOffset{std::exchange(other.m_DashOffset, 0.f)}
    {}
    StrokeStyle& StrokeStyle::operator=(const StrokeStyle& other)
    {
        StrokeStyle{other}.swap(*this);
        return *this;
    }
    StrokeStyle& StrokeStyle::operator=(StrokeStyle&& other) noexcept
    {
        StrokeStyle{std::move(other)}.swap(*this);
        return *this;
    }

    void StrokeStyle::swap(StrokeStyle& other) noexcept
    {
        std::swap(m_NeedsToChange, other.m_NeedsToChange);
        std::swap(m_StartCap, other.m_StartCap);
        std::swap(m_EndCap, other.m_EndCap);
        std::swap(m_DashCap, other.m_DashCap);
        std::swap(m_LineJoin, other.m_LineJoin);
        std::swap(m_Style, other.m_Style);
        std::swap(m_Transform, other.m_Transform);
        std::swap(m_MiterLimit, other.m_MiterLimit);
        std::swap(m_DashOffset, other.m_DashOffset);
        std::swap(m_pStrokeStyle, other.m_pStrokeStyle);
        std::swap(m_pCustomDashes, other.m_pCustomDashes);
        std::swap(m_CustomDashesCount, other.m_CustomDashesCount);
    }
    void StrokeStyle::SetOuterCapStyles(Cap startCap, Cap endCap)
    {
        m_StartCap = startCap;
        m_EndCap = endCap;
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetCapStyle(Cap dashCap)
    {
        m_DashCap = dashCap;
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetJoinStyle(Join joinStyle)
    {
        m_LineJoin = joinStyle;
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetDashStyle(Dash dash)
    {
        m_Style = dash;
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetTransformStyle(TransformStyle transformStyle)
    {
        m_Transform = transformStyle;
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetMiterLimit(float miterLimit)
    {
        m_MiterLimit = std::max(miterLimit, 1.0f);
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetDashOffset(float dashOffset)
    {
        m_DashOffset = std::max(dashOffset, 0.0f);
        m_NeedsToChange = true;
    }
    void StrokeStyle::SetCustomDashes(const std::vector<float>& values)
    {
        constexpr uint8_t max = std::numeric_limits<uint8_t>::max();

        m_Style = Dash::Custom;
        if (values.size() > max)
        {
            const std::vector<float> cappedValues
            {
                values.begin(),
                std::next(values.begin(), max - 1)
            };
            CreateStrokeStyle(cappedValues.data(), static_cast<uint32_t>(cappedValues.size()));
            OutputDebugString(std::format(_T("Capped the custom dashes count to a size of {}, because the size of the vector was larger (with a size of {}).\n"), max, values.size()).c_str());
        }
        else CreateStrokeStyle(values.data(), static_cast<uint32_t>(values.size()));

        const auto size = m_pStrokeStyle->GetDashesCount();
        m_CustomDashesCount = static_cast<std::uint8_t>(size);
        m_pCustomDashes.reset();

        // Create new array if necessary
        if (size > 0)
        {
            const auto p = new float[size]{};
            m_pStrokeStyle->GetDashes(p, size);
            m_pCustomDashes.reset(p);
        }

        m_NeedsToChange = true;
    }
    StrokeStyle::Cap StrokeStyle::GetStartCapStyle() const { return m_StartCap; }
    StrokeStyle::Cap StrokeStyle::GetEndCapStyle() const { return m_EndCap; }
    StrokeStyle::Cap StrokeStyle::GetCapStyle() const { return m_DashCap; }
    StrokeStyle::Join StrokeStyle::GetJoinStyle() const { return m_LineJoin; }
    StrokeStyle::Dash StrokeStyle::GetDashStyle() const { return m_Style; }
    StrokeStyle::TransformStyle StrokeStyle::GetTransformStyle() const { return m_Transform; }

    float StrokeStyle::GetMiterLimit() const { return m_MiterLimit; }
    float StrokeStyle::GetDashOffset() const { return m_DashOffset; }
    std::vector<float> StrokeStyle::GetCustomDashes() const
    {
        if (m_pStrokeStyle == nullptr) return {};
        if (m_pCustomDashes == nullptr) return {};
        if (m_CustomDashesCount == 0) return {};

        return std::vector<float>{&(m_pCustomDashes[0]), &(m_pCustomDashes[m_CustomDashesCount])};
    }

    ID2D1StrokeStyle1* StrokeStyle::RendererDoor::GetStyle(const StrokeStyle& style){ return style.GetStyle(); }
    ID2D1StrokeStyle1* StrokeStyle::GetStyle() const
    {
        if (m_NeedsToChange) UpdateState();

        return m_pStrokeStyle;
    }
    void StrokeStyle::CreateStrokeStyle(const float* customDashes, uint32_t numberOfElements) const
    {
        SafeRelease(&m_pStrokeStyle);

        HResultHandler hr {S_OK, _T("StrokeStyle::CreateStrokeStyle")};
        const auto& factory = ENGINE.RenderMngr()->Get2DFactory().get();
        hr = factory->CreateStrokeStyle(
            D2D1::StrokeStyleProperties1(
                static_cast<D2D1_CAP_STYLE>(m_StartCap),
                static_cast<D2D1_CAP_STYLE>(m_EndCap),
                static_cast<D2D1_CAP_STYLE>(m_DashCap),
                static_cast<D2D1_LINE_JOIN>(m_LineJoin),
                m_MiterLimit,
                static_cast<D2D1_DASH_STYLE>(m_Style),
                m_DashOffset,
                static_cast<D2D1_STROKE_TRANSFORM_TYPE>(m_Transform)
                ),
            customDashes,
            customDashes ? numberOfElements : 0,
            &m_pStrokeStyle);

        m_NeedsToChange = false;
    }
    void StrokeStyle::UpdateState() const
    {
        if (m_CustomDashesCount == 0 || m_Style != Dash::Custom)
            CreateStrokeStyle(nullptr, 0);
        else if (m_Style == Dash::Custom)
            CreateStrokeStyle(m_pCustomDashes.get(), m_CustomDashesCount);
    }
}
