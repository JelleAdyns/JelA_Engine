#ifndef COMPONENTALLOCATOR_H
#define COMPONENTALLOCATOR_H

#include <cassert>
#include <cstdint>
#include <new>
#include <format>
#include <unordered_map>

#include "Defines.h"
#include "Component.h"

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

        ~ComponentAllocator()
        {
            ::operator delete(m_Begin, std::align_val_t{m_BlockAlignment});
        }

        ComponentAllocator(const ComponentAllocator& other) = delete;
        ComponentAllocator& operator=(const ComponentAllocator& other) = delete;

        ComponentAllocator(ComponentAllocator&& other) noexcept = default;      // Defined
        ComponentAllocator& operator=(ComponentAllocator&& other) noexcept = delete;

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

}

inline void* operator new(std::size_t n, jela::ComponentAllocator& alloc)
{
    return alloc.Acquire(n);
}
inline void* operator new[](std::size_t n, jela::ComponentAllocator& alloc)
{
    return operator new(n, alloc);
}
inline void operator delete(void* p, jela::ComponentAllocator& alloc) noexcept
{
    alloc.Release(p);
}
inline void operator delete[](void* p, jela::ComponentAllocator& alloc) noexcept
{
    operator delete(p, alloc);
}

#endif //COMPONENTALLOCATOR_H
