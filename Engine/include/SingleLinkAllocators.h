#ifndef SINGLELINKALLOCATOR_H
#define SINGLELINKALLOCATOR_H

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>

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
            std::byte data[size - infoSize]{};

            static_assert(infoSize <= size);
        };

    public:
        explicit SingleLinkAllocator(std::size_t bufferSize):
            MemoryAllocator{},
            m_RequestedBytes{bufferSize},
            m_AmountOfBlocks{bufferSize / sizeof(Block) + (bufferSize % sizeof(Block) > 0 ? 1 : 0) + 1},
            m_pHead{new Block[m_AmountOfBlocks]{}}
        {
            assert(m_pHead != nullptr);

#ifndef NDEBUG
            std::memset(m_pHead, HEAD_PATTERN, sizeof(Block));
            std::memset(m_pHead + 1, FREE_DATA_PATTERN, sizeof(Block) * (m_AmountOfBlocks - 1));
#endif

            m_pHead->blockCount = 0;
            m_pHead->next = m_pHead + 1;

            m_pHead->next->blockCount = m_AmountOfBlocks - 1; //Exclude Head block
            m_pHead->next->next = nullptr;

        }
        ~SingleLinkAllocator() override
        {
            delete[] m_pHead;
        }

        void* Acquire(std::size_t n) override;
        void Release(void* p) noexcept override;

    private:
        static void MergeFreeMemory(Block* pBlockToMerge, Block* pNextBlock);

        static constexpr uint8_t FREE_DATA_PATTERN = 0xEE;
        static constexpr uint8_t HEAD_PATTERN = 0xFF;
        static constexpr uint8_t ALLOC_PATTERN = 0x00;
        static constexpr uint8_t ALLOC_PADDING_PATTERN = 0xFC;

        const std::size_t m_RequestedBytes;
        const std::size_t m_AmountOfBlocks;
        Block* const m_pHead;

        tstring OverflowMessage() const;
    };

    template <std::size_t BUFFER_SIZE>
    class BufferAllocator final : public SingleLinkAllocator
    {
    public:
        BufferAllocator():
            SingleLinkAllocator{BUFFER_SIZE}
        {
            static_assert(BUFFER_SIZE > 0, "Buffer size must be greater than 0.");
        }
    };
}
#endif //SINGLELINKALLOCATOR_H
