#include "Sprite.h"

#include "Engine.h"
#include "GameObject.h"
#include "TextureImage.h"

namespace jela
{
    Sprite::Sprite():
        Sprite{1, 1, 0.f}
    {}
    Sprite::Sprite(int nrOfCols, int nrOfRows, float frameTime, bool updateAutomatically, bool updateRows)
        : m_Update{updateAutomatically}
        , m_UpdateRows{updateRows}
        , m_AmountOfCols{static_cast<::uint16_t>(nrOfCols)}
        , m_AmountOfRows{static_cast<::uint16_t>(nrOfRows)}
        , m_CurrFrame{}
        , m_FrameTime{frameTime}
        , m_PassedTime{}
        , m_pImage{}
    {
        assert((nrOfCols > 0 && nrOfRows > 0) && "Amount of colums and rows must be greater than 0!");
    }
    void Sprite::Start()
    {
        m_pImage = GetOwner()->GetComponent<TextureImage>();
        assert((m_pImage != nullptr) && "TextureImage pointer should be valid");

        m_pImage->SetSourceRectSize(m_pImage->GetTexture()->GetWidth() / m_AmountOfCols, m_pImage->GetTexture()->GetHeight() / m_AmountOfRows);
        m_pImage->UseSourceRectSize();
    }
    void Sprite::Update()
    {
        if(!m_Update) return;

        m_PassedTime += ENGINE.GetDeltaTime();
        if (m_PassedTime < m_FrameTime) return;

        if (m_UpdateRows && GetCurrentColumn() == m_AmountOfCols - 1)
            NextRow();

        NextColumn();
        m_PassedTime = 0.f;
    }
    void Sprite::SetFrameTime(float frameTime)
    {
        assert((frameTime >= 0.f) && "Frame Time cannot be negative!");
        m_FrameTime = frameTime;
    }
    void Sprite::SetUpdate(bool updateAutomatically)
    {
        m_Update = updateAutomatically;
    }
    void Sprite::SetUpdateRows(bool updateRows)
    {
        m_UpdateRows = updateRows;
    }
    void Sprite::SetNrOfColums(uint16_t cols)
    {
        m_AmountOfCols = cols;
    }
    void Sprite::SetNrOfRows(uint16_t rows)
    {
        m_AmountOfRows = rows;
    }

    void Sprite::SetCurrentFrame(uint16_t frame)
    {
        assert((frame < GetNrOfFrames()) && "Sprite frame index cannot be larger or equal to the total amount of frames!");
        m_CurrFrame = frame;
        const Rectf rect = m_pImage->GetSourceRect();
        m_pImage->SetSourceRectPos(GetCurrentColumn() * rect.width, GetCurrentRow() * rect.height);
    }
    void Sprite::SetColumn(uint16_t col)
    {
        SetCurrentFrame(GetCurrentRow() * m_AmountOfCols + col);
    }
    void Sprite::SetRow(uint16_t row)
    {
        SetCurrentFrame(row * m_AmountOfCols + GetCurrentColumn());
    }
    void Sprite::NextFrame()
    {
        SetCurrentFrame((m_CurrFrame + 1) % GetNrOfFrames());
    }
    void Sprite::NextColumn()
    {
        SetColumn((GetCurrentColumn() + 1) % m_AmountOfCols );
    }
    void Sprite::NextRow()
    {
        SetRow((GetCurrentRow() + 1) % m_AmountOfRows );
    }
    void Sprite::PreviousFrame()
    {
        const auto nrOfFrames = GetNrOfFrames();
        if (m_CurrFrame == 0) SetCurrentFrame(nrOfFrames -1);
        else SetCurrentFrame((m_CurrFrame - 1) % nrOfFrames);
    }
    void Sprite::PreviousColumn()
    {
        if (const auto column = GetCurrentColumn(); column == 0) SetColumn(m_AmountOfCols - 1);
        else SetColumn((column - 1) % m_AmountOfCols );
    }
    void Sprite::PreviousRow()
    {
        if (const auto row = GetCurrentRow(); row == 0) SetRow(m_AmountOfRows - 1);
        else SetRow((row - 1) % m_AmountOfRows);
    }
    uint16_t Sprite::GetNrOfFrames() const
    {
        return m_AmountOfCols * m_AmountOfRows;
    }
    uint16_t Sprite::GetNrOfCols() const
    {
        return m_AmountOfCols;
    }
    uint16_t Sprite::GetNrOfRows() const
    {
        return m_AmountOfRows;
    }
    uint16_t Sprite::GetCurrentFrame() const
    {
        return m_CurrFrame;
    }
    uint16_t Sprite::GetCurrentColumn() const
    {
        return (m_CurrFrame % m_AmountOfCols);
    }
    uint16_t Sprite::GetCurrentRow() const
    {
        return (m_CurrFrame / m_AmountOfCols);
    }
} // jela