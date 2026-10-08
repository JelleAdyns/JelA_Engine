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


    using DefaultBufferAllocator = BufferAllocator<BUFFER_SIZE>;

    enum TestableSizes
    {
        Default = BUFFER_SIZE,
        Small = 1,
        Minimum = SingleLinkAllocator::MINIMUM_BUFFER_SIZE,
        Large = 1'000'000
    };


    template <typename AllocT, TestableSizes SizeVal, bool AllowLargerBufferVal>
       requires std::is_base_of_v<SingleLinkAllocator, AllocT>
    class AllocType
    {
    public:
        using Alloc = AllocT;
        static constexpr TestableSizes Size = SizeVal;
        static constexpr bool AllowLargerBuffer = AllowLargerBufferVal;
    };

    template <typename T>
    class TestSingleLinkAllocators : public testing::Test
    {
    public:

        using Alloc = typename T::Alloc;
        static constexpr auto Size = T::Size;
        static constexpr bool AllowLargerBuffer = T::AllowLargerBuffer;

        void SetUp() override
        {
            if constexpr (std::is_same_v<Alloc, SingleLinkAllocator>)
            {
                if constexpr (!AllowLargerBuffer && Size == Small)
                {
                    EXPECT_THROW(pAlloc = std::make_unique<SingleLinkAllocator>(Size, AllowLargerBuffer), std::length_error);
                    return;
                }
                pAlloc = std::make_unique<SingleLinkAllocator>(Size, AllowLargerBuffer);
            }

            else if constexpr (std::is_same_v<Alloc, DefaultBufferAllocator>)
                pAlloc = std::make_unique<BufferAllocator<Size, AllowLargerBuffer>>();



            tstring debugName{};
            if constexpr (std::is_same_v<Alloc, SingleLinkAllocator>) debugName = _T("SingleList");
            else if constexpr (std::is_same_v<Alloc, DefaultBufferAllocator>) debugName = _T("BufferList");

            OutputDebugString(std::format(_T("TEST: {} - Size {}, AllowLargerBuffer {}\n"), debugName, static_cast<int>(Size), static_cast<bool>(AllowLargerBuffer)).c_str());
        }
        std::unique_ptr<SingleLinkAllocator> pAlloc{nullptr};
    };


     using DefaultAllocatorTypes = testing::Types<
        AllocType<SingleLinkAllocator, Default, false>,
        AllocType<SingleLinkAllocator, Default, true>,
        AllocType<SingleLinkAllocator, Small, false>,
        AllocType<SingleLinkAllocator, Small, true>,
        AllocType<SingleLinkAllocator, Minimum, false>,
        AllocType<SingleLinkAllocator, Minimum, true>,
        AllocType<SingleLinkAllocator, Large, false>,
        AllocType<SingleLinkAllocator, Large, true>,

        AllocType<DefaultBufferAllocator, Default, false>,
        AllocType<DefaultBufferAllocator, Default, true>,
        //AllocType<DefaultBufferAllocator, Small, false>, // Compile time error as intended
        AllocType<DefaultBufferAllocator, Small, true>,
        AllocType<DefaultBufferAllocator, Minimum, false>,
        AllocType<DefaultBufferAllocator, Minimum, true>,
        AllocType<DefaultBufferAllocator, Large, false>,
        AllocType<DefaultBufferAllocator, Large, true>
    >;

    class SingleLinkTestNames {
    public:
        template <typename T>
        static std::string GetName(int)
        {
            using Alloc = typename T::Alloc;
            constexpr auto Size = T::Size;
            constexpr bool AllowLargerBuffer = T::AllowLargerBuffer;
            std::string name{};

            if constexpr (AllowLargerBuffer) name += "PaddedBuffer";
            else name += "CappedBuffer";

            name += "_X_";

            if constexpr (Size == Default) name += "DefaultSize";
            else if constexpr (Size == Small) name += "SmallSize";
            else if constexpr (Size == Minimum) name += "MinimumSize";
            else if constexpr (Size == Large) name += "LargeSize";

            name += "_X_";

            if constexpr (std::is_same_v<Alloc, SingleLinkAllocator>) name += "SingleLinkAllocator";
            else if constexpr (std::is_same_v<Alloc, DefaultBufferAllocator>) name += "BufferAllocator";

            return name;
        }
    };

    TYPED_TEST_SUITE(TestSingleLinkAllocators, DefaultAllocatorTypes, SingleLinkTestNames);

    TYPED_TEST(TestSingleLinkAllocators, Getters)
    {
        if (!this->pAlloc) return;
        const auto amountOfDataBlocks = this->pAlloc->AmountOfDataBlocks();

        EXPECT_EQ(amountOfDataBlocks, this->pAlloc->AmountOfFreeBlocks());
        EXPECT_EQ(this->pAlloc->AmountOfOccupiedBlocks(), 0);

        const auto requestedSize = this->pAlloc->RequestedSize();
        const auto totalSize = this->pAlloc->CompleteBufferSize();

        if constexpr (TypeParam::AllowLargerBuffer) EXPECT_GE(totalSize, requestedSize);
        else EXPECT_LE(totalSize, requestedSize);

        void* p = this->pAlloc->Acquire(1);
        EXPECT_EQ(this->pAlloc->AmountOfFreeBlocks(), amountOfDataBlocks - 1);

        this->pAlloc->Release(p);
        EXPECT_EQ(this->pAlloc->AmountOfFreeBlocks(), amountOfDataBlocks);

        if (amountOfDataBlocks > 1 )
        {
            p = this->pAlloc->Acquire((amountOfDataBlocks - 1) * SingleLinkAllocator::BLOCK_SIZE);
            EXPECT_EQ(this->pAlloc->AmountOfFreeBlocks(), 0);

            this->pAlloc->Release(p);
            EXPECT_EQ(this->pAlloc->AmountOfFreeBlocks(), amountOfDataBlocks);
        }
    }

    TYPED_TEST(TestSingleLinkAllocators, SingleAllocation)
    {
        if (!this->pAlloc) return;
        EXPECT_THROW(this->pAlloc->Acquire(0), std::bad_alloc);

        void* p{};
        EXPECT_NO_THROW(p = this->pAlloc->Acquire(STRUCT_SIZE));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, STRUCT_SIZE);

        EXPECT_NO_THROW(this->pAlloc->Release(p));
    }
    TYPED_TEST(TestSingleLinkAllocators, InvalidRelease)
    {
        if (!this->pAlloc) return;
        void* p{nullptr};
        EXPECT_NO_THROW(this->pAlloc->Release(p));
        p = new char{'e'};

        EXPECT_NO_THROW(this->pAlloc->Release(p));
        EXPECT_EQ((*static_cast<char*>(p)), 'e');

        delete static_cast<char*>(p);
    }

    TYPED_TEST(TestSingleLinkAllocators, TwoAllocations)
    {
        if (!this->pAlloc) return;
        constexpr std::size_t sizeA = 34;
        constexpr std::size_t sizeB = 45;

        // This test requires the allocator to have at least enough room for 2 allocations
        if constexpr (TypeParam::Size < sizeA + sizeB) return;
        EXPECT_GT(this->pAlloc->RequestedSize(), sizeA + sizeB);

        void* p1{};
        EXPECT_NO_THROW((p1 = this->pAlloc->Acquire(sizeA)));
        EXPECT_NE(p1, nullptr);
        std::memset(p1, 1, sizeA);

        void* p2{};
        EXPECT_NO_THROW((p2 = this->pAlloc->Acquire(sizeB)));
        EXPECT_NE(p2, nullptr);
        std::memset(p2, 1, sizeB);


        EXPECT_NO_THROW(this->pAlloc->Release(p1));
        EXPECT_NO_THROW(this->pAlloc->Release(p2));
    }

    TYPED_TEST(TestSingleLinkAllocators, FillAllocator)
    {
        if (!this->pAlloc) return;

        std::vector<void*> vecPointers{};
        constexpr std::size_t maxBlocks_variation1{ 6 };
        constexpr std::size_t maxBlocks_variation2{ maxBlocks_variation1 / 2 };
        while (this->pAlloc->AmountOfFreeBlocks() > 0)
        {
            const auto freeBlocks = this->pAlloc->AmountOfFreeBlocks();

            std::size_t allocSize{SingleLinkAllocator::BLOCK_SIZE};
            if (freeBlocks >= maxBlocks_variation1) allocSize *= maxBlocks_variation1;
            else if (freeBlocks >= maxBlocks_variation2) allocSize *= maxBlocks_variation2;
            allocSize -= SingleLinkAllocator::BLOCK_HEADER_SIZE;

            EXPECT_NO_THROW(vecPointers.emplace_back( this->pAlloc->Acquire(allocSize)));
            EXPECT_NE(vecPointers.back(), nullptr);
            std::memset(vecPointers.back(), static_cast<int>( vecPointers.size() - 1 ), allocSize);

        }
        EXPECT_FALSE(this->pAlloc->IsOverflown());
        for (const auto p : vecPointers)
            EXPECT_NO_THROW(this->pAlloc->Release(p));
    }

    TYPED_TEST(TestSingleLinkAllocators, Overflow)
    {
        if (!this->pAlloc) return;

        const auto allocationSize = this->pAlloc->AmountOfFreeBlocks() * SingleLinkAllocator::BLOCK_SIZE - SingleLinkAllocator::BLOCK_HEADER_SIZE;

        // Fill entire buffer - no overflow
        void* p {nullptr};
        EXPECT_NO_THROW(p = this->pAlloc->Acquire(allocationSize));
        EXPECT_NE(p, nullptr);
        std::memset(p, 0, allocationSize);
        EXPECT_FALSE(this->pAlloc->IsOverflown());

        // Cause first overflow
        void* pOverflown1{nullptr};
        EXPECT_NO_THROW(pOverflown1 = this->pAlloc->Acquire(1));
        EXPECT_NE(pOverflown1, nullptr);
        std::memset(pOverflown1, 1, 1);
        EXPECT_TRUE(this->pAlloc->IsOverflown());
        EXPECT_EQ(this->pAlloc->AmountOfOverflowAllocations(), 1);

        // Cause second overflow
        void* pOverflown2{nullptr};
        EXPECT_NO_THROW(pOverflown2 = this->pAlloc->Acquire(1));
        EXPECT_NE(pOverflown2, nullptr);
        std::memset(pOverflown2, 2, 1);
        EXPECT_TRUE(this->pAlloc->IsOverflown());
        EXPECT_EQ(this->pAlloc->AmountOfOverflowAllocations(), 2);

        // Release first overflow
        EXPECT_NO_THROW(this->pAlloc->Release(pOverflown1));
        EXPECT_TRUE(this->pAlloc->IsOverflown());
        EXPECT_EQ(this->pAlloc->AmountOfOverflowAllocations(), 1);

        // Release second overflow
        EXPECT_NO_THROW(this->pAlloc->Release(pOverflown2));
        EXPECT_FALSE(this->pAlloc->IsOverflown());
        EXPECT_EQ(this->pAlloc->AmountOfOverflowAllocations(), 0);

        // Release first allocation
        EXPECT_NO_THROW(this->pAlloc->Release(p));
    }

    TYPED_TEST(TestSingleLinkAllocators, ReleaseInMiddle)
    {
        if (!this->pAlloc) return;

        constexpr std::size_t amountOfAllocations {3};

        // Only able to release in middle if more than 3 usable blocks are a available
        if (this->pAlloc->AmountOfDataBlocks() < amountOfAllocations) return;

        const std::size_t allocSize = TypeParam::AllowLargerBuffer
        ? this->pAlloc->RequestedSize() / amountOfAllocations
        : this->pAlloc->AmountOfFreeBlocks() * SingleLinkAllocator::BLOCK_SIZE / amountOfAllocations -
            SingleLinkAllocator::BLOCK_HEADER_SIZE * amountOfAllocations;

        void* pointer1 = this->pAlloc->Acquire(allocSize);
        void* pointer1ToRelease = this->pAlloc->Acquire(allocSize);
        void* pointer2 = this->pAlloc->Acquire(allocSize);

        EXPECT_NO_THROW(this->pAlloc->Release(pointer1ToRelease));

        void* p {nullptr};
        EXPECT_NO_THROW(p = this->pAlloc->Acquire(allocSize*2));
        EXPECT_TRUE(this->pAlloc->IsOverflown());
        EXPECT_EQ(this->pAlloc->AmountOfOverflowAllocations(), 1);
        EXPECT_NO_THROW(this->pAlloc->Release(p));

        EXPECT_NO_THROW(this->pAlloc->Release(pointer1));
        EXPECT_NO_THROW(this->pAlloc->Release(pointer2));

    }
    TYPED_TEST(TestSingleLinkAllocators, NewAndDeleteOperators)
    {
        if (!this->pAlloc) return;
        EXPECT_THROW(operator new (0, *this->pAlloc), std::bad_alloc);

        void* p{};

        EXPECT_NO_THROW(p = operator new (STRUCT_SIZE, *this->pAlloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, STRUCT_SIZE);

        EXPECT_NO_THROW(operator delete(p, *this->pAlloc));

        EXPECT_THROW(operator new [](0, *this->pAlloc), std::bad_alloc);

        EXPECT_NO_THROW(p = operator new[](STRUCT_SIZE, *this->pAlloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, STRUCT_SIZE);

        EXPECT_NO_THROW(operator delete[](p, *this->pAlloc));
    }

    TYPED_TEST(TestSingleLinkAllocators, NewAndDelete)
    {
        if (!this->pAlloc) return;
        DataStruct* pDS {nullptr};
        EXPECT_NO_THROW(pDS = new (*this->pAlloc) DataStruct{});
        EXPECT_NE(pDS, nullptr);
        EXPECT_EQ(pDS->y, 3);

        if (pDS) pDS->x = 1234;

        EXPECT_NO_THROW(operator delete (pDS, *this->pAlloc));

        pDS = nullptr;

        EXPECT_NO_THROW(pDS = new (*this->pAlloc) DataStruct[2]{});
        EXPECT_NE(pDS, nullptr);
        EXPECT_EQ(pDS[0].y, 3);
        EXPECT_EQ(pDS[1].y, 3);
        EXPECT_NO_THROW(pDS[0].y = 1);
        EXPECT_NO_THROW(pDS[1].y = 2);
        EXPECT_NO_THROW(operator delete (pDS, *this->pAlloc));

        EXPECT_NO_THROW(pDS = new (*this->pAlloc) DataStruct[2]{});
        EXPECT_NE(pDS, nullptr);
        EXPECT_NO_THROW(pDS[0].~DataStruct());
        EXPECT_NO_THROW(pDS[1].~DataStruct());
        EXPECT_NO_THROW(operator delete [](pDS, *this->pAlloc));
    }

}
