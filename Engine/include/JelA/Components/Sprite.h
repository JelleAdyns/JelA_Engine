#ifndef SPRITE_H
#define SPRITE_H
#include "Component.h"

namespace jela
{
    class TextureImage;

    class Sprite final : public Component
    {
    public:
        explicit Sprite();
        explicit Sprite(int nrOfCols, int nrOfRows, float frameTime = 0.f, bool updateAutomatically = true, bool updateRows = false);

        void Start() override;
        void Update() override;

        void SetFrameTime(float frameTime);
        void SetUpdate(bool updateAutomatically);
        void SetUpdateRows(bool updateRows);
        void SetNrOfColums(uint16_t cols);
        void SetNrOfRows(uint16_t rows);

        void SetCurrentFrame(uint16_t frame);
        void SetColumn(uint16_t col);
        void SetRow(uint16_t row);

        void NextFrame();
        void NextColumn();
        void NextRow();

        void PreviousFrame();
        void PreviousColumn();
        void PreviousRow();

        uint16_t GetNrOfFrames() const;
        uint16_t GetNrOfCols() const;
        uint16_t GetNrOfRows() const;

        uint16_t GetCurrentFrame() const;
        uint16_t GetCurrentColumn() const;
        uint16_t GetCurrentRow() const;

    private:

        bool m_Update;
        bool m_UpdateRows;

        uint16_t m_AmountOfCols;
        uint16_t m_AmountOfRows;
        uint16_t m_CurrFrame;

        float m_FrameTime;
        float m_PassedTime;

        TextureImage* m_pImage;
    };

} // jela

#endif //SPRITE_H
