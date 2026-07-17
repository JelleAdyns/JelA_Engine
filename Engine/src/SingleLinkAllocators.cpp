#include "SingleLinkAllocators.h"

#include <new>

#include "AudioService.h"

namespace jela
{
    void* SingleLinkAllocator::Acquire(std::size_t n)
    {
        if (n == 0) throw std::bad_alloc{};
        assert((m_pHead != nullptr));

        constexpr std::size_t blockSize = sizeof(Block);
        const std::size_t requestedAllocation = n + sizeof(Header);
        const std::size_t requestedBlocks = requestedAllocation / blockSize + (requestedAllocation % blockSize > 0 ? 1 : 0);

        auto pPreviousBlock = m_pHead;
        auto pNextBlock = m_pHead->next;
        while (pNextBlock != nullptr && pNextBlock->blockCount < requestedBlocks)
        {
            pPreviousBlock = pNextBlock;
            pNextBlock = pNextBlock->next;
        }

        if (pNextBlock == nullptr) throw std::bad_alloc();

        Block* pChosenBlock = pNextBlock; // defining other name to improve readability

        Block* pNewBlock = pChosenBlock + requestedBlocks; // creating a new startpoint of free data
        pNewBlock->blockCount = pChosenBlock->blockCount - requestedBlocks;
        pNewBlock->next = pChosenBlock->next; // the new startpoint should point to whatever the previous startpoint pointed to

        pChosenBlock->blockCount = requestedBlocks;
        pChosenBlock->next = pNewBlock;

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
        auto* pBlockToRelease = reinterpret_cast<Block*> (static_cast<Header*> (p) - 1);

        if (const Block* start = m_pHead + 1;
            pBlockToRelease < start || pBlockToRelease > m_pHead + m_AmountOfBlocks)
        {
            OutputDebugString(_T("Pointer to relaese lays outside of buffer! Returning..."));
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
}
