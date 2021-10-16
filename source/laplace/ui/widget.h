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

#include "../core/input_handler.h"
#include "context.h"
#include "layout.h"
#include <memory>
#include <vector>

namespace laplace::ui {
  /*  UI Widget base class.
   */
  class widget : public std::enable_shared_from_this<widget> {
  public:
    using ptr_widget  = std::shared_ptr<widget>;
    using vptr_widget = sl::vector<ptr_widget>;

    virtual ~widget() = default;

    virtual void tick(sl::time                 delta_msec,
                      core::cref_input_handler in);

    virtual void render(context const &con);

    /*  Check if the widget allowed to handle
     *  an event in specified location.
     */
    [[nodiscard]] virtual auto event_allowed(sl::index x, sl::index y)
        -> bool;

    void set_layout(layout fn);
    void set_level(sl::index level);
    void set_rect(cref_rect r);
    void set_visible(bool state);
    void set_enabled(bool state);
    void set_focus(bool has_focus);
    void move_to(sl::index x, sl::index y);

    void clear();
    auto attach(ptr_widget child) -> sl::index;
    void detach(ptr_widget child);
    void detach(sl::index child_index);

    [[nodiscard]] auto get_level() const -> sl::index;
    [[nodiscard]] auto get_rect() const -> rect;
    [[nodiscard]] auto get_absolute_x() const -> sl::index;
    [[nodiscard]] auto get_absolute_y() const -> sl::index;
    [[nodiscard]] auto get_absolute_rect() const -> rect;

    /*  Set focus to next enabled widget for childs.
     */
    void next_tab();

    /*  Set expiration state for the widget and all the childs. It
     *  will cause full redrawing.
     */
    void refresh();

    /*  Set the expiration state for this widget.
     */
    void set_expired(bool is_expired);

    [[nodiscard]] auto is_expired() const -> bool;
    [[nodiscard]] auto has_childs_expired() const -> bool;

    [[nodiscard]] auto is_visible() const -> bool;
    [[nodiscard]] auto is_enabled() const -> bool;
    [[nodiscard]] auto is_attached() const -> bool;
    [[nodiscard]] auto has_focus() const -> bool;
    [[nodiscard]] auto get_child_count() const -> sl::whole;

    /*  Get parent widget.
     */
    [[nodiscard]] auto get_parent() const -> ptr_widget;

    /*  Get child widget by index.
     */
    [[nodiscard]] auto get_child(sl::index index) const -> ptr_widget;

  protected:
    /*  Set if the widget can handle focus.
     */
    void set_handler(bool is_handler);

    void prepare(context const &con);
    void draw_childs(context const &con);
    void up_to_date();

    void widget_tick(sl::time                 delta_msec,
                     core::cref_input_handler in);

    void widget_render(context const &con);

    [[nodiscard]] auto is_widget_changed() -> bool;

  private:
    void update_indices(sl::index begin);
    void adjust_layout();
    void refresh_childs();

    static std::weak_ptr<context> m_default_context;

    bool m_expired        = true;
    bool m_expired_childs = true;

    layout    m_layout;
    sl::index m_level = 0;
    rect      m_rect;
    sl::index m_absolute_x   = 0;
    sl::index m_absolute_y   = 0;
    bool      m_is_changed   = true;
    bool      m_is_visible   = true;
    bool      m_is_enabled   = true;
    bool      m_is_handler   = false;
    bool      m_is_attached  = false;
    bool      m_has_focus    = false;
    sl::index m_attach_index = 0;
    sl::index m_focus_index  = 0;

    vptr_widget           m_childs;
    std::weak_ptr<widget> m_parent;
  };

  using ptr_widget  = widget::ptr_widget;
  using vptr_widget = widget::vptr_widget;
}

#endif
