/*  laplace/ui/ui_widget.cpp
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

#include "widget.h"

#include "../core/defs.h"
#include "../graphics/utils.h"
#include "../platform/opengl.h"
#include "context.h"
#include <algorithm>

namespace laplace::ui {
  using sl::vector, graphics::prepare_ui, core::cref_input_handler;

  auto widget::tick(sl::time           delta_msec,
                    cref_input_handler in,
                    bool               is_handled) -> bool {
    return widget_tick(delta_msec, in, is_handled);
  }

  void widget::render() {
    widget_render();

    up_to_date();
  }

  auto widget::event_allowed(sl::index x, sl::index y) -> bool {
    if (!contains(get_absolute_rect(), x, y))
      return false;

    auto p = m_parent.lock();

    if (!p)
      return true;

    return p->event_allowed(x, y);
  }

  void widget::set_layout(layout fn) {
    m_layout = fn;

    adjust_layout();
    set_expired(true);
  }

  void widget::set_level(sl::index level) {
    m_level = level;
    set_expired(true);
  }

  void widget::set_rect(cref_rect r) {
    if (m_rect != r) {
      m_rect = r;

      adjust_layout();
      set_expired(true);
    }
  }

  void widget::set_visible(bool state) {
    if (m_is_visible != state) {
      m_is_visible = state;
      set_expired(true);
    }
  }

  void widget::set_enabled(bool state) {
    if (m_is_enabled != state) {
      m_is_enabled = state;
      set_expired(true);
    }
  }

  void widget::set_focus(bool has_focus) {
    if (m_has_focus != has_focus) {
      if (has_focus) {
        if (auto p = m_parent.lock(); p) {
          if (p->m_focus_index < p->m_childs.size()) {
            p->m_childs[p->m_focus_index]->set_focus(false);
            p->m_focus_index = m_attach_index;
          }

          p->set_focus(true);
        }
      } else if (m_focus_index < m_childs.size()) {
        m_childs[m_focus_index]->set_focus(false);
        m_focus_index = m_childs.size();
      }

      m_has_focus = has_focus;
      set_expired(true);
    }
  }

  void widget::move_to(sl::index x, sl::index y) {
    if (m_rect.x != x || m_rect.y != y) {
      m_rect.x = x;
      m_rect.y = y;

      set_expired(true);
    }
  }

  void widget::draw_childs() {
    auto indices    = sl::vector<sl::index> {};
    auto local_area = rect { 0, 0, m_rect.width, m_rect.height };

    for (auto &c : m_childs) {
      if (c->is_visible()) {
        c->m_absolute_x = m_absolute_x + c->m_rect.x;
        c->m_absolute_y = m_absolute_y + c->m_rect.y;
      }
    }

    for (sl::index i = 0; i < m_childs.size(); i++) {
      auto &w = m_childs[i];

      if (w->is_visible() && intersects(local_area, w->m_rect)) {
        indices.emplace_back(i);
      }
    }

    auto op = [this](sl::index a, sl::index b) -> bool {
      return m_childs[a]->get_level() < m_childs[b]->get_level();
    };

    sort(indices.begin(), indices.end(), op);

    for (auto i : indices) { m_childs[i]->render(); }

    m_expired_childs = false;
  }

  void widget::up_to_date() {
    m_expired    = false;
    m_is_changed = false;
  }

  auto widget::widget_tick(sl::time           delta_msec,
                           cref_input_handler in,
                           bool               is_handled) -> bool {
    if (!m_is_attached) {
      m_absolute_x = m_rect.x;
      m_absolute_y = m_rect.y;
    }

    if (m_is_attached || (m_is_visible && m_is_enabled)) {
      auto list = vector<ptr_widget> {};

      for (auto &c : m_childs) {
        if (c->is_visible() && c->is_enabled()) {
          list.emplace_back(c);
        }
      }

      for (auto &c : list) {
        is_handled |= c->tick(delta_msec, in, is_handled);
      }
    }

    return is_handled;
  }

  void widget::widget_render() {
    if (m_is_attached) {
      draw_childs();
    } else if (m_is_visible) {
      /*  Check if visible only for root widget. Visibility
       *  checking for attached widgets in widget::draw_childs.
       */

      prepare();

      draw_childs();
    }
  }

  auto widget::is_widget_changed() -> bool {
    return m_is_changed;
  }

  void widget::clear() {
    for (sl::index i = 0; i < m_childs.size(); i++) {
      m_childs[i]->m_is_attached  = false;
      m_childs[i]->m_attach_index = 0;
      m_childs[i]->m_parent.reset();
    }

    set_expired(true);
  }

  auto widget::attach(ptr_widget child) -> sl::index {
    sl::index result = m_childs.size();

    if (auto c = child; c) {
      if (auto p = c->m_parent.lock(); p) {
        p->detach(c->m_attach_index);
      }

      c->m_is_attached  = true;
      c->m_attach_index = m_childs.size();
      c->m_parent       = weak_from_this();

      m_childs.emplace_back(child);

      set_expired(true);
    }

    return result;
  }

  void widget::detach(ptr_widget child) {
    if (child->m_is_attached &&
        child->m_parent.lock() == shared_from_this()) {
      if (child->m_attach_index < m_childs.size()) {
        m_childs.erase(        //
            m_childs.begin() + //
            static_cast<ptrdiff_t>(child->m_attach_index));

        update_indices(child->m_attach_index);
      }

      child->m_is_attached  = false;
      child->m_attach_index = 0;
      child->m_parent.reset();
    }

    set_expired(true);
  }

  void widget::detach(sl::index child_index) {
    if (child_index < m_childs.size()) {
      m_childs[child_index]->m_is_attached  = false;
      m_childs[child_index]->m_attach_index = 0;
      m_childs[child_index]->m_parent.reset();

      m_childs.erase(        //
          m_childs.begin() + //
          static_cast<ptrdiff_t>(child_index));

      update_indices(child_index);

      set_expired(true);
    }
  }

  auto widget::get_level() const -> sl::index {
    return m_level;
  }

  auto widget::get_rect() const -> cref_rect {
    return m_rect;
  }

  auto widget::get_absolute_x() const -> sl::index {
    return m_absolute_x;
  }

  auto widget::get_absolute_y() const -> sl::index {
    return m_absolute_y;
  }

  auto widget::get_absolute_rect() const -> rect {
    return { .x      = m_absolute_x,
             .y      = m_absolute_y,
             .width  = m_rect.width,
             .height = m_rect.height };
  }

  void widget::next_tab() {
    sl::index i = 0, j = 1;

    for (; i < m_childs.size(); i++) {
      if (m_childs[i]->has_focus()) {
        m_childs[i]->set_focus(false);
        break;
      }
    }

    if (i == m_childs.size() && i > 0) {
      i = m_childs.size() - 1;
    }

    for (; j < m_childs.size(); j++) {
      auto &c = *m_childs[(i + j) % m_childs.size()];

      if (c.m_is_handler && c.is_visible() && c.is_enabled()) {
        c.set_focus(true);
        break;
      }
    }
  }

  void widget::refresh() {
    m_expired        = true;
    m_expired_childs = true;

    for (auto &w : m_childs) { w->refresh(); }
  }

  void widget::set_expired(bool is_expired) {
    if (is_expired) {
      if (auto p = m_parent.lock(); p) {
        p->refresh_childs();
      }

      refresh();

      m_is_changed = true;
    } else {
      m_expired        = 0;
      m_expired_childs = 0;
    }
  }

  auto widget::is_expired() const -> bool {
    return m_expired;
  }

  auto widget::has_childs_expired() const -> bool {
    return m_expired_childs;
  }

  auto widget::is_visible() const -> bool {
    return m_is_visible;
  }

  auto widget::is_enabled() const -> bool {
    return m_is_enabled;
  }

  auto widget::is_attached() const -> bool {
    return m_is_attached;
  }

  auto widget::has_focus() const -> bool {
    return m_has_focus;
  }

  auto widget::get_child_count() const -> sl::index {
    return m_childs.size();
  }

  auto widget::get_parent() const -> ptr_widget {
    return m_parent.lock();
  }

  auto widget::get_child(sl::index index) const -> ptr_widget {
    ptr_widget result;

    if (index < m_childs.size()) {
      result = m_childs[index];
    }

    return result;
  }

  void widget::prepare() {
    prepare_ui();
  }

  auto widget::get_default_context() -> ptr_context {
    return context::get_default();
  }

  void widget::set_handler(bool is_handler) {
    m_is_handler = is_handler;
  }

  void widget::update_indices(sl::index begin) {
    for (sl::index i = begin; i < m_childs.size(); i++) {
      m_childs[i]->m_attach_index = i;
    }
  }

  void widget::adjust_layout() {
    if (m_layout) {
      auto childs  = sl::vector<sl::index> {};
      auto context = vlayout_context {};

      childs.reserve(m_childs.size());
      context.reserve(m_childs.size());

      for (sl::index i = 0; i < m_childs.size(); i++) {
        auto &c = *m_childs[i];

        if (c.is_visible()) {
          childs.emplace_back(i);

          context.emplace_back(layout_context {
              .level = c.get_level(), .box = c.get_rect() });
        }
      }

      auto rects = m_layout(m_rect, context);

      if (rects.size() == childs.size()) {
        for (sl::index i = 0; i < childs.size(); i++) {
          m_childs[childs[i]]->set_rect(rects[i]);
        }
      } else {
        error_("Invalid layout.", __FUNCTION__);
      }
    }
  }

  void widget::refresh_childs() {
    for (auto p = shared_from_this(); p; p = p->m_parent.lock()) {
      p->m_expired_childs = true;
    }
  }
}
