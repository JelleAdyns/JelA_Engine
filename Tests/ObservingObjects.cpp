#include <gtest/gtest.h>

#include "ObservingObjects.h"

namespace jela
{
    struct MyTexture
    {
        MyTexture(const tstring& name):
            NAME{name}
        {}
        tstring NAME;
    };
    static const tstring TEST_TEXTURE_ONE_NAME {_T("Wesley.png")};
    static const tstring TEST_TEXTURE_TWO_NAME {_T("Finn.png")};
    static const tstring TEST_TEXTURE_THREE_NAME {_T("Bob.png")};

    TEST(ResourcePtrTests, SingleResourcePtr)
    {
        // init
        ResourcePtr<MyTexture> p{};
        EXPECT_EQ(p.get(), nullptr);
        auto pTexture {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_ONE_NAME)};

        // binding resource
        p = ResourcePtr{*pTexture};
        EXPECT_NE(p.get(), nullptr);
        EXPECT_EQ(p.get(), &pTexture->object);
        EXPECT_EQ(p->NAME, TEST_TEXTURE_ONE_NAME);

        // Deleting resource
        pTexture.reset(nullptr);
        EXPECT_EQ(p.get(), nullptr);
    }
    TEST(ResourcePtrTests, MultipleResourcePtrs)
    {
        // init
        std::vector<ResourcePtr<MyTexture>> resourcePointers(100);

        for (const auto& p : resourcePointers)
            EXPECT_EQ(p.get(), nullptr);

        auto pTexture {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_ONE_NAME)};

        // binding resource
        for (auto& p : resourcePointers)
        {
            p = ResourcePtr{*pTexture};
            EXPECT_NE(p.get(), nullptr);
            EXPECT_EQ(p.get(), &pTexture->object);
            EXPECT_EQ(p->NAME, TEST_TEXTURE_ONE_NAME);
        }
        for (const auto& p : resourcePointers)
        {
            EXPECT_NE(p.get(), nullptr);
            EXPECT_EQ(p.get(), &pTexture->object);
            EXPECT_EQ(p->NAME, TEST_TEXTURE_ONE_NAME);
        }

        // Deleting resource
        pTexture.reset(nullptr);

        for (auto& p : resourcePointers)
            EXPECT_EQ(p.get(), nullptr);
    }
    TEST(ResourcePtrTests, Copy)
    {
        auto pTextureOne {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_ONE_NAME)};
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
        auto pTextureTwo {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_TWO_NAME)};
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
    TEST(ResourcePtrTests, Move)
    {
        auto pTextureOne {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_ONE_NAME)};
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
        auto pTextureTwo {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_TWO_NAME)};
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
    TEST(ResourcePtrTests, Operators)
    {
        auto pTextureOne {std::make_unique<ObjectObserved<MyTexture>>(TEST_TEXTURE_ONE_NAME)};
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
