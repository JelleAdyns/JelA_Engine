#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include <cstddef>
#include <unordered_map>
#include <utility>

#include "Defines.h"

namespace jela
{
    class MemoryAllocator
    {
    public:
        MemoryAllocator() = default;
        explicit MemoryAllocator(const tstring& customOverflowMessage):
            MemoryAllocator{customOverflowMessage, std::nullopt}
        {}
        explicit MemoryAllocator(std::optional<std::reference_wrapper<MemoryAllocator>> alloc):
            m_OptionalAllocator{alloc}
        {}
        explicit MemoryAllocator(const tstring& customOverflowMessage, std::optional<std::reference_wrapper<MemoryAllocator>> alloc):
            m_OptionalAllocator{alloc},
            m_OverflowMessage{customOverflowMessage}
        {}
        virtual ~MemoryAllocator() = default;

        MemoryAllocator(const MemoryAllocator& other) = default;
        MemoryAllocator(MemoryAllocator&& other) noexcept = default;
        MemoryAllocator& operator=(const MemoryAllocator& other) = default;
        MemoryAllocator& operator=(MemoryAllocator&& other) noexcept = default;

        virtual void* Acquire(std::size_t n) = 0;
        virtual void Release(void* p) noexcept = 0;
        bool IsOverflown() const
        {
            return m_OverflowKey && OVERFLOWED_ALLOCATIONS.contains(m_OverflowKey);
        }
        std::size_t AmountOfOverflowAllocations() const
        {
            if (!IsOverflown()) return 0;
            return  OVERFLOWED_ALLOCATIONS.at(m_OverflowKey).size();
        }
    protected:
        void* AllocateOverflow(std::size_t n)
        {
            OutputDebugString(m_OverflowMessage.c_str());

            if (m_OverflowKey == nullptr)
                m_OverflowKey = this;

            // ReSharper disable once CppDFAMemoryLeak
            const auto p = std::malloc(n);
            if (p == nullptr) throw std::bad_alloc();

            return OVERFLOWED_ALLOCATIONS[m_OverflowKey].emplace_back(p);
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

        std::optional<std::reference_wrapper<MemoryAllocator>> GetOptionalMemoryAllocator() const
        {
            return m_OptionalAllocator;
        }

    private:
        static inline std::unordered_map<void*, std::vector<void*>> OVERFLOWED_ALLOCATIONS{};
        void* m_OverflowKey{nullptr};
        const std::optional<std::reference_wrapper<MemoryAllocator>> m_OptionalAllocator{std::nullopt};
        tstring m_OverflowMessage{ _T("Memory was full. Allocating via std::malloc...\n")};
    };
}

inline void* operator new(std::size_t n, jela::MemoryAllocator& alloc)
{
    return alloc.Acquire(n);
}
inline void* operator new[](std::size_t n, jela::MemoryAllocator& alloc)
{
    return operator new(n, alloc);
}
inline void operator delete(void* p, jela::MemoryAllocator& alloc) noexcept
{
    alloc.Release(p);
}
inline void operator delete[](void* p, jela::MemoryAllocator& alloc) noexcept
{
    operator delete(p, alloc);
}
#endif //MEMORYALLOCATOR_H
