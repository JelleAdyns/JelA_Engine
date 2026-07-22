#ifndef SINGLELINKALLOCATOR_H
#define SINGLELINKALLOCATOR_H

#include <cassert>
#include <cstddef>
#include <cstdint>

#include "MemoryAllocator.h"

namespace jela
{

    class SingleLinkAllocator : public MemoryAllocator
    {
        struct Header
        {
            std::size_t blockCount{};
        };
        struct Block final : Header
        {
            static constexpr std::size_t size = 32;
            static constexpr std::size_t infoSize = sizeof(Header) + sizeof(Block*);

            Block* next {nullptr};
            uint8_t data[size - infoSize]{};

            static_assert(infoSize <= size);
        };

    public:
        explicit SingleLinkAllocator(std::size_t bufferSize, bool allowLargerBuffer = true):
            SingleLinkAllocator{bufferSize, allowLargerBuffer, _T(""), std::nullopt}
        {}

        explicit SingleLinkAllocator(std::size_t bufferSize, bool allowLargerBuffer, const tstring& customOverflowMessage):
            SingleLinkAllocator{bufferSize, allowLargerBuffer, customOverflowMessage, std::nullopt}
        {}

        explicit SingleLinkAllocator(std::size_t bufferSize, bool allowLargerBuffer, MemoryAllocator& alloc):
            SingleLinkAllocator{bufferSize, allowLargerBuffer, _T(""), std::optional<std::reference_wrapper<MemoryAllocator>>{alloc}}
        {}

        explicit SingleLinkAllocator(std::size_t bufferSize, bool allowLargerBuffer, const tstring& customOverflowMessage, MemoryAllocator& alloc):
            SingleLinkAllocator{bufferSize, allowLargerBuffer, customOverflowMessage, std::optional<std::reference_wrapper<MemoryAllocator>>{alloc}}
        {}

        SingleLinkAllocator(const SingleLinkAllocator& other) = delete;
        SingleLinkAllocator& operator=(const SingleLinkAllocator& other) = delete;

        SingleLinkAllocator(SingleLinkAllocator&& other) noexcept = default;      // Defined
        SingleLinkAllocator& operator=(SingleLinkAllocator&& other) noexcept = delete;

        ~SingleLinkAllocator() override
        {
            if (GetOptionalMemoryAllocator().has_value())
                operator delete(m_pHead, GetOptionalMemoryAllocator().value());
            else delete[] m_pHead;
        }

        void* Acquire(std::size_t n) override;
        void Release(void* p) noexcept override;

        std::size_t RequestedSize() const;
        std::size_t GetTotalBlocks() const;
        std::size_t AmountOfDataBlocks() const;
        std::size_t CompleteBufferSize() const;
        std::size_t AmountOfFreeBlocks() const;
        std::size_t AmountOfOccupiedBlocks() const;
        static constexpr std::size_t BLOCK_SIZE = sizeof(Block);

        static constexpr std::size_t MINIMUM_SIZE = sizeof(Block) * 2;

    private:

        explicit SingleLinkAllocator(std::size_t bufferSize, bool allowLargerBuffer, const tstring& customOverflowMessage, std::optional<std::reference_wrapper<MemoryAllocator>> alloc):
            MemoryAllocator{ OverflowMessage(bufferSize) + customOverflowMessage, alloc},
            m_RequestedBytes{bufferSize},
            m_AmountOfBlocks{ allowLargerBuffer ?
                bufferSize / BLOCK_DATA_SIZE + (bufferSize % BLOCK_DATA_SIZE > 0 ? 1 : 0) + 1 :
                bufferSize / sizeof(Block)
            },
            m_pHead{ CreateBuffer() }
        {
            assert(m_pHead != nullptr);

            if (!allowLargerBuffer && bufferSize < MINIMUM_SIZE)
            {
                delete [] m_pHead;
                throw std::length_error{std::format("Couldn't allocate the minimum required size ({}B). "
                                                    "Either allow for a larger buffer or request a larger buffersize.",
                                                    MINIMUM_SIZE)};
            }

#ifndef NDEBUG
            std::memset(m_pHead, HEAD_PATTERN, sizeof(Block));
            std::memset(m_pHead + 1, FREE_DATA_PATTERN, sizeof(Block) * (m_AmountOfBlocks - 1));
#endif

            m_pHead->blockCount = 0;
            m_pHead->next = m_pHead + 1;

            m_pHead->next->blockCount = m_AmountOfBlocks - 1; //Exclude Head block
            m_pHead->next->next = nullptr;

        }

        static void MergeFreeMemory(Block* pBlockToMerge, Block* pNextBlock);

        static constexpr uint8_t FREE_DATA_PATTERN = 0xEE;
        static constexpr uint8_t HEAD_PATTERN = 0xFF;
        static constexpr uint8_t ALLOC_PATTERN = 0x00;
        static constexpr uint8_t ALLOC_PADDING_PATTERN = 0xFC;

        // subtracting Header size from Block size because the header does not always count as data
        static constexpr std::size_t BLOCK_DATA_SIZE = sizeof(Block) - sizeof(Header);

        const std::size_t m_RequestedBytes;
        const std::size_t m_AmountOfBlocks;
        Block* const m_pHead;

        static tstring OverflowMessage(std::size_t requestedBytes);
        Block* CreateBuffer() const;
    };

    template <std::size_t BUFFER_SIZE, bool ALLOW_LARGER = true>
    class BufferAllocator final : public SingleLinkAllocator
    {
    public:
        BufferAllocator():
            SingleLinkAllocator{BUFFER_SIZE, ALLOW_LARGER}
        {
            static_assert(BUFFER_SIZE > 0, "Buffer size must be greater than 0.");
            if constexpr (!ALLOW_LARGER)
                static_assert(BUFFER_SIZE >= MINIMUM_SIZE, "Buffer size must be greater than or equal to the minimum size.");
        }
    };
}
#endif //SINGLELINKALLOCATOR_H
