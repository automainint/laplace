/*  apps/quadwar/root.h
 *
 *  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef __quadwar__root__
#define __quadwar__root__

#include "../../laplace/engine/access/entity.h"
#include "../../laplace/engine/basic_entity.h"

namespace quadwar_app {
  using namespace laplace;

  class root : public engine::basic_entity {
  public:
    root();
    ~root() override = default;

    static void slot_create(       //
        engine::access::entity en, //
        size_t                 id_actor);

    static void slot_remove(       //
        engine::access::entity en, //
        size_t                 id_actor);

    static void launch( //
        engine::access::entity en);

    static void status_changed( //
        engine::access::entity en);

    static auto changed( //
        engine::access::entity en) -> bool;

    static auto is_launched( //
        engine::access::entity en) -> bool;

    static auto get_slot_count( //
        engine::access::entity en) -> size_t;

    static auto get_slot(          //
        engine::access::entity en, //
        size_t                 index) -> size_t;

  protected:
    root(proto_tag);

    auto do_request(   //
        size_t     id, //
        cref_vbyte args) const -> vbyte override;

    void do_modify(    //
        size_t     id, //
        cref_vbyte args) override;

    void do_slot_create(const size_t id_actor);
    void do_slot_remove(const size_t id_actor);
    void do_launch();

  private:
    static size_t n_is_changed;
    static size_t n_is_launched;
    static size_t n_slot_count;

    static root m_proto;

    vuint m_slots;
  };

  using ptr_root = std::shared_ptr<root>;
}

#endif
