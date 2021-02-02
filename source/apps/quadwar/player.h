#pragma once

#include <laplace/engine/access/entity.h>
#include <laplace/engine/object/actor.h>

namespace quadwar_app {
  using namespace laplace;

  class player : public engine::object::actor {
  public:
    player(bool is_local = false);
    ~player() override = default;

    static void set_name(
        engine::access::entity en, std::u8string_view name);
    static auto get_name(engine::access::entity en)
        -> std::u8string;

  protected:
    auto do_request(size_t id, cref_vbyte args) const
        -> vbyte override;
    void do_modify(size_t id, cref_vbyte args) override;

  private:
    player(proto_tag);

    static player m_proto;

    std::u8string m_name;
  };
}
