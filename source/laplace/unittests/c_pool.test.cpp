#include "../core/pool.h"
#include <gtest/gtest.h>

using namespace laplace;
using namespace core;
using namespace std;

TEST(laplace_core, pool_contains)
{
    pool p;

    wstring a = L"my file";
    wstring b = L"file2";

    EXPECT_FALSE(p.contains(a));
    EXPECT_FALSE(p.contains(b));
}

TEST(laplace_core, pool_store)
{
    pool p;

    wstring name = L"my file";

    EXPECT_FALSE(p.contains(name));

    p.store(name, make_shared<element>());

    EXPECT_TRUE(p.contains(name));
}

TEST(laplace_core, pool_release)
{
    pool p;

    wstring name = L"file2";

    EXPECT_FALSE(p.contains(name));

    p.store(name, make_shared<element>());

    EXPECT_TRUE(p.contains(name));
}

TEST(laplace_core, pool_clear)
{
    pool p;

    wstring a = L"file1";
    wstring b = L"file2";

    EXPECT_FALSE(p.contains(a));
    EXPECT_FALSE(p.contains(b));

    p.store(a, make_shared<element>());
    p.store(b, make_shared<element>());

    EXPECT_TRUE(p.contains(a));
    EXPECT_TRUE(p.contains(b));

    p.clear();

    EXPECT_FALSE(p.contains(a));
    EXPECT_FALSE(p.contains(b));
}

TEST(laplace_core, pool_load)
{
    pool p;

    wstring a = L"file1";
    wstring b = L"file2";

    EXPECT_TRUE(!p.load(a));
    EXPECT_TRUE(!p.load(b));

    auto obj1 = make_shared<element>();
    auto obj2 = make_shared<element>();

    p.store(a, obj1);
    p.store(b, obj2);

    EXPECT_EQ(p.load(a), obj1);
    EXPECT_EQ(p.load(b), obj2);
}
