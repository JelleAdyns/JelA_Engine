#include <gtest/gtest.h>

#include "SingleLinkAllocators.h"

namespace jela
{

    struct DataStruct
    {
        DataStruct()
        {
            y = 3;
        }
        double x{};
        int y{};
        bool z{};
        int* p{};
        double t{};
    };

    constexpr std::size_t STRUCT_SIZE = sizeof(DataStruct);
    constexpr std::size_t BUFFER_SIZE = 500;

    void TestSingleAllocation(SingleLinkAllocator& alloc)
    {
        EXPECT_THROW(alloc.Acquire(0), std::bad_alloc);

        void* p{};
        EXPECT_NO_THROW(p = alloc.Acquire(STRUCT_SIZE));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, STRUCT_SIZE);

        EXPECT_NO_THROW(alloc.Release(p));
    }

    void TestInvalidRelease(SingleLinkAllocator& alloc)
    {
        void* p{nullptr};
        EXPECT_NO_THROW(alloc.Release(p));
        p = new char{'e'};

        EXPECT_NO_THROW(alloc.Release(p));
        EXPECT_EQ((*static_cast<char*>(p)), 'e');

        delete static_cast<char*>(p);
    }

    void TestTwoAllocations(SingleLinkAllocator& alloc)
    {
        constexpr std::size_t sizeA = 34;
        constexpr std::size_t sizeB = 45;

        // This test requires the allocator to have at least enough room for 2 allocations
        EXPECT_GT(alloc.RequestedSize(), sizeA + sizeB);

        void* p1{};
        EXPECT_NO_THROW((p1 = alloc.Acquire(sizeA)));
        EXPECT_NE(p1, nullptr);
        std::memset(p1, 1, sizeA);

        void* p2{};
        EXPECT_NO_THROW((p2 = alloc.Acquire(sizeB)));
        EXPECT_NE(p2, nullptr);
        std::memset(p2, 1, sizeB);


        EXPECT_NO_THROW(alloc.Release(p1));
        EXPECT_NO_THROW(alloc.Release(p2));
    }

    void TestFillAllocator(SingleLinkAllocator& alloc)
    {
        const size_t nbPieces = alloc.RequestedSize() / SingleLinkAllocator::BLOCK_SIZE;
        const auto pointers = new void* [nbPieces];
        const size_t test_size = alloc.RequestedSize() / nbPieces;
        for (size_t i = 0; i < nbPieces ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(test_size));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), test_size);
        }
        for (size_t i = 0; i < nbPieces ; i++)
            EXPECT_NO_THROW(alloc.Release(pointers[i]));
        delete [] pointers;
    }


    void TestOverflow(SingleLinkAllocator& alloc)
    {
        constexpr std::size_t amountOfSmallAllocs = 5;
        void* pointers[amountOfSmallAllocs]{};
        for (size_t i = 0; i < amountOfSmallAllocs ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(STRUCT_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), STRUCT_SIZE);
        }

        const auto allocationSize = alloc.AmountOfFreeBlocks() * SingleLinkAllocator::BLOCK_SIZE;

        void* p = alloc.Acquire(allocationSize);

        EXPECT_TRUE(alloc.IsOverflown());
        EXPECT_EQ(alloc.AmountOfOverflowAllocations(), 1);

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

        alloc.Release(p);

        EXPECT_FALSE(alloc.IsOverflown());
    }

    void TestReleaseInMiddle(SingleLinkAllocator& alloc)
    {
        const std::size_t allocSize = alloc.AmountOfFreeBlocks() * SingleLinkAllocator::BLOCK_SIZE / 6;
        void* pointer1 = alloc.Acquire(allocSize);
        void* pointer1ToRelease = alloc.Acquire(allocSize);
        void* pointer2 = alloc.Acquire(allocSize);
        void* pointer2ToRelease = alloc.Acquire(allocSize);
        void* pointer3 = alloc.Acquire(allocSize);

        EXPECT_NO_THROW(alloc.Release(pointer1ToRelease));
        EXPECT_NO_THROW(alloc.Release(pointer2ToRelease));

        void* p {nullptr};
        EXPECT_NO_THROW(p = alloc.Acquire(allocSize*2));
        EXPECT_TRUE(alloc.IsOverflown());
        EXPECT_EQ(alloc.AmountOfOverflowAllocations(), 1);
        EXPECT_NO_THROW(alloc.Release(p));

        EXPECT_NO_THROW(alloc.Release(pointer1));
        EXPECT_NO_THROW(alloc.Release(pointer2));
        EXPECT_NO_THROW(alloc.Release(pointer3));

    }

    void TestNewAndDeleteOperators(SingleLinkAllocator& alloc)
    {
        EXPECT_THROW(operator new (0, alloc), std::bad_alloc);

        void* p{};

        EXPECT_NO_THROW(p = operator new (STRUCT_SIZE, alloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, STRUCT_SIZE);

        EXPECT_NO_THROW(operator delete(p, alloc));

        EXPECT_THROW(operator new [](0, alloc), std::bad_alloc);

        EXPECT_NO_THROW(p = operator new[](STRUCT_SIZE, alloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, STRUCT_SIZE);

        EXPECT_NO_THROW(operator delete[](p, alloc));
    }

    void TestNewAndDelete(SingleLinkAllocator& alloc)
    {
        DataStruct* pDS {nullptr};
        EXPECT_NO_THROW(pDS = new (alloc) DataStruct{});
        EXPECT_NE(pDS, nullptr);
        EXPECT_EQ(pDS->y, 3);

        if (pDS) pDS->x = 1234;

        EXPECT_NO_THROW(operator delete (pDS, alloc));

        pDS = nullptr;

        EXPECT_NO_THROW(pDS = new (alloc) DataStruct[2]{});
        EXPECT_NE(pDS, nullptr);
        EXPECT_EQ(pDS[0].y, 3);
        EXPECT_EQ(pDS[1].y, 3);
        EXPECT_NO_THROW(pDS[0].y = 1);
        EXPECT_NO_THROW(pDS[1].y = 2);
        EXPECT_NO_THROW(operator delete (pDS, alloc));

        EXPECT_NO_THROW(pDS = new (alloc) DataStruct[2]{});
        EXPECT_NE(pDS, nullptr);
        EXPECT_NO_THROW(pDS[0].~DataStruct());
        EXPECT_NO_THROW(pDS[1].~DataStruct());
        EXPECT_NO_THROW(operator delete [](pDS, alloc));
    }

    void TestGetters(SingleLinkAllocator& alloc, bool allowLargerBuffer)
    {
        const auto amountOfDataBlocks = alloc.AmountOfDataBlocks();

        EXPECT_EQ(amountOfDataBlocks, alloc.AmountOfFreeBlocks());
        EXPECT_EQ(alloc.AmountOfOccupiedBlocks(), 0);

        const auto requestedSize = alloc.RequestedSize();
        const auto totalSize = alloc.CompleteBufferSize();

        if (allowLargerBuffer) EXPECT_GE(totalSize, requestedSize);
        else EXPECT_LE(totalSize, requestedSize);

        void* p = alloc.Acquire(1);
        EXPECT_EQ(alloc.AmountOfFreeBlocks(), amountOfDataBlocks - 1);

        alloc.Release(p);
        EXPECT_EQ(alloc.AmountOfFreeBlocks(), amountOfDataBlocks);

        if (amountOfDataBlocks > 1 )
        {
            p = alloc.Acquire((amountOfDataBlocks - 1) * SingleLinkAllocator::BLOCK_SIZE);
            EXPECT_EQ(alloc.AmountOfFreeBlocks(), 0);

            alloc.Release(p);
            EXPECT_EQ(alloc.AmountOfFreeBlocks(), amountOfDataBlocks);
        }
    }

    // -------------------------------------------------------------------------------------------------------
    // SingleLinkAllocator
    TEST(SingleLinkAllocatorTests, SingleAllocation)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestSingleAllocation(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestSingleAllocation(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, InvalidRelease)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestInvalidRelease(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestInvalidRelease(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, TwoAllocations)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestTwoAllocations(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestTwoAllocations(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, FillAllocator)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestFillAllocator(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestFillAllocator(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, Overflow)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestOverflow(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestOverflow(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, ReleaseInMiddle)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestReleaseInMiddle(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestReleaseInMiddle(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, NewAndDeleteOperator)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestNewAndDeleteOperators(allocLarge);
        SingleLinkAllocator allocSmalll{BUFFER_SIZE, false};
        TestNewAndDeleteOperators(allocSmalll);
    }

    TEST(SingleLinkAllocatorTests, NewAndDelete)
    {
        SingleLinkAllocator allocLarge{BUFFER_SIZE};
        TestNewAndDelete(allocLarge);
        SingleLinkAllocator allocSmall{BUFFER_SIZE, false};
        TestNewAndDelete(allocSmall);
    }

    TEST(SingleLinkAllocatorTests, CompleteBufferSize)
    {
        SingleLinkAllocator alloc1{BUFFER_SIZE, true};
        TestGetters(alloc1, true);
        SingleLinkAllocator alloc2{1, true};
        TestGetters(alloc2, true);
        SingleLinkAllocator alloc3{1'000'000, true};
        TestGetters(alloc3, true);

        SingleLinkAllocator alloc4{BUFFER_SIZE, false};
        TestGetters(alloc4, false);
        SingleLinkAllocator * alloc5{nullptr};
        EXPECT_THROW(alloc5 = new SingleLinkAllocator(1, false), std::length_error);
        SingleLinkAllocator alloc6{SingleLinkAllocator::MINIMUM_SIZE, false};
        TestGetters(alloc6, false);
        SingleLinkAllocator alloc7{1'000'000, false};
        TestGetters(alloc7, false);

    }
    // -------------------------------------------------------------------------------------------------------

    // -------------------------------------------------------------------------------------------------------
    // BufferAllocator
    TEST(BufferAllocatorTests, SingleAllocation)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestSingleAllocation(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestSingleAllocation(allocSmall);
    }

    TEST(BufferAllocatorTests, InvalidRelease)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestInvalidRelease(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestInvalidRelease(allocSmall);
    }

    TEST(BufferAllocatorTests, TwoAllocations)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestTwoAllocations(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestTwoAllocations(allocSmall);
    }

    TEST(BufferAllocatorTests, FillAllocator)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestFillAllocator(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestFillAllocator(allocSmall);
    }

    TEST(BufferAllocatorTests, Overflow)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestOverflow(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestOverflow(allocSmall);
    }

    TEST(BufferAllocatorTests, ReleaseInMiddle)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestReleaseInMiddle(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestReleaseInMiddle(allocSmall);
    }

    TEST(BufferAllocatorTests, NewAndDeleteOperator)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestNewAndDeleteOperators(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestNewAndDeleteOperators(allocSmall);
    }

    TEST(BufferAllocatorTests, NewAndDelete)
    {
        BufferAllocator<BUFFER_SIZE> allocLarge{};
        TestNewAndDelete(allocLarge);
        BufferAllocator<BUFFER_SIZE, false> allocSmall{};
        TestNewAndDelete(allocSmall);
    }

    TEST(BufferAllocatorTests, CompleteBufferSize)
    {
        BufferAllocator<BUFFER_SIZE, true> alloc1{};
        TestGetters(alloc1, true);
        BufferAllocator<1, true> alloc2{};
        TestGetters(alloc2, true);
        BufferAllocator<1'000'000, true> alloc3{};
        TestGetters(alloc3, true);

        BufferAllocator<BUFFER_SIZE, false> alloc4{};
        TestGetters(alloc4, false);
        // BufferAllocator<1, false> alloc5{};
        // TestGetters(alloc5, false);
        BufferAllocator<SingleLinkAllocator::MINIMUM_SIZE, false> alloc5{};
        TestGetters(alloc5, false);
        BufferAllocator<1'000'000, false> alloc6{};
        TestGetters(alloc6, false);
    }
    // -------------------------------------------------------------------------------------------------------


}
