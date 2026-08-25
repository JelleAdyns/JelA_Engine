#include <gtest/gtest.h>

#include "Component.h"
#include "FixedSizeAllocators.h"


namespace jela
{

    class DerivedCompA : Component
    {

    public:

        DerivedCompA()
        {
            std::cout << "DerivedComp::DerivedCompA()" << std::endl;
            y = MAX_AMOUNT;
        }
        ~DerivedCompA() override
        {
            std::cout << "DerivedComp::~DerivedCompA()" << std::endl;
        }
        static constexpr std::size_t MAX_AMOUNT{ 100 };

        double x{};
        int y{};
        bool z{};
        Component* p{};
    };

    class SmallAmountA : DerivedCompA
    {
    public:
        static constexpr std::size_t MAX_AMOUNT{ 1 };
    };

    class LargeAmountA : DerivedCompA
    {
    public:
        static constexpr std::size_t MAX_AMOUNT{ 1'000'000 };
    };

    class DerivedCompB : Component
    {

    public:

        DerivedCompB()
        {
            std::cout << "DerivedComp::DerivedCompB()" << std::endl;
        }
        ~DerivedCompB() override
        {
            std::cout << "DerivedComp::~DerivedCompB()" << std::endl;
        }
        static constexpr std::size_t MAX_AMOUNT{ 100 };
    };
    class SmallAmountB : DerivedCompA
    {
    public:
        static constexpr std::size_t MAX_AMOUNT{ 1 };
    };

    class LargeAmountB : DerivedCompA
    {
    public:
        static constexpr std::size_t MAX_AMOUNT{ 1'000'000 };
    };

    constexpr std::size_t BLOCK_SIZE = sizeof(DerivedCompA);
    constexpr std::size_t BUFFER_SIZE = 20;
    constexpr std::size_t AMOUNT_OF_OVERFLOW_ALLOCATIONS{ 10 };

    void TestSingleAllocation(FixedSizeAllocator& alloc)
    {
        EXPECT_THROW(alloc.Acquire(BLOCK_SIZE - 1), std::length_error);
        EXPECT_THROW(alloc.Acquire(BLOCK_SIZE + 1), std::length_error);

        void* p{};
        EXPECT_NO_THROW(p = alloc.Acquire(BLOCK_SIZE));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, BLOCK_SIZE);

