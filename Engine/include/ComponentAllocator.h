#ifndef COMPONENTALLOCATOR_H
#define COMPONENTALLOCATOR_H

#include <cassert>
#include <cstdint>
#include <new>




namespace jela
{
    class WrongIndexException : public std::runtime_error
    {
    public :
        explicit WrongIndexException(const std::string& info) :
            runtime_error(std::format("BadIndexException: {}", info))
        {}
    };

    class ComponentAllocator final
    {
    public:


        explicit ComponentAllocator(std::size_t blockSize, std::size_t blockAlignment, std::size_t capacity):
            m_Begin{[&]()
            {
                auto boolSize = sizeof(bool) * capacity;

                // -------------------------------------------------------------------------------------------
                // Add trailing padding
                if (boolSize % blockAlignment != 0)
                {
                    const auto boolSizeAligned = (boolSize / blockAlignment + 1) * blockAlignment;
                    assert(boolSizeAligned > boolSize);
                    assert(boolSizeAligned % blockAlignment == 0);
                    boolSize = boolSizeAligned;
                }
                // -------------------------------------------------------------------------------------------

                return static_cast<std::byte*>(::operator new (blockSize * capacity + boolSize, std::align_val_t{blockAlignment}));
            }()},
            m_InUse{reinterpret_cast<bool*>(m_Begin + blockSize * capacity)},
            m_Capacity{capacity},
            m_BlockSize{blockSize},
            m_BlockAlignment{blockAlignment},
            m_FirstFreeObjectIndex{0}
        {
            assert(capacity > 0);
            assert(blockSize > 0);
            assert(blockSize % blockAlignment == 0);

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

        [[nodiscard]] void* Acquire(std::size_t n)
        {
            if (n > m_BlockSize)
                throw std::length_error("Invalid block size.");

            if (!m_FirstFreeObjectIndex.has_value())
            {
               return AllocateOverflow(n, OverflowMessage());
            }

            const auto freeIndex = m_FirstFreeObjectIndex.value();

            if (freeIndex >= m_Capacity)
                throw std::bad_alloc();

            if (IsInUse(freeIndex))
                throw WrongIndexException{"Index had a value, but was refering to a block that was already in use."};

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

    private:

        // Reserved buffer
        std::byte* m_Begin;
        bool* m_InUse;

        const std::size_t m_Capacity;
        const std::size_t m_BlockSize;
        const std::size_t m_BlockAlignment;
        std::optional<std::uint32_t> m_FirstFreeObjectIndex;

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

        static inline std::vector<void*> OVERFLOWED_ALLOCATIONS{};

        static void* AllocateOverflow(std::size_t n, const tstring& message)
        {
            OutputDebugString(message.c_str());
            return OVERFLOWED_ALLOCATIONS.emplace_back(std::malloc(n));
        }
        static bool TryDeallocateOverfow(void* p)
        {
            if (const auto it = std::ranges::find(OVERFLOWED_ALLOCATIONS, p);
                    it != OVERFLOWED_ALLOCATIONS.cend())
            {
                OVERFLOWED_ALLOCATIONS.erase(it);
                std::free(p);
                return true;
            }

            return false;
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
