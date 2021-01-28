#include "../core/element.h"
#include <gtest/gtest.h>

using namespace laplace;
using namespace core;
using namespace std;

TEST(laplace_core, element_type_id)
{
    element e;
    EXPECT_EQ(e.get_type_id(), element::type_id());
}

TEST(laplace_core, element_inheritance)
{
    class my_element : public element
    {
    public:
        my_element() { set_type_id(1); }
        ~my_element() = default;
    };

    my_element e;

    EXPECT_EQ(e.get_type_id(), 1);
}
