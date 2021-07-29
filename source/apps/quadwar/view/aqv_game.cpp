/*  apps/quadwar/view/aqv_game.cpp
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

#include "../../../laplace/engine/world.h"
#include "../object/landscape.h"
#include "../object/root.h"
#include "../object/sets.h"
#include "game.h"
#include <numeric>

namespace quadwar_app::view {
  namespace access = engine::access;

  using std::min, std::numeric_limits, engine::vec2z, engine::intval,
      object::sets::scale_real;

  void game::adjust_layout(sl::index width, sl::index height) {
    m_camera.set_frame(
        { static_cast<real>(width), static_cast<real>(height) });
  }

  void game::set_position(const vec2 v) {
    m_camera.set_position(v * m_camera.get_grid_scale());
  }

  void game::set_cursor(const vec2 cursor) {
    const auto s = m_camera.get_scale();
    const auto f = m_camera.get_frame() / 2.f;
    const auto p = m_camera.get_position();

    if (s <= numeric_limits<decltype(s)>::epsilon())
      return;

    m_cursor = p + (cursor - f) / s;
  }

  auto game::get_unit() -> sl::index {
    if (m_selection.empty()) {
      return -1;
    }

    return m_selection[0];
  }

  auto game::get_grid_position() -> vec2z {
    const auto s = m_camera.get_grid_scale();

    if (s <= numeric_limits<decltype(s)>::epsilon())
      return {};

    const auto p = (m_cursor / s) * static_cast<real>(scale_real);

    const auto x = static_cast<intval>(round(p.x()));
    const auto y = static_cast<intval>(round(p.y()));

    return { x, y };
  }

  void game::move(const vec2 delta) {
    const auto s = get_scale();

    if (s > numeric_limits<decltype(s)>::epsilon())
      m_camera.set_position(m_camera.get_position() + delta / s);
  }

  void game::scale(const real delta) {
    m_scale_ln += delta;
    m_camera.set_scale(get_scale());
  }

  void game::click() {
    const auto units = m_units.get_units();

    for (sl::index i = 0; i < units.size(); i++) {
      if (units[i].rect[0].x() <= m_cursor.x() &&
          units[i].rect[1].x() > m_cursor.x() &&
          units[i].rect[0].y() <= m_cursor.y() &&
          units[i].rect[1].y() > m_cursor.y()) {

        m_selection.resize(1);
        m_selection[0] = units[i].id;
        return;
      }
    }

    m_selection.clear();
  }

  void game::render(engine::access::world w) {
    update_bounds(w);
    update_highlight();

    m_landscape.render(m_camera, w);
    m_units.render(m_camera, w, m_highlight, m_selection);
  }

  auto game::get_scale() const -> real {
    return exp(m_scale_ln);
  }

  void game::update_bounds(world w) {
    auto r = w.get_entity(w.get_root());

    const auto land = w.get_entity(object::root::get_landscape(r));

    const auto width = static_cast<real>(
        object::landscape::get_width(land));

    const auto height = static_cast<real>(
        object::landscape::get_height(land));

    const auto s = get_scale();

    if (s <= numeric_limits<decltype(s)>::epsilon())
      return;

    const auto fx = m_camera.get_frame().x() / s;
    const auto fy = m_camera.get_frame().y() / s;

    const auto dx = m_camera.get_grid_scale() * width;
    const auto dy = m_camera.get_grid_scale() * height;

    const auto hx = min(dx / 2.f, fx / 2.f);
    const auto hy = min(dy / 2.f, fy / 2.f);

    const auto x0 = hx;
    const auto y0 = hy;
    const auto x1 = dx - hx;
    const auto y1 = dy - hy;

    m_camera.set_bounds({ x0, y0 }, { x1, y1 });
  }

  void game::update_highlight() {
    const auto units = m_units.get_units();

    for (sl::index i = 0; i < units.size(); i++) {
      if (units[i].rect[0].x() <= m_cursor.x() &&
          units[i].rect[1].x() > m_cursor.x() &&
          units[i].rect[0].y() <= m_cursor.y() &&
          units[i].rect[1].y() > m_cursor.y()) {

        m_highlight.resize(1);
        m_highlight[0] = units[i].id;
        return;
      }
    }

    m_highlight.clear();
  }
}
