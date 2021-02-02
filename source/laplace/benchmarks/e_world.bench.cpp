#include "../engine/access/world.h"
#include "../engine/world.h"
#include <benchmark/benchmark.h>

namespace laplace::engine::world_test {
  class my_entity : public basic_entity {
  public:
    my_entity() :
        basic_entity(true, false, false, false, 10, box {}) {
      using namespace object;

      setup_sets({ { sets::debug_value, 0, 0 } });

      n_value = index_of(sets::debug_value);
    }

    ~my_entity() override = default;

    void tick(access::world) override {
      apply_delta(n_value, 1);
    }

  private:
    size_t n_value = 0;
  };
}

using namespace laplace;
using namespace engine;
using namespace object;
using namespace world_test;
using namespace std;

static void bm_laplace_engine_world_startup(
    benchmark::State &state) {
  for (auto _ : state) {
    auto a = make_shared<world>();
    a->set_thread_count(32);
  }
}

BENCHMARK(bm_laplace_engine_world_startup);

static void bm_laplace_engine_world_single_thread(
    benchmark::State &state) {
  auto a = make_shared<world>();
  auto e = make_shared<my_entity>();

  a->set_thread_count(0);
  a->spawn(e, id_undefined);

  for (auto _ : state) {
    a->tick(100);

    volatile auto value =
        e->get(e->index_of(sets::debug_value));
  }
}

BENCHMARK(bm_laplace_engine_world_single_thread);

static void bm_laplace_engine_world_multithreading(
    benchmark::State &state) {
  auto a = make_shared<world>();
  auto e = make_shared<my_entity>();

  a->set_thread_count(32);
  a->spawn(e, id_undefined);

  for (auto _ : state) {
    a->tick(100);

    volatile auto value =
        e->get(e->index_of(sets::debug_value));
  }
}

BENCHMARK(bm_laplace_engine_world_multithreading);
