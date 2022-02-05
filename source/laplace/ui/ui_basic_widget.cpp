/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "basic_widget.h"

#include <algorithm>
#include <utility>

namespace laplace::ui {
  using std::weak_ptr, std::lower_bound, std::u8string_view,
      core::input_handler, core::input_event, core::input_event;

  weak_ptr<context> basic_widget::m_default_context;

  auto basic_widget::process_event(input_event const &ev) noexcept
      -> event_status {
    return widget_process_event(ev);
  }

  void basic_widget::tick(sl::time             delta_msec,
                          input_handler const &in) noexcept {
    widget_tick(delta_msec, in);
  }

  void basic_widget::render(context const &con) noexcept {
    widget_render(con);

    up_to_date();
  }

  auto basic_widget::event_allowed(sl::index x, sl::index y) noexcept
      -> bool {
    if (!contains(get_absolute_rect(), x, y))
      return false;

    auto p = m_parent.lock();

    if (!p)
      return true;

    return p->event_allowed(x, y);
  }

  void basic_widget::process_events_and_tick(
      sl::time delta_msec, input_handler const &in) noexcept {
    for (auto const &ev : in.get_events()) process_event(ev);
    tick(delta_msec, in);
  }

  void basic_widget::set_layout(layout fn) noexcept {
    m_layout = std::move(fn);

    adjust_layout();
    set_expired(true);
  }

  void basic_widget::set_level(sl::index level) noexcept {
    m_level = level;
    set_expired(true);
  }

  void basic_widget::set_rect(cref_rect r) noexcept {
    if (m_rect != r) {
      m_rect = r;

      adjust_layout();
      set_expired(true);
    }
  }

  void basic_widget::set_visible(bool state) noexcept {
    if (m_is_visible != state) {
      m_is_visible = state;
      set_expired(true);
    }
  }

  void basic_widget::set_enabled(bool state) noexcept {
    if (m_is_enabled != state) {
      m_is_enabled = state;
      set_expired(true);
    }
  }

  void basic_widget::set_focus(bool has_focus) noexcept {
    if (m_has_focus == has_focus)
      return;

    if (has_focus) {
      if (auto p = m_parent.lock(); p) {
        if (p->m_focus_index >= 0 &&
            p->m_focus_index < p->m_childs.size()) {
          p->m_childs[p->m_focus_index]->set_focus(false);
          p->m_focus_index = m_attach_index;
        }

        p->set_focus(true);
      }
    } else if (m_focus_index >= 0 &&
               m_focus_index < m_childs.size()) {
      m_childs[m_focus_index]->set_focus(false);
      m_focus_index = -1;
    }

    m_has_focus = has_focus;
    set_expired(true);
  }

  void basic_widget::update_root() noexcept {
    m_absolute_x = m_rect.x;
    m_absolute_y = m_rect.y;
    update_attached();
  }

  void basic_widget::update_attached() noexcept {
    for (auto &c : m_childs) {
      if (!c->is_visible())
        continue;

      c->m_absolute_x = m_absolute_x + c->m_rect.x;
      c->m_absolute_y = m_absolute_y + c->m_rect.y;
      c->update_attached();
    }
  }

  void basic_widget::draw_attached_widgets(
      context const &con) noexcept {
    auto const local_area = rect { 0, 0, m_rect.width,
                                   m_rect.height };

    auto indices = sl::vector<sl::index> {};
    indices.reserve(m_childs.size());

    auto op = [this](sl::index a, sl::index b) -> bool {
      return m_childs[a]->get_level() < m_childs[b]->get_level();
    };

    for (sl::index i = 0; i < m_childs.size(); i++) {
      auto const &w = m_childs[i];

      if (w->is_visible() && intersects(local_area, w->m_rect))
        indices.emplace(
            lower_bound(indices.begin(), indices.end(), i, op), i);
    }

    for (auto i : indices) { m_childs[i]->render(con); }

    m_expired_childs = false;
  }

  void basic_widget::up_to_date() noexcept {
    m_expired    = false;
    m_is_changed = false;
  }

  auto basic_widget::widget_process_event(
      input_event const &ev) noexcept -> event_status {
    if (is_attached() || (is_visible() && is_enabled())) {
      auto list = sl::vector<basic_widget *> {};
      list.reserve(m_childs.size());

      for (auto &c : m_childs)
        if (c->is_visible() && c->is_enabled())
          list.emplace_back(c.get());

      for (auto &c : list)
        if (c->process_event(ev) == event_status::remove)
          return event_status::remove;
    }

    return event_status::proceed;
  }

  void basic_widget::widget_tick(sl::time             delta_msec,
                                 input_handler const &in) noexcept {
    if (!is_attached()) {
      m_absolute_x = m_rect.x;
      m_absolute_y = m_rect.y;
    }

    if (is_attached() || (is_visible() && is_enabled())) {
      auto list = sl::vector<basic_widget *> {};
      list.reserve(m_childs.size());

      for (auto &c : m_childs)
        if (c->is_visible() && c->is_enabled())
          list.emplace_back(c.get());

      for (auto &c : list) c->tick(delta_msec, in);
    }
  }

  void basic_widget::widget_render(context const &con) noexcept {
    if (is_attached()) {
      draw_attached_widgets(con);
    } else
      /*  Check if visible only for root widget. Visibility
       *  checking for attached widgets in widget::draw_childs.
       */
      if (is_visible()) {
        con.prepare();
        update_root();
        draw_attached_widgets(con);
      }
  }

