#include <gtest/gtest.h>

#include "ObservingObjects.h"

namespace jela
{
    struct MyTexture
    {
        MyTexture(const tstring& name): NAME{name} {}
        tstring NAME;
    };

    class TestResourcePointers : public ::testing::Test
    {
    protected:
        static inline const tstring TEST_TEXTURE_ONE_NAME {_T("Wesley.png")};
        static inline const tstring TEST_TEXTURE_TWO_NAME {_T("Finn.png")};
        std::unique_ptr<ObjectObserved<MyTexture>> pTextureOne {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_ONE_NAME)};
        std::unique_ptr<ObjectObserved<MyTexture>> pTextureTwo {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_TWO_NAME)};
    };

    TEST_F(TestResourcePointers, SingleResourcePtr)
    {
        // init
        ResourcePtr<MyTexture> p{};
        EXPECT_EQ(p.get(), nullptr);

        // binding resource
        p = ResourcePtr{*pTextureOne};
        EXPECT_NE(p.get(), nullptr);
        EXPECT_EQ(p.get(), &pTextureOne->object);
        EXPECT_EQ(p->NAME, TEST_TEXTURE_ONE_NAME);

        // Deleting resource
        pTextureOne.reset(nullptr);
        EXPECT_EQ(p.get(), nullptr);
    }
    TEST_F(TestResourcePointers, MultipleResourcePtrs)
    {
        // init
        std::vector<ResourcePtr<MyTexture>> resourcePointers(100);

        for (const auto& p : resourcePointers)
            EXPECT_EQ(p.get(), nullptr);

        // binding resource
        for (auto& p : resourcePointers)
        {
            p = ResourcePtr{*pTextureOne};
            EXPECT_NE(p.get(), nullptr);
            EXPECT_EQ(p.get(), &pTextureOne->object);
            EXPECT_EQ(p->NAME, TEST_TEXTURE_ONE_NAME);
        }
        for (const auto& p : resourcePointers)
        {
            EXPECT_NE(p.get(), nullptr);
            EXPECT_EQ(p.get(), &pTextureOne->object);
            EXPECT_EQ(p->NAME, TEST_TEXTURE_ONE_NAME);
        }

        // Deleting resource
        pTextureOne.reset(nullptr);

        for (auto& p : resourcePointers)
            EXPECT_EQ(p.get(), nullptr);
    }
    TEST_F(TestResourcePointers, Copy)
    {
        ResourcePtr p1{*pTextureOne};
        EXPECT_NE(p1.get(), nullptr);
        EXPECT_EQ(p1.get(), &pTextureOne->object);
        EXPECT_EQ(p1->NAME, TEST_TEXTURE_ONE_NAME);

        //Copy Constructor
        ResourcePtr p2{p1};
        EXPECT_NE(p2.get(), nullptr);
        EXPECT_EQ(p2.get(), &pTextureOne->object);
        EXPECT_EQ(p2->NAME, TEST_TEXTURE_ONE_NAME);

        //Copy operator
        ResourcePtr p3{*pTextureTwo};
        EXPECT_NE(p3.get(), nullptr);
        EXPECT_EQ(p3.get(), &pTextureTwo->object);
        EXPECT_EQ(p3->NAME, TEST_TEXTURE_TWO_NAME);
        p3 = p1;
        EXPECT_NE(p3.get(), nullptr);
        EXPECT_EQ(p3.get(), &pTextureOne->object);
        EXPECT_EQ(p3->NAME, TEST_TEXTURE_ONE_NAME);

        // Deleting resource
        pTextureOne.reset(nullptr);
        pTextureTwo.reset(nullptr);
        EXPECT_EQ(p1.get(), nullptr);
        EXPECT_EQ(p2.get(), nullptr);
        EXPECT_EQ(p3.get(), nullptr);
    }
    TEST_F(TestResourcePointers, Move)
    {
        ResourcePtr p1{*pTextureOne};
        EXPECT_NE(p1.get(), nullptr);
        EXPECT_EQ(p1.get(), &pTextureOne->object);
        EXPECT_EQ(p1->NAME, TEST_TEXTURE_ONE_NAME);

        //Move Constructor
        ResourcePtr p2{std::move(p1)};
        EXPECT_EQ(p1.get(), nullptr);
        EXPECT_NE(p2.get(), nullptr);
        EXPECT_EQ(p2.get(), &pTextureOne->object);
        EXPECT_EQ(p2->NAME, TEST_TEXTURE_ONE_NAME);

        //Move operator
        ResourcePtr p3{*pTextureTwo};
        EXPECT_NE(p3.get(), nullptr);
        EXPECT_EQ(p3.get(), &pTextureTwo->object);
        EXPECT_EQ(p3->NAME, TEST_TEXTURE_TWO_NAME);
        p3 = std::move(p2);
        EXPECT_EQ(p2.get(), nullptr);
        EXPECT_NE(p3.get(), nullptr);
        EXPECT_EQ(p3.get(), &pTextureOne->object);
        EXPECT_EQ(p3->NAME, TEST_TEXTURE_ONE_NAME);

        // Deleting resource
        pTextureOne.reset(nullptr);
        pTextureTwo.reset(nullptr);
        EXPECT_EQ(p1.get(), nullptr);
        EXPECT_EQ(p2.get(), nullptr);
        EXPECT_EQ(p3.get(), nullptr);
    }
    TEST_F(TestResourcePointers, Operators)
    {
        ResourcePtr p1{*pTextureOne};
        EXPECT_NO_THROW(p1.get());
        EXPECT_NE(p1.get(), nullptr);
        EXPECT_EQ(p1.get(), &pTextureOne->object);

        // Unary Asterisk operator
        EXPECT_NO_THROW(*p1);
        EXPECT_EQ((*p1).NAME, TEST_TEXTURE_ONE_NAME);
        // Arrow operator
        EXPECT_NO_THROW(p1->NAME);
        EXPECT_EQ(p1->NAME, TEST_TEXTURE_ONE_NAME);
    }

}
