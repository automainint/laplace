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

    static void slot_create(entity en, sl::index id_actor);
    static void slot_remove(entity en, sl::index id_actor);
    static void unit_create(entity en, sl::index id_unit);
    static void unit_remove(entity en, sl::index id_unit);
    static void loading(entity en);
    static void launch(entity en);
    static void status_changed(entity en);
    static void set_landscape(entity en, sl::index id_landscape);
    static void set_pathmap(entity en, sl::index id_pathmap);

    [[nodiscard]] static auto get_version(entity en) -> sl::index;
    [[nodiscard]] static auto is_loading(entity en) -> bool;
    [[nodiscard]] static auto is_launched(entity en) -> bool;
    [[nodiscard]] static auto get_slot_count(entity en) -> sl::whole;
    [[nodiscard]] static auto get_slot(entity en, sl::index index)
        -> sl::index;
    [[nodiscard]] static auto get_unit_count(entity en) -> sl::whole;
    [[nodiscard]] static auto get_unit(entity en, sl::index index)
        -> sl::index;
    [[nodiscard]] static auto get_all_units(entity en) -> sl::vector<sl::index>;
    [[nodiscard]] static auto get_landscape(entity en) -> sl::index;
    [[nodiscard]] static auto get_pathmap(entity en) -> sl::index;

  protected:
    root(proto_tag);

    auto do_request(sl::index id, span_cbyte args) const -> vbyte override;
    void do_modify(sl::index id, span_cbyte args) override;

  private:
    void do_slot_create(const sl::index id_actor);
    void do_slot_remove(const sl::index id_actor);
    void do_unit_create(const sl::index id_unit);
    void do_unit_remove(const sl::index id_unit);
    void do_launch();

    static sl::index n_version;
    static sl::index n_is_loading;
    static sl::index n_is_launched;
    static sl::index n_slot_count;
    static sl::index n_unit_count;
    static sl::index n_landscape;
    static sl::index n_pathmap;

    static root m_proto;

    sl::vector<sl::index> m_slots;
    sl::vector<sl::index> m_units;
  };

  using ptr_root = std::shared_ptr<root>;
}

#endif
