#include <gtest/gtest.h>

#include "Component.h"
#include "FixedSizeAllocators.h"


namespace jela
{
    constexpr static std::size_t BUFFER_SIZE = 20;
    constexpr static std::size_t AMOUNT_OF_OVERFLOW_ALLOCATIONS{ 10 };

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
        static constexpr std::size_t MAX_AMOUNT{ BUFFER_SIZE };

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

    constexpr static std::size_t DERIVED_A_SIZE = sizeof(DerivedCompA);

    class DerivedCompB : Component
    {
    public:
        DerivedCompB() { std::cout << "DerivedComp::DerivedCompB()" << std::endl; }
        ~DerivedCompB() override { std::cout << "DerivedComp::~DerivedCompB()" << std::endl; }
        static constexpr std::size_t MAX_AMOUNT{ BUFFER_SIZE };
    };

    class SmallAmountB : DerivedCompB
    {
    public:
        static constexpr std::size_t MAX_AMOUNT{ 1 };
    };

    class LargeAmountB : DerivedCompB
    {
    public:
        static constexpr std::size_t MAX_AMOUNT{ 1'000'000 };
    };


    using DefaultTypeAllocator = TypeAllocator<DerivedCompA, BUFFER_SIZE>;


    template <typename T>
        requires std::is_base_of_v<FixedSizeAllocator, T>
    class TestFixedSizeAllocators : public testing::Test
    {
    public:
        void SetUp() override
        {
            if constexpr (std::is_same_v<T, FixedSizeAllocator>)
                pAlloc = std::make_unique<FixedSizeAllocator>(std::type_identity<DerivedCompA>{}, BUFFER_SIZE);

            else if constexpr (std::is_same_v<T, ComponentAllocator>)
                pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<DerivedCompA>{});

