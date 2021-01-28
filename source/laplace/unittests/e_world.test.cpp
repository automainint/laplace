#include "../engine/world.h"
#include "../engine/access/world.h"
#include <gtest/gtest.h>
#include <thread>
#include <chrono>

namespace laplace::engine::world_test
{
    class my_entity : public entity
    {
    public:
        my_entity();
        ~my_entity() override;

        void tick(access::world) override;

    private:
        size_t n_value = 0;
    };

    my_entity::my_entity() : entity(true, false, false, false, 10, box { })
    {
        using namespace object;

        setup_sets({
            { sets::debug_value, 0, 0 }
        });

        n_value = index_of(sets::debug_value);
    }

    my_entity::~my_entity() { }

    void my_entity::tick(access::world)
    {
        apply_delta(n_value, 1);
    }
}

using namespace laplace;
using namespace engine;
using namespace object;
using namespace world_test;
using namespace std;
using namespace chrono;

TEST(laplace_engine, world_startup_time)
{
    /*  TODO
     *  Make a benchmark.
     */

    auto t0 = steady_clock::now();

    auto a = make_shared<world>();
    auto b = make_shared<world>();

    a->set_thread_count(16);
    b->set_thread_count(16);

    a.reset();
    b.reset();

    auto duration = duration_cast<milliseconds>(steady_clock::now() - t0).count();

    EXPECT_LE(duration, 500);
}

TEST(laplace_engine, world_single_thread)
{
    auto a = make_shared<world>();
    auto e = make_shared<my_entity>();

    a->set_thread_count(0);

    a->spawn(e, id_undefined);
    a->tick(100);

    auto value = e->get(e->index_of(sets::debug_value));

    e.reset();
    a.reset();

    EXPECT_EQ(value, 10);
}

TEST(laplace_engine, world_multithreading)
{
    /*  TODO
     *  Make a benchmark.
     */

    auto a = make_shared<world>();
    auto e = make_shared<my_entity>();

    a->set_thread_count(32);

    a->spawn(e, id_undefined);

    auto t0 = steady_clock::now();

    a->tick(100);

    auto duration = duration_cast<milliseconds>(steady_clock::now() - t0).count();

    auto value = e->get(e->index_of(sets::debug_value));

    e.reset();
    a.reset();

    EXPECT_EQ(value, 10);
    EXPECT_LE(duration, 300);
}
