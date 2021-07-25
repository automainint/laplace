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

    static void create(world w);
    static void loading(entity en);
    static void launch(entity en);
    static void status_changed(entity en);
    static void set_landscape(entity en, sl::index id_landscape);
    static void set_pathmap(entity en, sl::index id_pathmap);

    [[nodiscard]] static auto get_version(entity en) -> sl::index;
    [[nodiscard]] static auto is_loading(entity en) -> bool;
    [[nodiscard]] static auto is_launched(entity en) -> bool;
    [[nodiscard]] static auto get_landscape(entity en) -> sl::index;
    [[nodiscard]] static auto get_pathmap(entity en) -> sl::index;
    [[nodiscard]] static auto get_slots(entity en) -> sl::index;
    [[nodiscard]] static auto get_units(entity en) -> sl::index;

  protected:
    root(proto_tag);

  private:
    static sl::index n_version;
    static sl::index n_is_loading;
    static sl::index n_is_launched;
    static sl::index n_landscape;
    static sl::index n_pathmap;
    static sl::index n_slots;
    static sl::index n_units;

    static root m_proto;
  };

  using ptr_root = std::shared_ptr<root>;
}

#endif