            else if constexpr (std::is_same_v<T, DefaultTypeAllocator>)
                pAlloc = std::make_unique<T>();
        }
        std::unique_ptr<FixedSizeAllocator> pAlloc{nullptr};
    };


    using DefaultAllocatorTypes = testing::Types<FixedSizeAllocator, ComponentAllocator, DefaultTypeAllocator>;
    class FixedSizeNames {
    public:
        template <typename T>
        static constexpr std::string GetName(int)
        {
            if constexpr (std::is_same_v<T, FixedSizeAllocator>) return "FixedSizeAllocator";
            if constexpr (std::is_same_v<T, ComponentAllocator>) return "ComponentAllocator";
            if constexpr (std::is_same_v<T, DefaultTypeAllocator>) return "TypeAllocator";
        }
    };

    TYPED_TEST_SUITE(TestFixedSizeAllocators, DefaultAllocatorTypes, FixedSizeNames);

    TYPED_TEST(TestFixedSizeAllocators, SingleAllocation)
    {

        EXPECT_THROW(this->pAlloc->Acquire(DERIVED_A_SIZE - 1), std::length_error);
        EXPECT_THROW(this->pAlloc->Acquire(DERIVED_A_SIZE + 1), std::length_error);

        void* p{};
        EXPECT_NO_THROW(p = this->pAlloc->Acquire(DERIVED_A_SIZE));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, DERIVED_A_SIZE);

        EXPECT_NO_THROW(this->pAlloc->Release(p));
    }

    TYPED_TEST(TestFixedSizeAllocators, InvalidRelease)
    {
        void* p{nullptr};
        EXPECT_NO_THROW(this->pAlloc->Release(p));
        p = new char{'e'};

        EXPECT_NO_THROW(this->pAlloc->Release(p));
        EXPECT_EQ((*static_cast<char*>(p)), 'e');

        delete static_cast<char*>(p);
    }

    TYPED_TEST(TestFixedSizeAllocators, TwoAllocations)
    {
        void* p1{};
        EXPECT_NO_THROW((p1 = this->pAlloc->Acquire(DERIVED_A_SIZE)));
        EXPECT_NE(p1, nullptr);
        std::memset(p1, 1, DERIVED_A_SIZE);

        void* p2{};
        EXPECT_NO_THROW((p2 = this->pAlloc->Acquire(DERIVED_A_SIZE)));
        EXPECT_NE(p2, nullptr);
        std::memset(p2, 1, DERIVED_A_SIZE);


        EXPECT_NO_THROW(this->pAlloc->Release(p1));
        EXPECT_NO_THROW(this->pAlloc->Release(p2));
    }

    TYPED_TEST(TestFixedSizeAllocators, FillAllocator)
    {
        constexpr std::size_t bufferSize{
            std::is_same_v<TypeParam, ComponentAllocator>
            ? Component::GetMaxAmount<DerivedCompA>()
            : BUFFER_SIZE
        };

        void* pointers[bufferSize]{};

        for (size_t i = 0; i < bufferSize ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = this->pAlloc->Acquire(DERIVED_A_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), DERIVED_A_SIZE);
        }
        for (const auto & pointer : pointers)
            EXPECT_NO_THROW(this->pAlloc->Release(pointer));
    }

    TYPED_TEST(TestFixedSizeAllocators, Overflow)
    {
        constexpr std::size_t bufferSize{
            (std::is_same_v<TypeParam, ComponentAllocator>
                ? Component::GetMaxAmount<DerivedCompA>()
                : BUFFER_SIZE)
            + AMOUNT_OF_OVERFLOW_ALLOCATIONS
        };

        void* pointers[bufferSize]{};
        for (size_t i = 0; i < bufferSize ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = this->pAlloc->Acquire(DERIVED_A_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), DERIVED_A_SIZE);
        }

        EXPECT_TRUE(this->pAlloc->IsOverflown());
        EXPECT_EQ(this->pAlloc->AmountOfOverflowAllocations(), AMOUNT_OF_OVERFLOW_ALLOCATIONS);

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(this->pAlloc->Release(pointer));

        EXPECT_FALSE(this->pAlloc->IsOverflown());
    }

    TYPED_TEST(TestFixedSizeAllocators, ReleaseInMiddle)
    {
        constexpr std::size_t bufferSize{
            std::is_same_v<TypeParam, ComponentAllocator>
            ? Component::GetMaxAmount<DerivedCompA>()
            : BUFFER_SIZE
        };
        void* pointers[bufferSize]{};
        for (size_t i = 0; i < bufferSize; i++)
        {
            EXPECT_NO_THROW(pointers[i] = this->pAlloc->Acquire(DERIVED_A_SIZE));
            EXPECT_NE(pointers[i], nullptr);
            std::memset(pointers[i], static_cast<int>( i ), DERIVED_A_SIZE);
        }
        EXPECT_FALSE(this->pAlloc->IsOverflown());

        constexpr std::size_t middleIndex{bufferSize/2 };
        EXPECT_NO_THROW(this->pAlloc->Release(pointers[middleIndex]));

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(this->pAlloc->Release(pointer));

    }

    TYPED_TEST(TestFixedSizeAllocators, NewAndDeleteOperators)
    {
        auto& pAlloc = *this->pAlloc;
        EXPECT_THROW(operator new (DERIVED_A_SIZE - 1, pAlloc), std::length_error);
        EXPECT_THROW(operator new (DERIVED_A_SIZE + 1, pAlloc), std::length_error);

        void* p{};

        EXPECT_NO_THROW(p = operator new (DERIVED_A_SIZE, pAlloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, DERIVED_A_SIZE);

        EXPECT_NO_THROW(operator delete(p, pAlloc));


        EXPECT_THROW(operator new[](DERIVED_A_SIZE - 1, pAlloc), std::length_error);
        EXPECT_THROW(operator new[](DERIVED_A_SIZE + 1, pAlloc), std::length_error);


        EXPECT_NO_THROW(p = operator new[](DERIVED_A_SIZE, pAlloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, DERIVED_A_SIZE);

        EXPECT_NO_THROW(operator delete[](p, pAlloc));
    }

    TYPED_TEST(TestFixedSizeAllocators, NewAndDelete)
    {
        auto& pAlloc = *this->pAlloc;
        DerivedCompA* pDC {nullptr};
        EXPECT_NO_THROW(pDC = new (pAlloc) DerivedCompA{});
        EXPECT_NE(pDC, nullptr);
        EXPECT_EQ(pDC->y, DerivedCompA::MAX_AMOUNT);

        if (pDC) pDC->x = 1234;

        EXPECT_NO_THROW(operator delete (pDC, pAlloc));

        pDC = nullptr;

        EXPECT_THROW(pDC = new (pAlloc) DerivedCompA[2]{}, std::length_error);
        EXPECT_EQ(pDC, nullptr);

        EXPECT_NO_THROW(operator delete (pDC, pAlloc));
    }

    enum TestableSizes
    {
        Default = BUFFER_SIZE,
        Small = 1,
        Large = 1'000'000
    };

    template <typename AllocT, TestableSizes SizeVal, bool SmallestBlockSizeVal>
        requires std::is_base_of_v<FixedSizeAllocator, AllocT>
    class AllocType
    {
    public:
        using Alloc = AllocT;
        static constexpr TestableSizes Size = SizeVal;
        static constexpr bool SmallestBlockSize = SmallestBlockSizeVal;
    };

    template <typename T>
    class TestBufferSizes : public testing::Test
    {
    public:
        void SetUp() override
        {
            using Alloc = typename T::Alloc;
            constexpr auto Size = T::Size;
            constexpr bool SmallestBlockSize = T::SmallestBlockSize;

            static_assert(std::is_base_of_v<FixedSizeAllocator, Alloc>);

            if constexpr (std::is_same_v<Alloc, FixedSizeAllocator>)
            {
                if constexpr (SmallestBlockSize) pAlloc = std::make_unique<FixedSizeAllocator>(std::type_identity<bool>{},Size);
                else pAlloc = std::make_unique<FixedSizeAllocator>(std::type_identity<DerivedCompA>{},Size);
            }

            else if constexpr (std::is_same_v<Alloc, ComponentAllocator>)
            {
                switch (Size)
                {
                case Default:
                    if (SmallestBlockSize) pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<DerivedCompB>{});
                    else pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<DerivedCompA>{});
                    break;
                case Small:
                    if (SmallestBlockSize) pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<SmallAmountB>{});
                    else pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<SmallAmountA>{});
                    break;
                case Large:
                    if (SmallestBlockSize) pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<LargeAmountB>{});
                    else pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<LargeAmountA>{});
                    break;
                }
            }

            else if constexpr (std::is_same_v<Alloc, DefaultTypeAllocator>)
            {
                if (SmallestBlockSize) pAlloc = std::make_unique<TypeAllocator<bool, Size>>();
                else pAlloc = std::make_unique<TypeAllocator<DerivedCompA, Size>>();
            }
        }

    protected:
        std::unique_ptr<FixedSizeAllocator> pAlloc{nullptr};
    };

    using BufferSizeAllocatorTypes = testing::Types<
        AllocType<FixedSizeAllocator, Default, false>,
        AllocType<FixedSizeAllocator, Default, true>,
        AllocType<FixedSizeAllocator, Small, false>,
        AllocType<FixedSizeAllocator, Small, true>,
        AllocType<FixedSizeAllocator, Large, false>,
        AllocType<FixedSizeAllocator, Large, true>,

        AllocType<ComponentAllocator, Default, false>,
        AllocType<ComponentAllocator, Default, true>,
        AllocType<ComponentAllocator, Small, false>,
        AllocType<ComponentAllocator, Small, true>,
        AllocType<ComponentAllocator, Large, false>,
        AllocType<ComponentAllocator, Large, true>,

        AllocType<DefaultTypeAllocator, Default, false>,
        AllocType<DefaultTypeAllocator, Default, true>,
        AllocType<DefaultTypeAllocator, Small, false>,
        AllocType<DefaultTypeAllocator, Small, true>,
        AllocType<DefaultTypeAllocator, Large, false>,
        AllocType<DefaultTypeAllocator, Large, true>
    >;

    class BufferSizeTestNames {
    public:
        template <typename T>
        static std::string GetName(int)
        {
            using Alloc = typename T::Alloc;
            constexpr auto Size = T::Size;
            constexpr bool SmallestBlockSize = T::SmallestBlockSize;
            std::string name{};

            if constexpr (SmallestBlockSize) name += "TinyBlock";
            else name += "DefaultBlock";

            name += "_X_";

            if constexpr (Size == Default) name += "DefaultSize";
            else if constexpr (Size == Small) name += "SmallSize";
            else if constexpr (Size == Large) name += "LargeSize";

            name += "_X_";

            if constexpr (std::is_same_v<Alloc, FixedSizeAllocator>) name += "FixedSizeAllocator";
            else if constexpr (std::is_same_v<Alloc, ComponentAllocator>) name += "ComponentAllocator";
            else if constexpr (std::is_same_v<Alloc, DefaultTypeAllocator>) name += "TypeAllocator";

            return name;
        }
    };

    TYPED_TEST_SUITE(TestBufferSizes, BufferSizeAllocatorTypes, BufferSizeTestNames);
    TYPED_TEST(TestBufferSizes, CompleteBufferSize)
    {
        const auto& pAlloc = *this->pAlloc;
        const auto blockSize = pAlloc.GetBlockSize();
        const auto bufferSize = pAlloc.GetCapacity();
        const auto completeSize = pAlloc.CompleteBufferSize();
        const auto objectBufferSize = bufferSize * blockSize;
        EXPECT_GT(completeSize, objectBufferSize);
        EXPECT_GE(completeSize, objectBufferSize + bufferSize * sizeof(bool));
        EXPECT_LE(completeSize, objectBufferSize * 2);
    }
}
