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
    root()           = default;
    ~root() override = default;

    auto changed() -> bool;

    auto get_slot_count() -> size_t;
    auto get_slot(size_t index) -> size_t;

    static void slot_create(engine::access::entity en,
                            size_t                 id_actor);
    static void slot_remove(engine::access::entity en,
                            size_t                 id_actor);
    static void status_changed(engine::access::entity en);

    static auto get_slot_count(engine::access::entity en)
        -> size_t;
    static auto get_slot(engine::access::entity en,
                         size_t index) -> size_t;

  protected:
    auto do_request(size_t id, cref_vbyte args) const
        -> vbyte override;
    void do_modify(size_t id, cref_vbyte args) override;

  private:
    bool  m_is_changed = false;
    vuint m_slots;
  };

  using ptr_root = std::shared_ptr<root>;
}

#endif
