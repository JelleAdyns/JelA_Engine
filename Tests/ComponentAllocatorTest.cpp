#include <gtest/gtest.h>

#include "Component.h"
#include "ComponentAllocator.h"
#if __GNUC__
// ignoring some gcc warnings because we are deliberately messing with memory
#pragma GCC diagnostic ignored "-Wdelete-incomplete"
#ifndef __clang__
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#endif
#endif
namespace jela
{

    class DerivedComp final : Component
    {

    public:

        DerivedComp()
        {
            std::cout << "DerivedComp::DerivedComp()" << std::endl;
            y = MAX_AMOUNT;
        }
        ~DerivedComp() override
        {
            std::cout << "DerivedComp::~DerivedComp()" << std::endl;
        }
        static constexpr std::size_t MAX_AMOUNT{ 100 };

        double x{};
        int y{};
        bool z{};
        Component* p{};
    };

    constexpr auto BLOCK_SIZE = sizeof(DerivedComp);

    TEST(component_allocator_test, Single_Allocation)
    {
        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

        EXPECT_THROW(alloc.Acquire(BLOCK_SIZE - 1), std::length_error);
        EXPECT_THROW(alloc.Acquire(BLOCK_SIZE + 1), std::length_error);

        void* p{};
        EXPECT_NO_THROW(p = alloc.Acquire(BLOCK_SIZE));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, BLOCK_SIZE);

        EXPECT_NO_THROW(alloc.Release(p));
    }

    TEST(component_allocator_test, Invalid_Release)
    {
        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

        void* p{nullptr};
        EXPECT_NO_THROW(alloc.Release(p));
        p = new char{'e'};

        EXPECT_NO_THROW(alloc.Release(p));
        EXPECT_EQ((*static_cast<char*>(p)), 'e');

        delete static_cast<char*>(p);
    }

    TEST(component_allocator_test, Two_Allocations)
    {
        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};


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

    TEST(component_allocator_test, Fill_Allocator)
    {
        constexpr size_t amountOfAllocs{ Component::GetAmount<DerivedComp>() };
        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

        void* pointers[amountOfAllocs]{};
        for (size_t i = 0; i < amountOfAllocs ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(BLOCK_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), BLOCK_SIZE);
        }
        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));
    }


    TEST(component_allocator_test, Overflow)
    {
        constexpr size_t amountOfOverflowAllocations{ 10 };
        constexpr size_t amountOfAllocs{ Component::GetAmount<DerivedComp>() + amountOfOverflowAllocations };

        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

        void* pointers[amountOfAllocs]{};
        for (size_t i = 0; i < amountOfAllocs ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(BLOCK_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), BLOCK_SIZE);
        }

        EXPECT_TRUE(alloc.IsOverflown());
        EXPECT_EQ(alloc.AmountOfOverflowAllocations(), amountOfOverflowAllocations);

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

        EXPECT_FALSE(alloc.IsOverflown());
    }

    TEST(component_allocator_test, Release_In_Middle)
    {
        constexpr size_t amountOfAllocs{ Component::GetAmount<DerivedComp>()};

        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

        void* pointers[amountOfAllocs]{};
        for (size_t i = 0; i < amountOfAllocs ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(BLOCK_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), BLOCK_SIZE);
        }
        EXPECT_FALSE(alloc.IsOverflown());

        constexpr size_t middleIndex{amountOfAllocs/2 };
        EXPECT_NO_THROW(alloc.Release(pointers[middleIndex]));

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

    }

    TEST(component_allocator_test, New_And_Delete_Operator)
    {
        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

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

    TEST(component_allocator_test, New_And_Delete)
    {
        ComponentAllocator alloc{std::type_identity<DerivedComp>{}};

        DerivedComp* pDC {nullptr};
        EXPECT_NO_THROW(pDC = new (alloc) DerivedComp{});
        EXPECT_NE(pDC, nullptr);
        EXPECT_EQ(pDC->y, DerivedComp::MAX_AMOUNT);

        if (pDC) pDC->x = 1234;

        EXPECT_NO_THROW(operator delete (pDC, alloc));

        pDC = nullptr;

        EXPECT_THROW(pDC = new (alloc) DerivedComp[2]{}, std::length_error);
        EXPECT_EQ(pDC, nullptr);

        EXPECT_NO_THROW(operator delete (pDC, alloc));
    }

}
