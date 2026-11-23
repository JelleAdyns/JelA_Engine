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
        CreateStrokeStyle();
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

        CopyDashesBuffer(other.m_pCustomDashes.get(), other.m_CustomDashesCount);

        CreateStrokeStyle();
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
    void StrokeStyle::SetStartCap(Cap startCap)
    {
        if (m_StartCap != startCap) m_NeedsToChange = true;
        m_StartCap = startCap;
    }
    void StrokeStyle::SetEndCap(Cap endCap)
    {
        if (m_EndCap != endCap) m_NeedsToChange = true;
        m_EndCap = endCap;
    }
    void StrokeStyle::SetCapStyle(Cap dashCap)
    {
        if (m_DashCap != dashCap) m_NeedsToChange = true;
        m_DashCap = dashCap;
    }
    void StrokeStyle::SetJoinStyle(Join joinStyle)
    {
        if (m_LineJoin != joinStyle) m_NeedsToChange = true;
        m_LineJoin = joinStyle;
    }
    void StrokeStyle::SetDashStyle(Dash dash)
    {
        if (m_Style != dash) m_NeedsToChange = true;

        if (dash == Dash::Custom && !m_pCustomDashes)
        {
            constexpr std::array dashes{1.f};
            CopyDashesBuffer(dashes.data(), static_cast<std::uint8_t>(dashes.size()));
        }

        m_Style = dash;
    }
    void StrokeStyle::SetTransformStyle(TransformStyle transformStyle)
    {
        if (m_Transform != transformStyle) m_NeedsToChange = true;
        m_Transform = transformStyle;
    }
    void StrokeStyle::SetMiterLimit(float miterLimit)
    {
        miterLimit = std::max(miterLimit, 1.0f);
        if (m_MiterLimit != miterLimit) m_NeedsToChange = true;
        m_MiterLimit = miterLimit;
    }
    void StrokeStyle::SetDashOffset(float dashOffset)
    {
        dashOffset = std::max(dashOffset, 0.0f);
        if (m_DashOffset != dashOffset) m_NeedsToChange = true;
        m_DashOffset = dashOffset;
    }
    void StrokeStyle::SetCustomDashes(const std::vector<float>& values)
    {
        constexpr uint8_t max = std::numeric_limits<uint8_t>::max();

        std::vector resultValues{values};

        // Cap buffersize if necessary
        if (values.size() > max)
        {
            resultValues = std::vector<float>{ values.begin(), std::next(values.begin(), max - 1) };
            OutputDebugString(std::format(_T("Capped the custom dashes count to a size of {}, because the size of the vector was larger (with a size of {}).\n"), max, values.size()).c_str());
        }

        m_pCustomDashes.reset();
        const auto size = resultValues.size();

        CopyDashesBuffer(resultValues.data(),static_cast<std::uint8_t>(size));

        m_Style = Dash::Custom;
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
        return m_pStrokeStyle;
    }
    bool StrokeStyle::CreateStrokeStyle()
    {
        OutputDebugString(_T("Creating Stroke Style...\n"));
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
            (m_Style == Dash::Custom ? m_pCustomDashes.get() : nullptr),
            (m_Style == Dash::Custom ? m_CustomDashesCount : 0),
            &m_pStrokeStyle);

        m_NeedsToChange = false;

        return hr.Succeeded();
    }
    void StrokeStyle::UpdateState()
    {
        if (!m_NeedsToChange) return;

        if (!CreateStrokeStyle())
        {
            OutputDebugString(
                std::format(_T("Creation of StrokeStyle failed. Properties:\n"
                "Start Cap: {}\nEnd Cap: {}\nDash Cap: {}\nJoin Style: {}\nDash Style: {}\nTransform Style: {}\nMiter Limit: {}\nDash Offset: {}\nCustom Dashes Buffer Size: {}\n"),
                std::to_underlying(m_StartCap),
                std::to_underlying(m_EndCap),
                std::to_underlying(m_DashCap),
                std::to_underlying(m_LineJoin),
                std::to_underlying(m_Style),
                std::to_underlying(m_Transform),
                m_MiterLimit,
                m_DashOffset,
                m_CustomDashesCount).c_str()
                );
        }
    }
    void StrokeStyle::CopyDashesBuffer(const float* dashesToCopy, std::uint8_t size)
    {
        m_CustomDashesCount = size;
        if (m_CustomDashesCount > 0)
        {
            const auto p = new float[m_CustomDashesCount]{};
            for (std::uint8_t i = 0; i < m_CustomDashesCount; i++) p[i] = dashesToCopy[i];

            m_pCustomDashes.reset(p);
        }
    }
}
