#include <gtest/gtest.h>

#include "Component.h"
#include "FixedSizeAllocators.h"


namespace jela
{

    constexpr static std::size_t BUFFER_SIZE = 20;
    constexpr static std::size_t AMOUNT_OF_OVERFLOW_ALLOCATIONS{ 10 };

    enum class TestableSizes
    {
        Default = BUFFER_SIZE,
        Small = 1,
        Large = 500'000
    };

    class DerivedCompA : Component
    {
    public:

        DerivedCompA() { std::cout << "DerivedComp::DerivedCompA()" << std::endl; y = MAX_AMOUNT; }
        ~DerivedCompA() override { std::cout << "DerivedComp::~DerivedCompA()" << std::endl; }
        static constexpr std::size_t MAX_AMOUNT{ static_cast<std::size_t>(TestableSizes::Default) };

        double x{};
        int y{};
        bool z{};
        Component* p{};
    };

    class SmallAmountA : DerivedCompA
    {
    public: static constexpr std::size_t MAX_AMOUNT{static_cast<std::size_t>(TestableSizes::Small)};
    };

    class LargeAmountA : DerivedCompA
    {
    public: static constexpr std::size_t MAX_AMOUNT{ static_cast<std::size_t>(TestableSizes::Large) };
    };


    class DerivedCompB : Component
    {
    public:
        DerivedCompB() { std::cout << "DerivedComp::DerivedCompB()" << std::endl; }
        ~DerivedCompB() override { std::cout << "DerivedComp::~DerivedCompB()" << std::endl; }
        static constexpr std::size_t MAX_AMOUNT{ static_cast<std::size_t>(TestableSizes::Default) };
    };

    class SmallAmountB : DerivedCompB
    {
    public: static constexpr std::size_t MAX_AMOUNT{ static_cast<std::size_t>(TestableSizes::Small) };
    };

    class LargeAmountB : DerivedCompB
    {
    public: static constexpr std::size_t MAX_AMOUNT{ static_cast<std::size_t>(TestableSizes::Large) };
    };


    using DefaultTypeAllocator = TypeAllocator<DerivedCompA, BUFFER_SIZE>;


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
    class TestFixedSizeAllocators : public testing::Test
    {
    public:
        using Alloc = typename T::Alloc;
        static_assert(std::is_base_of_v<FixedSizeAllocator, Alloc>);

        static constexpr auto Size = T::Size;
        static constexpr bool SmallestBlockSize = T::SmallestBlockSize;

    private:
        static consteval auto TypeUsed()
        {
            if constexpr (std::is_same_v<Alloc, ComponentAllocator>)
            {
                if constexpr (Size == TestableSizes::Default)
                {
                    if constexpr (SmallestBlockSize) return std::type_identity<DerivedCompB>{};
                    else return std::type_identity<DerivedCompA>{};
                }
                else if constexpr (Size == TestableSizes::Small)
                {
                    if constexpr (SmallestBlockSize) return std::type_identity<SmallAmountB>{};
                    else return std::type_identity<SmallAmountA>{};
                }
                else if constexpr (Size == TestableSizes::Large)
                {
                    if constexpr (SmallestBlockSize) return std::type_identity<LargeAmountB>{};
                    else return std::type_identity<LargeAmountA>{};
                }
            }
            else if constexpr (SmallestBlockSize) return std::type_identity<bool>{};
            else return std::type_identity<DerivedCompA>{};
        }
    protected:
        using StoredType = typename decltype(TypeUsed())::type;
        void SetUp() override
        {
            if constexpr (std::is_same_v<Alloc, FixedSizeAllocator>)
                pAlloc = std::make_unique<FixedSizeAllocator>(std::type_identity<StoredType>{}, static_cast<std::size_t>(Size));

            else if constexpr (std::is_same_v<Alloc, ComponentAllocator>)
                pAlloc = std::make_unique<ComponentAllocator>(std::type_identity<StoredType>{});

            else if constexpr (std::is_same_v<Alloc, DefaultTypeAllocator>)
                pAlloc = std::make_unique<TypeAllocator<StoredType, static_cast<std::size_t>(Size)>>();

            if (pAlloc) blockSize = sizeof(StoredType);
        }

        void TearDown() override
        {
            EXPECT_EQ(pAlloc->AmountOfUsedBlocks(), 0);
            EXPECT_EQ(pAlloc->AmountOfFreeBlocks(), pAlloc->GetCapacity());
        }

        std::unique_ptr<FixedSizeAllocator> pAlloc{nullptr};
        std::size_t blockSize{};
    };

    using DefaultFixedSizeAllocatorTypes = testing::Types<
        AllocType<FixedSizeAllocator, TestableSizes::Default, false>,
        AllocType<FixedSizeAllocator, TestableSizes::Default, true>,
        AllocType<FixedSizeAllocator, TestableSizes::Small, false>,
        AllocType<FixedSizeAllocator, TestableSizes::Small, true>,
        AllocType<FixedSizeAllocator, TestableSizes::Large, false>,
        AllocType<FixedSizeAllocator, TestableSizes::Large, true>,