  auto basic_widget::is_widget_changed() const noexcept -> bool {
    return m_is_changed;
  }

  void basic_widget::clear() noexcept {
    for (auto &child : m_childs) {
      child->m_is_attached  = false;
      child->m_attach_index = 0;
      child->m_parent.reset();
    }

    m_childs.clear();
    set_expired(true);
  }

  auto basic_widget::attach(ptr_widget const &child) noexcept
      -> sl::index {
    if (!child)
      return -1;

    sl::index result = m_childs.size();

    if (auto p = child->m_parent.lock(); p)
      p->detach(child->m_attach_index);

    child->m_is_attached  = true;
    child->m_attach_index = m_childs.size();
    child->m_parent       = weak_from_this();

    m_childs.emplace_back(child);

    set_expired(true);

    return result;
  }

  void basic_widget::detach(ptr_widget const &child) noexcept {
    if (!child)
      return;

    if (child->m_is_attached &&
        child->m_parent.lock() == shared_from_this()) {
      if (child->m_attach_index >= 0 &&
          child->m_attach_index < m_childs.size()) {
        m_childs.erase(m_childs.begin() + child->m_attach_index);
        update_indices(child->m_attach_index);
      }

      child->m_is_attached  = false;
      child->m_attach_index = 0;
      child->m_parent.reset();
    }

    set_expired(true);
  }

  void basic_widget::detach(sl::index child_index) noexcept {
    if (child_index >= 0 && child_index < m_childs.size()) {
      m_childs[child_index]->m_is_attached  = false;
      m_childs[child_index]->m_attach_index = 0;
      m_childs[child_index]->m_parent.reset();

      m_childs.erase(m_childs.begin() + child_index);
      update_indices(child_index);

      set_expired(true);
    }
  }

  auto basic_widget::get_level() const noexcept -> sl::index {
    return m_level;
  }

  auto basic_widget::get_rect() const noexcept -> rect {
    return m_rect;
  }

  auto basic_widget::get_absolute_x() const noexcept -> sl::index {
    return m_absolute_x;
  }

  auto basic_widget::get_absolute_y() const noexcept -> sl::index {
    return m_absolute_y;
  }

  auto basic_widget::get_absolute_rect() const noexcept -> rect {
    return { .x      = m_absolute_x,
             .y      = m_absolute_y,
             .width  = m_rect.width,
             .height = m_rect.height };
  }

  void basic_widget::next_tab() noexcept {
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

  void basic_widget::refresh() noexcept {
    m_expired        = true;
    m_expired_childs = true;

    for (auto &w : m_childs) w->refresh();
  }

  void basic_widget::set_expired(bool is_expired) noexcept {
    if (is_expired) {
      if (auto p = m_parent.lock(); p)
        p->refresh_attached();

      refresh();

      m_is_changed = true;
    } else {
      m_expired        = false;
      m_expired_childs = false;
    }
  }

  auto basic_widget::is_expired() const noexcept -> bool {
    return m_expired;
  }

  auto basic_widget::has_attached_expired() const noexcept -> bool {
    return !m_childs.empty() && m_expired_childs;
  }

  auto basic_widget::is_visible() const noexcept -> bool {
    return m_is_visible;
  }

  auto basic_widget::is_enabled() const noexcept -> bool {
    return m_is_enabled;
  }

  auto basic_widget::is_attached() const noexcept -> bool {
    return m_is_attached;
  }

  auto basic_widget::has_focus() const noexcept -> bool {
    return m_has_focus;
  }

  auto basic_widget::get_attached_widget_count() const noexcept
      -> sl::index {
    return m_childs.size();
  }

  auto basic_widget::get_parent() const noexcept -> ptr_widget {
    return m_parent.lock();
  }

  auto basic_widget::get_attached_widget(sl::index n) const noexcept
      -> ptr_widget {
    if (n >= 0 && n < m_childs.size())
      return m_childs[n];
    return {};
  }

  void basic_widget::set_handler(bool is_handler) noexcept {
    m_is_handler = is_handler;
  }

  void basic_widget::update_indices(sl::index begin) noexcept {
    for (sl::index i = begin; i < m_childs.size(); i++)
      m_childs[i]->m_attach_index = i;
  }

  void basic_widget::adjust_layout() noexcept {
    if (!m_layout)
      return;

    auto childs  = sl::vector<sl::index> {};
    auto context = sl::vector<layout_context> {};

    childs.reserve(m_childs.size());
    context.reserve(m_childs.size());

    for (sl::index i = 0; i < m_childs.size(); i++) {
      auto &c = *m_childs[i];

      if (c.is_visible()) {
        childs.emplace_back(i);

        context.emplace_back(layout_context { .level = c.get_level(),
                                              .box = c.get_rect() });
      }
    }

    auto rects = m_layout(m_rect, context);

    if (rects.size() == childs.size()) {
      for (sl::index i = 0; i < childs.size(); i++)
        m_childs[childs[i]]->set_rect(rects[i]);
    } else {
      error_("Invalid layout.", __FUNCTION__);
    }
  }

  void basic_widget::refresh_attached() noexcept {
    for (auto p = shared_from_this(); p; p = p->m_parent.lock())
      p->m_expired_childs = true;
  }
}
