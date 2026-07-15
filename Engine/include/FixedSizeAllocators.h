#ifndef FIXEDSIZEALLOCATORS_H
#define FIXEDSIZEALLOCATORS_H

#include <optional>
#include <typeindex>
#include <unordered_map>

#include "Defines.h"

namespace jela
{
    class FixedSizeAllocator
    {
    public:

    template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T>, std::size_t capacity):
            m_Capacity{capacity},
            m_BlockSize{sizeof(T)},
            m_BlockAlignment{alignof(T)},
            m_Begin{CreateBuffer()},
            m_InUse{reinterpret_cast<bool*>(m_Begin + m_BlockSize * m_Capacity)},
            m_FirstFreeObjectIndex{0}
        {
#ifndef NDEBUG
            std::memset(m_Begin, FREE_DATA_PATTERN, m_BlockSize * m_Capacity);
            std::memset(m_InUse, IN_USE_PATTERN, ((sizeof(bool) * m_Capacity) / m_BlockAlignment + 1) * m_BlockAlignment);
#endif // NDEBUG

            std::memset(m_InUse, 0, sizeof(bool) * m_Capacity);
        }

        ~FixedSizeAllocator()
        {
            ::operator delete(m_Begin, std::align_val_t{m_BlockAlignment});
        }

        FixedSizeAllocator(const FixedSizeAllocator& other) = delete;
        FixedSizeAllocator& operator=(const FixedSizeAllocator& other) = delete;

        FixedSizeAllocator(FixedSizeAllocator&& other) noexcept = default;      // Defined
        FixedSizeAllocator& operator=(FixedSizeAllocator&& other) noexcept = delete;

        void* Acquire(std::size_t n);
        void Release(void* p) noexcept;

        bool IsOverflown() const;
        std::size_t AmountOfOverflowAllocations() const;

    private:

        static inline std::unordered_map<void*, std::vector<void*>> OVERFLOWED_ALLOCATIONS{};

        static constexpr uint8_t FREE_DATA_PATTERN = 0xAA;
        static constexpr uint8_t IN_USE_PATTERN = 0xBB;

        const std::size_t m_Capacity;
        const std::size_t m_BlockSize;
        const std::size_t m_BlockAlignment;

        std::byte* m_Begin; // Reserved buffer
        bool* m_InUse;

        std::optional<std::uint32_t> m_FirstFreeObjectIndex;

        void* m_OverflowKey{nullptr};

        void* GetAddressFromIndex(std::size_t index) const;
        bool IsInUse(std::uint32_t index) const;
        void SetInUse(std::uint32_t index, bool value) const;

        tstring OverflowMessage() const;
        void* AllocateOverflow(std::size_t n, const tstring& message);
        bool TryDeallocateOverfow(void* p) const;

        std::byte* CreateBuffer() const;
    };

    template <typename T, std::size_t N>
    class TypeAllocator final : public FixedSizeAllocator
    {
    public:
        explicit TypeAllocator():
            FixedSizeAllocator{std::type_identity<T>{}, N}
        {}
    };
}

inline void* operator new(std::size_t n, jela::FixedSizeAllocator& alloc)
{
    return alloc.Acquire(n);
}
inline void* operator new[](std::size_t n, jela::FixedSizeAllocator& alloc)
{
    return operator new(n, alloc);
}
inline void operator delete(void* p, jela::FixedSizeAllocator& alloc) noexcept
{
    alloc.Release(p);
}
inline void operator delete[](void* p, jela::FixedSizeAllocator& alloc) noexcept
{
    operator delete(p, alloc);
}
#endif //FIXEDSIZEALLOCATORS_H