        EXPECT_NO_THROW(alloc.Release(p));
    }

    void TestInvalidRelease(FixedSizeAllocator& alloc)
    {
        void* p{nullptr};
        EXPECT_NO_THROW(alloc.Release(p));
        p = new char{'e'};

        EXPECT_NO_THROW(alloc.Release(p));
        EXPECT_EQ((*static_cast<char*>(p)), 'e');

        delete static_cast<char*>(p);
    }

    void TestTwoAllocations(FixedSizeAllocator& alloc)
    {
        void* p1{};
        EXPECT_NO_THROW((p1 = alloc.Acquire(BLOCK_SIZE)));
        EXPECT_NE(p1, nullptr);
        std::memset(p1, 1, BLOCK_SIZE);

        void* p2{};
        EXPECT_NO_THROW((p2 = alloc.Acquire(BLOCK_SIZE)));
        EXPECT_NE(p2, nullptr);
        std::memset(p2, 1, BLOCK_SIZE);


        EXPECT_NO_THROW(alloc.Release(p1));
        EXPECT_NO_THROW(alloc.Release(p2));
    }

    void TestFillAllocator(FixedSizeAllocator& alloc, std::span<void*> pointers)
    {
        for (size_t i = 0; i < pointers.size() ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(BLOCK_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), BLOCK_SIZE);
        }
        for (const auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));
    }


    void TestOverflow(FixedSizeAllocator& alloc, std::span<void*> pointers)
    {
        for (size_t i = 0; i < pointers.size() ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(BLOCK_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), BLOCK_SIZE);
        }

        EXPECT_TRUE(alloc.IsOverflown());
        EXPECT_EQ(alloc.AmountOfOverflowAllocations(), AMOUNT_OF_OVERFLOW_ALLOCATIONS);

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

        EXPECT_FALSE(alloc.IsOverflown());
    }

    void TestReleaseInMiddle(FixedSizeAllocator& alloc, std::span<void*> pointers)
    {
        for (size_t i = 0; i < pointers.size(); i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(BLOCK_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), BLOCK_SIZE);
        }
        EXPECT_FALSE(alloc.IsOverflown());

        const std::size_t middleIndex{pointers.size()/2 };
        EXPECT_NO_THROW(alloc.Release(pointers[middleIndex]));

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

    }

    void TestNewAndDeleteOperators(FixedSizeAllocator& alloc)
    {
        EXPECT_THROW(operator new (BLOCK_SIZE - 1, alloc), std::length_error);
        EXPECT_THROW(operator new (BLOCK_SIZE + 1, alloc), std::length_error);

        void* p{};

        EXPECT_NO_THROW(p = operator new (BLOCK_SIZE, alloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, BLOCK_SIZE);

        EXPECT_NO_THROW(operator delete(p, alloc));


        EXPECT_THROW(operator new[](BLOCK_SIZE - 1, alloc), std::length_error);
        EXPECT_THROW(operator new[](BLOCK_SIZE + 1, alloc), std::length_error);


        EXPECT_NO_THROW(p = operator new[](BLOCK_SIZE, alloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, BLOCK_SIZE);

        EXPECT_NO_THROW(operator delete[](p, alloc));
    }

    void TestNewAndDelete(FixedSizeAllocator& alloc)
    {
        DerivedCompA* pDC {nullptr};
        EXPECT_NO_THROW(pDC = new (alloc) DerivedCompA{});
        EXPECT_NE(pDC, nullptr);
        EXPECT_EQ(pDC->y, DerivedCompA::MAX_AMOUNT);

        if (pDC) pDC->x = 1234;

        EXPECT_NO_THROW(operator delete (pDC, alloc));

        pDC = nullptr;

        EXPECT_THROW(pDC = new (alloc) DerivedCompA[2]{}, std::length_error);
        EXPECT_EQ(pDC, nullptr);

        EXPECT_NO_THROW(operator delete (pDC, alloc));
    }

    void TestCompleteBufferSize(const FixedSizeAllocator& alloc)
    {
        const auto blockSize = alloc.GetBlockSize();
        const auto bufferSize = alloc.GetCapacity();
        const auto completeSize = alloc.CompleteBufferSize();
        const auto objectBufferSize = bufferSize * blockSize;
        EXPECT_GT(completeSize, objectBufferSize);
        EXPECT_GE(completeSize, objectBufferSize + bufferSize * sizeof(bool));
        EXPECT_LE(completeSize, objectBufferSize * 2);
    }

    // -------------------------------------------------------------------------------------------------------
    // FixedSizeAllocator
    TEST(FixedSizeAllocatorTests, SingleAllocation)
    {
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestSingleAllocation(alloc);
    }

    TEST(FixedSizeAllocatorTests, InvalidRelease)
    {
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestInvalidRelease(alloc);
    }

    TEST(FixedSizeAllocatorTests, TwoAllocations)
    {
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestTwoAllocations(alloc);
    }

    TEST(FixedSizeAllocatorTests, FillAllocator)
    {
        void* pointers[BUFFER_SIZE]{};
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestFillAllocator(alloc, pointers);
    }

    TEST(FixedSizeAllocatorTests, Overflow)
    {
        void* pointers[BUFFER_SIZE + AMOUNT_OF_OVERFLOW_ALLOCATIONS]{};
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestOverflow(alloc, pointers);
    }

    TEST(FixedSizeAllocatorTests, ReleaseInMiddle)
    {
        void* pointers[BUFFER_SIZE]{};
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestReleaseInMiddle(alloc, pointers);
    }

    TEST(FixedSizeAllocatorTests, NewAndDeleteOperator)
    {
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestNewAndDeleteOperators(alloc);
    }

    TEST(FixedSizeAllocatorTests, NewAndDelete)
    {
        FixedSizeAllocator alloc{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestNewAndDelete(alloc);
    }

    TEST(FixedSizeAllocatorTests, CompleteBufferSize)
    {
        const FixedSizeAllocator alloc1{std::type_identity<DerivedCompA>{}, BUFFER_SIZE};
        TestCompleteBufferSize(alloc1);
        const FixedSizeAllocator alloc2{std::type_identity<DerivedCompA>{}, 1};
        TestCompleteBufferSize(alloc2);
        const FixedSizeAllocator alloc3{std::type_identity<DerivedCompA>{}, 1'000'000};
        TestCompleteBufferSize(alloc3);

        const FixedSizeAllocator alloc4{std::type_identity<bool>{}, BUFFER_SIZE};
        TestCompleteBufferSize(alloc4);
        const FixedSizeAllocator alloc5{std::type_identity<bool>{}, 1};
        TestCompleteBufferSize(alloc5);
        const FixedSizeAllocator alloc6{std::type_identity<bool>{}, 1'000'000};
        TestCompleteBufferSize(alloc6);
    }
    // -------------------------------------------------------------------------------------------------------

    // -------------------------------------------------------------------------------------------------------
    // TypeAllocator
    TEST(TypeAllocatorTests, SingleAllocation)
    {
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestSingleAllocation(alloc);
    }

    TEST(TypeAllocatorTests, InvalidRelease)
    {
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestInvalidRelease(alloc);
    }

    TEST(TypeAllocatorTests, TwoAllocations)
    {
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestTwoAllocations(alloc);
    }

    TEST(TypeAllocatorTests, FillAllocator)
    {
        void* pointers[BUFFER_SIZE]{};
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestFillAllocator(alloc, pointers);
    }

    TEST(TypeAllocatorTests, Overflow)
    {
        void* pointers[BUFFER_SIZE + AMOUNT_OF_OVERFLOW_ALLOCATIONS]{};
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestOverflow(alloc, pointers);
    }

    TEST(TypeAllocatorTests, ReleaseInMiddle)
    {
        void* pointers[BUFFER_SIZE]{};
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestReleaseInMiddle(alloc, pointers);
    }

    TEST(TypeAllocatorTests, NewAndDeleteOperator)
    {
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestNewAndDeleteOperators(alloc);
    }

    TEST(TypeAllocatorTests, NewAndDelete)
    {
        TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc{};
        TestNewAndDelete(alloc);
    }

    TEST(TypeAllocatorTests, CompleteBufferSize)
    {
        const TypeAllocator<DerivedCompA, BUFFER_SIZE> alloc1{};
        TestCompleteBufferSize(alloc1);
        const TypeAllocator<DerivedCompA, 1> alloc2{};
        TestCompleteBufferSize(alloc2);
        const TypeAllocator<DerivedCompA, 1'000'000> alloc3{};
        TestCompleteBufferSize(alloc3);

        const TypeAllocator<bool, BUFFER_SIZE> alloc4{};
        TestCompleteBufferSize(alloc4);
        const TypeAllocator<bool, 1> alloc5{};
        TestCompleteBufferSize(alloc5);
        const TypeAllocator<bool, 1'000'000> alloc6{};
        TestCompleteBufferSize(alloc6);
    }
    // -------------------------------------------------------------------------------------------------------

    // -------------------------------------------------------------------------------------------------------
    // ComponentAllocator
    TEST(ComponentAllocatorTests, SingleAllocation)
    {
        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestSingleAllocation(alloc);
    }

    TEST(ComponentAllocatorTests, InvalidRelease)
    {
        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestInvalidRelease(alloc);
    }

    TEST(ComponentAllocatorTests, TwoAllocations)
    {
        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestTwoAllocations(alloc);
    }

    TEST(ComponentAllocatorTests, FillAllocator)
    {
        constexpr size_t amountOfAllocs{ Component::GetMaxAmount<DerivedCompA>() };
        void* pointers[amountOfAllocs]{};

        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestFillAllocator(alloc, pointers);
    }

    TEST(ComponentAllocatorTests, Overflow)
    {
        constexpr size_t amountOfAllocs{ Component::GetMaxAmount<DerivedCompA>() + AMOUNT_OF_OVERFLOW_ALLOCATIONS };
        void* pointers[amountOfAllocs]{};

        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestOverflow(alloc, pointers);
    }

    TEST(ComponentAllocatorTests, ReleaseInMiddle)
    {
        constexpr size_t amountOfAllocs{ Component::GetMaxAmount<DerivedCompA>() };
        void* pointers[amountOfAllocs]{};

        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestReleaseInMiddle(alloc, pointers);
    }

    TEST(ComponentAllocatorTests, NewAndDeleteOperator)
    {
        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestNewAndDeleteOperators(alloc);
    }

    TEST(ComponentAllocatorTests, NewAndDelete)
    {
        ComponentAllocator alloc{std::type_identity<DerivedCompA>{}};
        TestNewAndDelete(alloc);
    }

    TEST(ComponentAllocatorTests, CompleteBufferSize)
    {
        const ComponentAllocator alloc1{std::type_identity<DerivedCompA>{}};
        TestCompleteBufferSize(alloc1);
        const ComponentAllocator alloc2{std::type_identity<SmallAmountA>{}};
        TestCompleteBufferSize(alloc2);
        const ComponentAllocator alloc3{std::type_identity<LargeAmountA>{}};
        TestCompleteBufferSize(alloc3);

        const ComponentAllocator alloc4{std::type_identity<DerivedCompB>{}};
        TestCompleteBufferSize(alloc4);
        const ComponentAllocator alloc5{std::type_identity<SmallAmountB>{}};
        TestCompleteBufferSize(alloc5);
        const ComponentAllocator alloc6{std::type_identity<LargeAmountB>{}};
        TestCompleteBufferSize(alloc6);
    }
    // -------------------------------------------------------------------------------------------------------

}
