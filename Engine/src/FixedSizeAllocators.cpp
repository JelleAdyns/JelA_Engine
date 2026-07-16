#include "FixedSizeAllocators.h"

namespace jela
{
    void* FixedSizeAllocator::Acquire(std::size_t n)
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
    void FixedSizeAllocator::Release(void* p) noexcept
    {
        if (p == nullptr)
        {
            OutputDebugString(_T("Trying to release a nullptr! Returning...\n"));
            return;
        }

        const ptrdiff_t ptrDifference = static_cast<std::byte*>(p) - m_pBegin;
        const auto index = static_cast<std::size_t>(ptrDifference) / m_BlockSize;

        if (p < m_pBegin ||
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

    bool FixedSizeAllocator::IsOverflown() const
    {
        return m_OverflowKey && OVERFLOWED_ALLOCATIONS.contains(m_OverflowKey);
    }
    std::size_t FixedSizeAllocator::AmountOfOverflowAllocations() const
    {
        if (!IsOverflown()) return 0;
        return  OVERFLOWED_ALLOCATIONS.at(m_OverflowKey).size();
    }
    std::size_t FixedSizeAllocator::DataSize() const
    {
        return m_BlockSize * m_Capacity;
    }
    std::size_t FixedSizeAllocator::NonDataSize() const
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

        return boolSize;
    }
    std::size_t FixedSizeAllocator::CompleteBufferSize() const
    {
        return DataSize() + NonDataSize();
    }
    std::size_t FixedSizeAllocator::GetCapacity() const
    {
        return m_Capacity;
    }
    std::size_t FixedSizeAllocator::GetBlockSize() const
    {
        return m_BlockSize;
    }
    std::size_t FixedSizeAllocator::GetBlockAlignment() const
    {
        return m_BlockAlignment;
    }
    void* FixedSizeAllocator::GetAddressFromIndex(std::size_t index) const
    {
        return m_pBegin + index * m_BlockSize;
    }
    bool FixedSizeAllocator::IsInUse(std::uint32_t index) const
    {
        return *(m_pInUse + sizeof(bool) * index);
    }
    void FixedSizeAllocator::SetInUse(std::uint32_t index, bool value) const
    {
        *(m_pInUse + sizeof(bool) * index) = value;
    }
    tstring FixedSizeAllocator::OverflowMessage() const
    {
        return std::format(_T("FixedSizeAllocator with block size {}, block aligment {} and capacity {} was full when trying to allocate a component. Continuing with 'std::malloc'.\n"
                                "TIP: Define a 'static constexpr std::size_t MAX_AMOUNT' field in your component class to customize the max amount of that component.\n"),
                                m_BlockSize, m_BlockAlignment, m_Capacity);
    }
    void* FixedSizeAllocator::AllocateOverflow(std::size_t n, const tstring& message)
    {
        OutputDebugString(message.c_str());

        if (m_OverflowKey == nullptr)
            m_OverflowKey = this;

        // ReSharper disable once CppDFAMemoryLeak
        const auto p = std::malloc(n);
        if (p == nullptr) throw std::bad_alloc();

        return OVERFLOWED_ALLOCATIONS[m_OverflowKey].emplace_back(p);
    }
    bool FixedSizeAllocator::TryDeallocateOverfow(void* p) const
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
    std::byte* FixedSizeAllocator::CreateBuffer() const
    {
        return static_cast<std::byte*>(::operator new (CompleteBufferSize(), std::align_val_t{m_BlockAlignment}));
    }
}
