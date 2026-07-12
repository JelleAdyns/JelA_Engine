#ifdef _DEBUG
#if __has_include(<vld.h>)
#include <vld.h>
#endif
#endif // _DEBUG

#include <gtest/gtest.h>

#include "Defines.h"

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