        AllocType<ComponentAllocator, TestableSizes::Default, false>,
        AllocType<ComponentAllocator, TestableSizes::Default, true>,
        AllocType<ComponentAllocator, TestableSizes::Small, false>,
        AllocType<ComponentAllocator, TestableSizes::Small, true>,
        AllocType<ComponentAllocator, TestableSizes::Large, false>,
        AllocType<ComponentAllocator, TestableSizes::Large, true>,

        AllocType<DefaultTypeAllocator, TestableSizes::Default, false>,
        AllocType<DefaultTypeAllocator, TestableSizes::Default, true>,
        AllocType<DefaultTypeAllocator, TestableSizes::Small, false>,
        AllocType<DefaultTypeAllocator, TestableSizes::Small, true>,
        AllocType<DefaultTypeAllocator, TestableSizes::Large, false>,
        AllocType<DefaultTypeAllocator, TestableSizes::Large, true>
    >;

    class FixedSizeTestNames {
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

            if constexpr (Size == TestableSizes::Default) name += "DefaultSize";
            else if constexpr (Size == TestableSizes::Small) name += "SmallSize";
            else if constexpr (Size == TestableSizes::Large) name += "LargeSize";

            name += "_X_";

            if constexpr (std::is_same_v<Alloc, FixedSizeAllocator>) name += "FixedSizeAllocator";
            else if constexpr (std::is_same_v<Alloc, ComponentAllocator>) name += "ComponentAllocator";
            else if constexpr (std::is_same_v<Alloc, DefaultTypeAllocator>) name += "TypeAllocator";

