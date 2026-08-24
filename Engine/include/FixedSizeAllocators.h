#ifndef FIXEDSIZEALLOCATORS_H
#define FIXEDSIZEALLOCATORS_H

#include <cassert>
#include <optional>
#include <ranges>

#include "Defines.h"
#include "MemoryAllocator.h"

namespace jela
{
    class FixedSizeAllocator : public MemoryAllocator
    {
    public:

        template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T> t, std::size_t capacity):
        FixedSizeAllocator{t, capacity, _T(""), std::nullopt}
        {}

        template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T> t, std::size_t capacity, const tstring& customOverflowMessage):
        FixedSizeAllocator{t, capacity, customOverflowMessage, std::nullopt}
        {}

        template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T> t, std::size_t capacity, MemoryAllocator& alloc):
            FixedSizeAllocator{t, capacity, _T(""), std::optional<std::reference_wrapper<MemoryAllocator>>{alloc}}
        {}

        template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T> t, std::size_t capacity, const tstring& customOverflowMessage, MemoryAllocator& alloc):
            FixedSizeAllocator{ t, capacity, customOverflowMessage, std::optional<std::reference_wrapper<MemoryAllocator>>{alloc} }
        {}

        ~FixedSizeAllocator() override
        {
            if (GetOptionalMemoryAllocator().has_value())
                operator delete(m_pBegin, GetOptionalMemoryAllocator().value());
            else ::operator delete(m_pBegin, CompleteBufferSize(), std::align_val_t{m_BlockAlignment});
        }

        FixedSizeAllocator(const FixedSizeAllocator& other) = delete;
        FixedSizeAllocator& operator=(const FixedSizeAllocator& other) = delete;

        FixedSizeAllocator(FixedSizeAllocator&& other) noexcept = default;      // Defined
        FixedSizeAllocator& operator=(FixedSizeAllocator&& other) noexcept = delete;

        void* Acquire(std::size_t n) override;
        void Release(void* p) noexcept override;

        std::size_t DataSize() const;
        std::size_t NonDataSize() const;
        std::size_t CompleteBufferSize() const;
        std::size_t GetCapacity() const;
        std::size_t GetBlockSize() const;
        std::size_t GetBlockAlignment() const;
    private:

        template <typename T>
        explicit FixedSizeAllocator(std::type_identity<T>, std::size_t capacity, const tstring& customOverflowMessage, std::optional<std::reference_wrapper<MemoryAllocator>> alloc):
            MemoryAllocator{OverflowMessage(sizeof(T), Alignment<T>(), capacity) + customOverflowMessage, alloc},
            m_Capacity{capacity},
            m_BlockSize{sizeof(T)},
            m_BlockAlignment{Alignment<T>()},
            m_pBegin{CreateBuffer()},
            m_pInUse{StartBoolBuffer()},
            m_FirstFreeObjectIndex{0}
        {
            assert(m_BlockSize % m_BlockAlignment == 0);

#ifndef NDEBUG
            std::memset(m_pBegin, FREE_DATA_PATTERN, DataSize());
            std::memset(m_pInUse, IN_USE_PATTERN, NonDataSize());
#endif // NDEBUG

            std::memset(m_pInUse, 0, sizeof(bool) * m_Capacity);
        }

        static constexpr uint8_t FREE_DATA_PATTERN = 0xAA;
        static constexpr uint8_t IN_USE_PATTERN = 0xBB;

        const std::size_t m_Capacity;
        const std::size_t m_BlockSize;
        const std::size_t m_BlockAlignment;

        std::byte* m_pBegin; // Reserved buffer
        bool* m_pInUse;

        std::optional<std::uint32_t> m_FirstFreeObjectIndex;

        void* GetAddressFromIndex(std::size_t index) const;
        bool IsInUse(std::uint32_t index) const;
        void SetInUse(std::uint32_t index, bool value) const;

        static tstring OverflowMessage(std::size_t blockSize, std::size_t alignment, std::size_t capacity);

        template <typename T>
        static constexpr std::size_t Alignment()
        {
            return std::max(alignof(T), alignof(bool));
        }

        std::byte* CreateBuffer() const;
        bool* StartBoolBuffer() const;
    };

    template <typename T, std::size_t N = 0>
    class TypeAllocator final : public FixedSizeAllocator
    {
    public:
        static_assert(N > 0, "Amount of objects must be greater than 0");
        explicit TypeAllocator():
            FixedSizeAllocator{std::type_identity<T>{}, N}
        {}
    };
    template <typename T>
    class TypeAllocator<T, 0> final : public FixedSizeAllocator
    {
    public:
        explicit TypeAllocator(std::size_t capacity):
            FixedSizeAllocator{std::type_identity<T>{}, capacity}
        {
            assert(capacity > 0 && "Amount of objects must be greater than 0");
        }
    };
}

#endif //FIXEDSIZEALLOCATORS_H
