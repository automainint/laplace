/*  laplace/ui/widget.h
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

#ifndef laplace_ui_widget_h
#define laplace_ui_widget_h

#include "../platform/wrap.h"
#include "layout.h"
#include <memory>
#include <vector>

namespace laplace::ui {
  class context;

  using ptr_context = std::shared_ptr<context>;

  /*  UI Widget base class.
   */
  class widget : public std::enable_shared_from_this<widget> {
  public:
    using ptr_widget  = std::shared_ptr<widget>;
    using vptr_widget = std::vector<ptr_widget>;

    virtual ~widget() = default;

    /*  Widget live loop. Returns true if
     *  any event was handled.
     */
    virtual auto tick(size_t delta_msec, platform::ref_input in, bool is_handled)
        -> bool;

    virtual void render();

    /*  Check if the widget allowed to handle
     *  an event in specified location.
     */
    virtual auto event_allowed(int x, int y) -> bool;

    void set_layout(layout fn);
    void set_level(size_t level);
    void set_rect(cref_rect r);
    void set_visible(bool state);
    void set_enabled(bool state);
    void set_focus(bool has_focus);
    void move_to(int x, int y);

    void clear();
    auto attach(ptr_widget child) -> size_t;
    void detach(ptr_widget child);
    void detach(size_t child_index);

    auto get_level() const -> size_t;
    auto get_rect() const -> cref_rect;

    auto get_absolute_x() const -> int;
    auto get_absolute_y() const -> int;
    auto get_absolute_rect() const -> rect;

    /*  Set focus to next enabled
     *  widget for childs.
     */
    void next_tab();

    /*  Set expiration state for the widget
     *  and all the childs. It will cause
     *  full redrawing.
     */
    void refresh();

    /*  Set the expiration state for
     *  this widget.
     */
    void set_expired(bool is_expired);

    auto is_expired() const -> bool;
    auto has_childs_expired() const -> bool;

    auto is_visible() const -> bool;
    auto is_enabled() const -> bool;
    auto is_attached() const -> bool;
    auto has_focus() const -> bool;
    auto get_child_count() const -> size_t;

    /*  Get parent widget.
     */
    auto get_parent() const -> ptr_widget;

    /*  Get child widget by index.
     */
    auto get_child(size_t index) const -> ptr_widget;

    static void prepare();
    static auto get_default_context() -> ptr_context;

  protected:
    /*  Set if the widget can handle focus.
     */
    void set_handler(bool is_handler);

    void draw_childs();
    void up_to_date();

    auto widget_tick(size_t delta_msec, platform::ref_input in, bool is_handled)
        -> bool;
    void widget_render();

    auto is_widget_changed() -> bool;

  private:
    void update_indices(size_t begin);
    void adjust_layout();
    void refresh_childs();

    bool m_expired        = true;
    bool m_expired_childs = true;

    layout m_layout;
    size_t m_level = 0;
    rect   m_rect;
    int    m_absolute_x   = 0;
    int    m_absolute_y   = 0;
    bool   m_is_changed   = true;
    bool   m_is_visible   = true;
    bool   m_is_enabled   = true;
    bool   m_is_handler   = false;
    bool   m_is_attached  = false;
    bool   m_has_focus    = false;
    size_t m_attach_index = 0;
    size_t m_focus_index  = 0;

    vptr_widget           m_childs;
    std::weak_ptr<widget> m_parent;
  };

  using ptr_widget  = widget::ptr_widget;
  using vptr_widget = widget::vptr_widget;
}

#endif