            return name;
        }
    };

    TYPED_TEST_SUITE(TestFixedSizeAllocators, DefaultFixedSizeAllocatorTypes, FixedSizeTestNames);

    TYPED_TEST(TestFixedSizeAllocators, CompleteBufferSize)
    {
        auto& alloc = *this->pAlloc;

        const auto blockSize = alloc.GetBlockSize();
        EXPECT_EQ(blockSize, sizeof(typename TestFixture::StoredType));

        const auto bufferSize = alloc.GetCapacity();
        const auto completeSize = alloc.CompleteBufferSize();
        const auto objectBufferSize = bufferSize * blockSize;
        EXPECT_GT(completeSize, objectBufferSize);
        EXPECT_GE(completeSize, objectBufferSize + bufferSize * sizeof(bool));
        EXPECT_LE(completeSize, objectBufferSize * 2);

        auto freeBlocks = alloc.AmountOfFreeBlocks();
        auto usedBlocks = alloc.AmountOfUsedBlocks();
        EXPECT_EQ(freeBlocks, bufferSize);
        EXPECT_EQ(usedBlocks, 0);
    }

    TYPED_TEST(TestFixedSizeAllocators, SingleAllocation)
    {
        auto& alloc = *this->pAlloc;
        EXPECT_THROW(alloc.Acquire(this->blockSize - 1), std::length_error);
        EXPECT_THROW(alloc.Acquire(this->blockSize + 1), std::length_error);

        void* p{};
        EXPECT_NO_THROW(p = alloc.Acquire(this->blockSize));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, this->blockSize);

        const auto bufferSize = alloc.GetCapacity();
        const auto freeBlocks = alloc.AmountOfFreeBlocks();
        const auto usedBlocks = alloc.AmountOfUsedBlocks();
        EXPECT_EQ(freeBlocks, bufferSize - 1);
        EXPECT_EQ(usedBlocks, 1);

        EXPECT_NO_THROW(alloc.Release(p));
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
        auto& alloc = *this->pAlloc;

        void* p1{};
        EXPECT_NO_THROW((p1 = alloc.Acquire(this->blockSize)));
        EXPECT_NE(p1, nullptr);
        std::memset(p1, 1, this->blockSize);

        void* p2{};
        EXPECT_NO_THROW((p2 = alloc.Acquire(this->blockSize)));
        EXPECT_NE(p2, nullptr);
        std::memset(p2, 1, this->blockSize);

        const auto bufferSize = alloc.GetCapacity();
        const auto freeBlocks = alloc.AmountOfFreeBlocks();
        const auto usedBlocks = alloc.AmountOfUsedBlocks();
        if (bufferSize <= 2)
        {
            EXPECT_EQ(freeBlocks, 0);
            EXPECT_EQ(usedBlocks, bufferSize);
        }
        else
        {
            EXPECT_EQ(freeBlocks, bufferSize - 2);
            EXPECT_EQ(usedBlocks, 2);
        }

        EXPECT_NO_THROW(alloc.Release(p1));
        EXPECT_NO_THROW(alloc.Release(p2));
    }

    TYPED_TEST(TestFixedSizeAllocators, FillAllocator)
    {
        auto& alloc = *this->pAlloc;

        constexpr std::size_t bufferSize{static_cast<std::size_t>(TestFixture::Size)};

        std::vector<void*> pointers(bufferSize);

        for (size_t i = 0; i < bufferSize ; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(this->blockSize));
            EXPECT_NE(pointers[i], nullptr);
        }

        const auto capacity = alloc.GetCapacity();
        const auto freeBlocks = alloc.AmountOfFreeBlocks();
        const auto usedBlocks = alloc.AmountOfUsedBlocks();
        EXPECT_EQ(freeBlocks, 0);
        EXPECT_EQ(usedBlocks, capacity);

        EXPECT_FALSE(alloc.IsOverflown());
        for (const auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));
    }

    TYPED_TEST(TestFixedSizeAllocators, Overflow)
    {
        auto& alloc = *this->pAlloc;

        constexpr std::size_t bufferSize{ static_cast<std::size_t>(TestFixture::Size) + AMOUNT_OF_OVERFLOW_ALLOCATIONS };

        std::vector<void*> pointers(bufferSize);

        for (size_t i = 0; i < bufferSize ; ++i)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(this->blockSize));
            EXPECT_NE(pointers[i], nullptr);
        }

        EXPECT_TRUE(alloc.IsOverflown());
        EXPECT_EQ(alloc.AmountOfOverflowAllocations(), AMOUNT_OF_OVERFLOW_ALLOCATIONS);

        for (size_t i = 1; i <= AMOUNT_OF_OVERFLOW_ALLOCATIONS ; ++i)
        {
            void* p{pointers.back()};
            EXPECT_NO_THROW(alloc.Release(p));
            EXPECT_EQ(alloc.AmountOfOverflowAllocations(), AMOUNT_OF_OVERFLOW_ALLOCATIONS - i);
            pointers.pop_back();
        }

        const auto capacity = alloc.GetCapacity();
        const auto freeBlocks = alloc.AmountOfFreeBlocks();
        const auto usedBlocks = alloc.AmountOfUsedBlocks();
        EXPECT_EQ(freeBlocks, 0);
        EXPECT_EQ(usedBlocks, capacity);

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

        EXPECT_FALSE(alloc.IsOverflown());
    }

    TYPED_TEST(TestFixedSizeAllocators, ReleaseInMiddle)
    {
        auto& alloc = *this->pAlloc;

        constexpr std::size_t bufferSize{static_cast<std::size_t>(TestFixture::Size)};

        std::vector<void*> pointers(bufferSize);

        for (size_t i = 0; i < bufferSize; i++)
        {
            EXPECT_NO_THROW(pointers[i] = alloc.Acquire(this->blockSize));
            EXPECT_NE(pointers[i], nullptr);
        }
        EXPECT_FALSE(alloc.IsOverflown());

        constexpr std::size_t middleIndex{bufferSize/2 };
        EXPECT_NO_THROW(alloc.Release(pointers[middleIndex]));

        for (auto & pointer : pointers)
            EXPECT_NO_THROW(alloc.Release(pointer));

    }

    TYPED_TEST(TestFixedSizeAllocators, NewAndDeleteOperators)
    {
        auto& alloc = *this->pAlloc;
        EXPECT_THROW(operator new (this->blockSize - 1, alloc), std::length_error);
        EXPECT_THROW(operator new (this->blockSize + 1, alloc), std::length_error);

        void* p{};

        EXPECT_NO_THROW(p = operator new (this->blockSize, alloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, this->blockSize);

        EXPECT_NO_THROW(operator delete(p, alloc));


        EXPECT_THROW(operator new[](this->blockSize - 1, alloc), std::length_error);
        EXPECT_THROW(operator new[](this->blockSize + 1, alloc), std::length_error);


        EXPECT_NO_THROW(p = operator new[](this->blockSize, alloc));
        EXPECT_NE(p, nullptr);

        std::memset(p, 1, this->blockSize);

        EXPECT_NO_THROW(operator delete[](p, alloc));
    }

    TYPED_TEST(TestFixedSizeAllocators, NewAndDelete)
    {
        using StoredType = TestFixture::StoredType;
        auto& alloc = *this->pAlloc;

        StoredType* pObject {nullptr};
        EXPECT_NO_THROW(pObject = new (alloc) StoredType{});
        EXPECT_NE(pObject, nullptr);

        if constexpr (std::is_same_v<StoredType, DerivedCompA>)
        {
            EXPECT_EQ(pObject->y, StoredType::MAX_AMOUNT);
            if (pObject) EXPECT_NO_THROW(pObject->x = 1234);
        }

        EXPECT_NO_THROW(operator delete (pObject, alloc));

        pObject = nullptr;

        EXPECT_THROW(pObject = new (alloc) StoredType[2]{}, std::length_error);
        EXPECT_EQ(pObject, nullptr);

        EXPECT_NO_THROW(operator delete (pObject, alloc));
    }


}
