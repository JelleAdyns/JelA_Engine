#include "ObservingObjects.h"
#include "ComponentManager.h"

namespace jela
{
    void ComponentIndex::Notify(const CompsChangedInfo& changeInfo)
    {
        if (!GetValue().has_value()) return;

        if (GetValue() == changeInfo.removedComp)
        {
            ResetValue();
            return;
        }

        if (changeInfo.changedIndexInfo.has_value())
        {
            const auto [prevIndex, newIndex] = changeInfo.changedIndexInfo.value();
            if (GetValue() == prevIndex) SetValue(newIndex);
        }
    }
}
