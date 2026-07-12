#ifndef COMPONENTALLOCATOR_H
#define COMPONENTALLOCATOR_H

#include <cassert>
#include <cstdint>
#include <new>
#include <format>
#include "Defines.h"

namespace jela
{

    class ComponentAllocator final
    {
    public:

        template <cDerivedComponent T>
        explicit ComponentAllocator(std::type_identity<T>):
            m_Capacity{Component::GetAmount<T>()},
            m_BlockSize{sizeof(T)},
            m_BlockAlignment{alignof(T)},
            m_Begin{[&]()
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
            }()},
            m_InUse{reinterpret_cast<bool*>(m_Begin + m_BlockSize * m_Capacity)},
            m_FirstFreeObjectIndex{0}
        {


#ifndef NDEBUG
            std::memset(m_Begin, DATA_PATTERN, m_BlockSize * m_Capacity);
            std::memset(m_InUse, IN_USE_PATTERN, ((sizeof(bool) * m_Capacity) / m_BlockAlignment + 1) * m_BlockAlignment);
#endif // NDEBUG

            std::memset(m_InUse, 0, sizeof(bool) * m_Capacity);
        }

        ~ComponentAllocator()
        {
            ::operator delete(m_Begin, std::align_val_t{m_BlockAlignment});
        }

        void* Acquire(std::size_t n)
        {
            if (n != m_BlockSize)
                throw std::length_error("Invalid block size.");

            if (!m_FirstFreeObjectIndex.has_value())
            {
               return AllocateOverflow(n, OverflowMessage());
            }

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
                if (!IsInUse(index))
                {
                    m_FirstFreeObjectIndex = index;
                    break;
                }
            }

            return block;
        }

        void Release(void* p)
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
                if (TryDeallocateOverfow(p)) return;
                throw std::runtime_error("Invalid pointer to release.");
            }

#ifndef NDEBUG
            std::memset(p, DATA_PATTERN, m_BlockSize);
#endif // NDEBUG

            SetInUse(static_cast<std::uint32_t>(index), false);
            if (index < m_FirstFreeObjectIndex)
                m_FirstFreeObjectIndex = static_cast<std::uint32_t>(index);
        }

        bool IsOverflown() const
        {
            return m_OverflowKey && OVERFLOWED_ALLOCATIONS.contains(m_OverflowKey);
        }

        std::size_t AmountOfOverflowAllocations() const
        {
            if (!IsOverflown()) return 0;
            return  OVERFLOWED_ALLOCATIONS.at(m_OverflowKey).size();
        }

    private:

        const std::size_t m_Capacity;
        const std::size_t m_BlockSize;
        const std::size_t m_BlockAlignment;

        // Reserved buffer
        std::byte* m_Begin;
        bool* m_InUse;

        std::optional<std::uint32_t> m_FirstFreeObjectIndex;

        void* m_OverflowKey{nullptr};

        void* GetAddressFromIndex(std::size_t index) const
        {
            return m_Begin + index * m_BlockSize;
        }

        bool IsInUse(std::uint32_t index) const
        {
            return *(m_InUse + sizeof(bool) * index);
        }
        void SetInUse(std::uint32_t index, bool value) const
        {
            *(m_InUse + sizeof(bool) * index) = value;
        }

        tstring OverflowMessage() const
        {
            return std::format(_T("ComponentAllocator with block size {}, block aligment {} and capacity {} was full when trying to allocate a component. Continuing with 'std::malloc'.\n"
                                    "TIP: Define a 'static constexpr std::size_t MAX_AMOUNT' field in your component class to customize the max amount of that component.\n"),
                                    m_BlockSize, m_BlockAlignment, m_Capacity);
        }

        static constexpr uint8_t DATA_PATTERN = 0xAA;
        static constexpr uint8_t IN_USE_PATTERN = 0xBB;


        static inline std::unordered_map<void*, std::vector<void*>> OVERFLOWED_ALLOCATIONS{};

        void* AllocateOverflow(std::size_t n, const tstring& message)
        {
            OutputDebugString(message.c_str());

            if (m_OverflowKey == nullptr)
                m_OverflowKey = this;

            return OVERFLOWED_ALLOCATIONS[m_OverflowKey].emplace_back(std::malloc(n));
        }
        bool TryDeallocateOverfow(void* p) const
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
    };

}

inline void* operator new(std::size_t n, jela::ComponentAllocator& alloc)
{
    return alloc.Acquire(n);
}
inline void operator delete(void* p, jela::ComponentAllocator& alloc)
{
    alloc.Release(p);
}

#endif //COMPONENTALLOCATOR_H
