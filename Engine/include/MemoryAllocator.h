#ifndef MEMORYALLOCATOR_H
#define MEMORYALLOCATOR_H
#include <cstddef>

namespace jela
{
    class MemoryAllocator
    {
    public:
        MemoryAllocator() = default;
        virtual ~MemoryAllocator() = default;

        MemoryAllocator(const MemoryAllocator& other) = default;
        MemoryAllocator(MemoryAllocator&& other) noexcept = default;
        MemoryAllocator& operator=(const MemoryAllocator& other) = default;
        MemoryAllocator& operator=(MemoryAllocator&& other) noexcept = default;

        virtual void* Acquire(std::size_t n) = 0;
        virtual void Release(void* p) noexcept = 0;
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
