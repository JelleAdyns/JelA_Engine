#ifndef FIXEDSIZEALLOCATORS_H
#define FIXEDSIZEALLOCATORS_H

#include <cassert>
#include <optional>
#include <typeindex>
#include <unordered_map>

#include "Defines.h"
#include "MemoryAllocator.h"

namespace jela
{
    class FixedSizeAllocator : public MemoryAllocator
    {
    public:

    template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T>, std::size_t capacity):
            MemoryAllocator{},
            m_Capacity{capacity},
            m_BlockSize{sizeof(T)},
            m_BlockAlignment{alignof(T)},
            m_pBegin{CreateBuffer()},
            m_pInUse{reinterpret_cast<bool*>(m_pBegin + m_BlockSize * m_Capacity)},
            m_FirstFreeObjectIndex{0}
        {
            assert(m_BlockSize % m_BlockAlignment == 0);

#ifndef NDEBUG
            std::memset(m_pBegin, FREE_DATA_PATTERN, DataSize());
            std::memset(m_pInUse, IN_USE_PATTERN, NonDataSize());
#endif // NDEBUG

            std::memset(m_pInUse, 0, sizeof(bool) * m_Capacity);
        }

        ~FixedSizeAllocator() override
        {
            ::operator delete(m_pBegin, CompleteBufferSize(), std::align_val_t{m_BlockAlignment});
        }

        FixedSizeAllocator(const FixedSizeAllocator& other) = delete;
        FixedSizeAllocator& operator=(const FixedSizeAllocator& other) = delete;

        FixedSizeAllocator(FixedSizeAllocator&& other) noexcept = default;      // Defined
        FixedSizeAllocator& operator=(FixedSizeAllocator&& other) noexcept = delete;

        void* Acquire(std::size_t n) override;
        void Release(void* p) noexcept override;

        bool IsOverflown() const;
        std::size_t AmountOfOverflowAllocations() const;

        std::size_t DataSize() const;
        std::size_t NonDataSize() const;
        std::size_t CompleteBufferSize() const;
        std::size_t GetCapacity() const;
        std::size_t GetBlockSize() const;
        std::size_t GetBlockAlignment() const;
    private:

        static inline std::unordered_map<void*, std::vector<void*>> OVERFLOWED_ALLOCATIONS{};

        static constexpr uint8_t FREE_DATA_PATTERN = 0xAA;
        static constexpr uint8_t IN_USE_PATTERN = 0xBB;

        const std::size_t m_Capacity;
        const std::size_t m_BlockSize;
        const std::size_t m_BlockAlignment;

        std::byte* m_pBegin; // Reserved buffer
        bool* m_pInUse;

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
        {
            static_assert(N > 0, "Amount of objects must be greater than 0");
        }
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
