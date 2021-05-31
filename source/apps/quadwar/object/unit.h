/*  apps/quadwar/object/unit.h
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

#ifndef quadwar_object_unit_h
#define quadwar_object_unit_h

#include "../../../laplace/engine/basic_entity.h"
#include "../view/defs.h"
#include "defs.h"

namespace quadwar_app::object {
  class unit : public engine::basic_entity, helper {
  public:
    static constexpr engine::intval default_health = 100;
    static constexpr engine::intval default_radius = 1;

    unit();
    ~unit() override = default;

    void tick(engine::access::world w) override;

    static auto spawn_start_units(world w, sl::whole unit_count)
        -> std::vector<sl::index>;

    static void set_actor(entity en, sl::index id_actor);
    static void set_x(entity en, engine::intval x);
    static void set_y(entity en, engine::intval y);
    static void set_position(entity en, engine::vec2i p);
    static void set_radius(entity en, engine::intval radius);
    static void set_health(entity en, engine::intval health);

    [[nodiscard]] static auto get_actor(entity en) -> sl::index;
    [[nodiscard]] static auto get_x(entity en) -> engine::intval;
    [[nodiscard]] static auto get_y(entity en) -> engine::intval;
    [[nodiscard]] static auto get_position(entity en) -> engine::vec2i;
    [[nodiscard]] static auto get_radius(entity en) -> engine::intval;
    [[nodiscard]] static auto get_health(entity en) -> engine::intval;

    [[nodiscard]] static auto scale_of_x(entity en) -> engine::intval;
    [[nodiscard]] static auto scale_of_y(entity en) -> engine::intval;
    [[nodiscard]] static auto scale_of_radius(entity en)
        -> engine::intval;

    [[nodiscard]] static auto get_position_scaled(entity en)
        -> view::vec2;

    [[nodiscard]] static auto get_radius_scaled(entity en)
        -> view::real;

  protected:
    unit(proto_tag);

  private:
    static unit m_proto;

    static sl::index n_actor;
    static sl::index n_x;
    static sl::index n_y;
    static sl::index n_radius;
    static sl::index n_health;
  };
}

#endif
