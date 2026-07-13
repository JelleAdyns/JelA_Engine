#include "ComponentAllocator.h"

namespace jela
{
    void* ComponentAllocator::Acquire(std::size_t n)
    {
        if (n != m_BlockSize)
            throw std::length_error("Invalid block size.");

        if (!m_FirstFreeObjectIndex.has_value())
            return AllocateOverflow(n, OverflowMessage());

        const auto freeIndex = m_FirstFreeObjectIndex.value();
        assert(freeIndex < static_cast<std::uint32_t>(m_Capacity));
        assert((!IsInUse(freeIndex) && "Index had a value, but was refering to a block that was already in use."));

        void* block = GetAddressFromIndex(freeIndex);

        // Mark as 'In Use'
        SetInUse(freeIndex, true);

        // Find next free object
        m_FirstFreeObjectIndex.reset();
        for (std::uint32_t index = freeIndex + 1; index < static_cast<std::uint32_t>(m_Capacity); index++)
        {
            if (IsInUse(index)) continue;

            m_FirstFreeObjectIndex = index;
            break;
        }

        return block;
    }
    void ComponentAllocator::Release(void* p) noexcept
    {
        if (p == nullptr)
        {
            OutputDebugString(_T("Trying to release a nullptr! Returning...\n"));
            return;
        }

        const ptrdiff_t ptrDifference = static_cast<std::byte*>(p) - m_Begin;
        const auto index = static_cast<std::size_t>(ptrDifference) / m_BlockSize;

        if (p < m_Begin ||
            index >= m_Capacity)
        {
            if (TryDeallocateOverfow(p))
                return;

            OutputDebugString(_T("Invalid pointer to release. Returning...\n"));
            return;
        }

        const auto uintIndex = static_cast<std::uint32_t>(index);

        if (!IsInUse(uintIndex))
        {
            OutputDebugString(std::format(_T("Trying to release unused memory at address {}. Returning...\n"), p).c_str());
            return;
        }

#ifndef NDEBUG
        std::memset(p, FREE_DATA_PATTERN, m_BlockSize);
#endif // NDEBUG

        SetInUse(uintIndex, false);
        if (uintIndex < m_FirstFreeObjectIndex) m_FirstFreeObjectIndex = uintIndex;
    }

    bool ComponentAllocator::IsOverflown() const
    {
        return m_OverflowKey && OVERFLOWED_ALLOCATIONS.contains(m_OverflowKey);
    }
    std::size_t ComponentAllocator::AmountOfOverflowAllocations() const
    {
        if (!IsOverflown()) return 0;
        return  OVERFLOWED_ALLOCATIONS.at(m_OverflowKey).size();
    }
    void* ComponentAllocator::GetAddressFromIndex(std::size_t index) const
    {
        return m_Begin + index * m_BlockSize;
    }
    bool ComponentAllocator::IsInUse(std::uint32_t index) const
    {
        return *(m_InUse + sizeof(bool) * index);
    }
    void ComponentAllocator::SetInUse(std::uint32_t index, bool value) const
    {
        *(m_InUse + sizeof(bool) * index) = value;
    }
    tstring ComponentAllocator::OverflowMessage() const
    {
        return std::format(_T("ComponentAllocator with block size {}, block aligment {} and capacity {} was full when trying to allocate a component. Continuing with 'std::malloc'.\n"
                                "TIP: Define a 'static constexpr std::size_t MAX_AMOUNT' field in your component class to customize the max amount of that component.\n"),
                                m_BlockSize, m_BlockAlignment, m_Capacity);
    }
    void* ComponentAllocator::AllocateOverflow(std::size_t n, const tstring& message)
    {
        OutputDebugString(message.c_str());

        if (m_OverflowKey == nullptr)
            m_OverflowKey = this;

        // ReSharper disable once CppDFAMemoryLeak
        const auto p = std::malloc(n);
        if (p == nullptr) throw std::bad_alloc();

        return OVERFLOWED_ALLOCATIONS[m_OverflowKey].emplace_back(p);
    }
    bool ComponentAllocator::TryDeallocateOverfow(void* p) const
    {
        if (m_OverflowKey == nullptr) return false;
        if (!OVERFLOWED_ALLOCATIONS.contains(m_OverflowKey)) return false;

        auto& addresses = OVERFLOWED_ALLOCATIONS.at(m_OverflowKey);
        const auto it = std::ranges::find(addresses, p);

        if (it == addresses.cend()) return false;

        addresses.erase(it);
        if (addresses.empty()) OVERFLOWED_ALLOCATIONS.erase(m_OverflowKey);

        std::free(p);
        return true;
    }
    std::byte* ComponentAllocator::CreateBuffer() const
    {
        auto boolSize = sizeof(bool) * m_Capacity;

        // -------------------------------------------------------------------------------------------
        // Add trailing padding
        if (boolSize % m_BlockAlignment != 0)
        {
            const auto boolSizeAligned = (boolSize / m_BlockAlignment + 1) * m_BlockAlignment;
            assert(boolSizeAligned > boolSize);
            assert(boolSizeAligned % m_BlockAlignment == 0);
            boolSize = boolSizeAligned;
        }
        // -------------------------------------------------------------------------------------------

        return static_cast<std::byte*>(::operator new (m_BlockSize * m_Capacity + boolSize, std::align_val_t{m_BlockAlignment}));
    }
}
