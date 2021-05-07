/*  apps/quadwar/object/root.h
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

#ifndef quadwar_object_root_h
#define quadwar_object_root_h

#include "../../../laplace/engine/basic_entity.h"
#include "defs.h"

namespace quadwar_app::object {
  class root : public engine::basic_entity, helper {
  public:
    root();
    ~root() override = default;

    static void slot_create(entity en, size_t id_actor);
    static void slot_remove(entity en, size_t id_actor);
    static void unit_create(entity en, size_t id_unit);
    static void unit_remove(entity en, size_t id_unit);
    static void loading(entity en);
    static void launch(entity en);
    static void status_changed(entity en);
    static void set_landscape(entity en, size_t id_landscape);
    static void set_pathmap(entity en, size_t id_pathmap);

    [[nodiscard]] static auto get_version(entity en) -> size_t;
    [[nodiscard]] static auto is_loading(entity en) -> bool;
    [[nodiscard]] static auto is_launched(entity en) -> bool;
    [[nodiscard]] static auto get_slot_count(entity en) -> size_t;
    [[nodiscard]] static auto get_slot(entity en, size_t index)
        -> size_t;
    [[nodiscard]] static auto get_unit_count(entity en) -> size_t;
    [[nodiscard]] static auto get_unit(entity en, size_t index)
        -> size_t;
    [[nodiscard]] static auto get_all_units(entity en) -> vuint;
    [[nodiscard]] static auto get_landscape(entity en) -> size_t;
    [[nodiscard]] static auto get_pathmap(entity en) -> size_t;

  protected:
    root(proto_tag);

    auto do_request(size_t id, span_cbyte args) const -> vbyte override;
    void do_modify(size_t id, span_cbyte args) override;

  private:
    void do_slot_create(const size_t id_actor);
    void do_slot_remove(const size_t id_actor);
    void do_unit_create(const size_t id_unit);
    void do_unit_remove(const size_t id_unit);
    void do_launch();

    static size_t n_version;
    static size_t n_is_loading;
    static size_t n_is_launched;
    static size_t n_slot_count;
    static size_t n_unit_count;
    static size_t n_landscape;
    static size_t n_pathmap;

    static root m_proto;

    vuint m_slots;
    vuint m_units;
  };

  using ptr_root = std::shared_ptr<root>;
}

#endif
