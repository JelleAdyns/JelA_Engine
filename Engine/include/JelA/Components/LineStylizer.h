#ifndef LINESTYLIZER_H
#define LINESTYLIZER_H
#include "Component.h"
#include "JelA/StrokeStyle.h"
#include "JelA/Structs.h"

namespace jela
{
    class LineStylizer final : public Component
    {
    public:
        const StrokeStyle& GetStrokeStyle() const;
        StrokeStyle& GetStrokeStyle();
        //void SetOuterCapStyles(Cap startCap, Cap endCap);
    private:
        StrokeStyle m_StrokeStyle{};
    };

}

#endif //LINESTYLIZER_H
