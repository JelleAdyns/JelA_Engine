#include "SingleLinkAllocators.h"

#include <new>

#include "AudioService.h"

namespace jela
{
    void* SingleLinkAllocator::Acquire(std::size_t n)
    {
        if (n == 0) throw std::bad_alloc{};
        assert((m_pHead != nullptr));

        constexpr std::size_t blockSize { sizeof(Block) };
        const std::size_t requestedAllocation { n + sizeof(Header) };
        const std::size_t requestedBlocks { requestedAllocation / blockSize + (requestedAllocation % blockSize > 0 ? 1 : 0) };

        auto pPreviousBlock { m_pHead };
        auto pNextBlock { m_pHead->next };
        while (pNextBlock != nullptr && pNextBlock->blockCount < requestedBlocks)
        {
            pPreviousBlock = pNextBlock;
            pNextBlock = pNextBlock->next;
        }

        if (pNextBlock == nullptr) return AllocateOverflow(n);

        const Block* const pEnd {m_pHead + m_AmountOfBlocks};

        Block* const pChosenBlock {pNextBlock}; // defining other name to improve readability

        if (Block* const pNewBlock {pChosenBlock + requestedBlocks};
            pNewBlock != pEnd)
        {
            pNewBlock->blockCount = pChosenBlock->blockCount - requestedBlocks;
            pNewBlock->next = pChosenBlock->next; // the new startpoint should point to whatever the previous startpoint pointed to

            pChosenBlock->next = pNewBlock;
        }
        else pChosenBlock->next = nullptr;

        pChosenBlock->blockCount = requestedBlocks;

        pPreviousBlock->next = pChosenBlock->next; // the previous block should jump over the chosenblock to point to the next free data

#ifndef NDEBUG
        const auto header = reinterpret_cast<Header*>(pChosenBlock);
        std::memset(header + 1, ALLOC_PADDING_PATTERN, pChosenBlock->blockCount * blockSize - sizeof(Header));
        std::memset(header + 1, ALLOC_PATTERN, n);
#endif // NDEBUG

        return reinterpret_cast<Header*>(pChosenBlock) + 1; // Jump over the first Header
    }
    void SingleLinkAllocator::Release(void* p) noexcept
    {
        if (p == nullptr)
        {
            OutputDebugString(_T("Trying to release a nullptr! Returning...\n"));
            return;
        }

        auto* pBlockToRelease = reinterpret_cast<Block*> (static_cast<Header*> (p) - 1);

        if (const Block* start = m_pHead + 1;
            pBlockToRelease < start || pBlockToRelease > m_pHead + m_AmountOfBlocks)
        {
            if (TryDeallocateOverfow(p))
                return;

            OutputDebugString(_T("Pointer to release lays outside of buffer! Returning...\n"));
            return;
        }

        auto pPreviousBlock = m_pHead;
        auto pCurrentFreeBlock = pPreviousBlock->next;
        while (pCurrentFreeBlock != nullptr && pCurrentFreeBlock < pBlockToRelease)
        {
            pPreviousBlock = pCurrentFreeBlock;
            pCurrentFreeBlock = pCurrentFreeBlock->next;
        }

        // Merge released memory with the next free memory, if possible.
        MergeFreeMemory(pBlockToRelease, pCurrentFreeBlock);

        // Merge previous free block with the released memory, if possible.
        MergeFreeMemory(pPreviousBlock, pBlockToRelease);

    }
    std::size_t SingleLinkAllocator::RequestedSize() const
    {
        return m_RequestedBytes;
    }
    std::size_t SingleLinkAllocator::GetTotalBlocks() const
    {
        return m_AmountOfBlocks;
    }
    std::size_t SingleLinkAllocator::AmountOfDataBlocks() const
    {
        return m_AmountOfBlocks - 1;
    }
    std::size_t SingleLinkAllocator::CompleteBufferSize() const
    {
        return m_AmountOfBlocks * sizeof(Block);
    }
    std::size_t SingleLinkAllocator::AmountOfFreeBlocks() const
    {
        std::size_t total = 0;
        const Block* pCurrentBlock = m_pHead->next;
        while (pCurrentBlock != nullptr)
        {
            const auto pNextBlock = pCurrentBlock->next;
            total += pCurrentBlock->blockCount;
            pCurrentBlock = pNextBlock;
        }

        return total;
    }
    std::size_t SingleLinkAllocator::AmountOfOccupiedBlocks() const
    {
        return AmountOfDataBlocks() - AmountOfFreeBlocks();
    }
    void SingleLinkAllocator::MergeFreeMemory(Block* pBlockToMerge, Block* pNextBlock)
    {
        // if the block to release is immediately followed by a free block, then ...
        if (pNextBlock != nullptr && pBlockToMerge + pBlockToMerge->blockCount == pNextBlock)
        {
            // merge the free memory
            pBlockToMerge->blockCount += pNextBlock->blockCount;
            pBlockToMerge->next = pNextBlock->next;
        }
        // otherwise, point immediately to the next free memory
        else pBlockToMerge->next = pNextBlock;
    }
    tstring SingleLinkAllocator::OverflowMessage() const
    {
        return std::format(_T("SingleListAllocator with requested buffer size {}B (real size {}B) was full when trying "
                              "to allocate a memory. Continuing with 'std::malloc'.\n"),
            m_AmountOfBlocks * sizeof(Block), m_RequestedBytes);
    }
}
