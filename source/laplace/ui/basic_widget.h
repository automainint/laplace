/*  laplace/ui/basic_widget.h
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
  class basic_widget
      : public std::enable_shared_from_this<basic_widget> {
  public:
    using ptr_widget  = std::shared_ptr<basic_widget>;
    using vptr_widget = sl::vector<ptr_widget>;

    basic_widget() noexcept                     = default;
    basic_widget(basic_widget const &) noexcept = default;
    basic_widget(basic_widget &&) noexcept      = default;
    auto operator         =(basic_widget const &) noexcept
        -> basic_widget & = default;
    auto operator         =(basic_widget &&) noexcept
        -> basic_widget & = default;

    virtual ~basic_widget() noexcept = default;

    virtual void tick(sl::time                 delta_msec,
                      core::cref_input_handler in) noexcept;

    virtual void render(context const &con) noexcept;

    /*  Check if the widget allowed to handle
     *  an event in specified location.
     */
    [[nodiscard]] virtual auto event_allowed(sl::index x,
                                             sl::index y) noexcept
        -> bool;

    void set_layout(layout fn) noexcept;
    void set_level(sl::index level) noexcept;
    void set_rect(cref_rect r) noexcept;
    void set_visible(bool state) noexcept;
    void set_enabled(bool state) noexcept;
    void set_focus(bool has_focus) noexcept;

    void clear() noexcept;
    auto attach(ptr_widget const &child) noexcept -> sl::index;
    void detach(ptr_widget const &child) noexcept;
    void detach(sl::index child_index) noexcept;

    [[nodiscard]] auto get_level() const noexcept -> sl::index;
    [[nodiscard]] auto get_rect() const noexcept -> rect;
    [[nodiscard]] auto get_absolute_x() const noexcept -> sl::index;
    [[nodiscard]] auto get_absolute_y() const noexcept -> sl::index;
    [[nodiscard]] auto get_absolute_rect() const noexcept -> rect;

    /*  Set focus to next enabled widget for childs.
     */
    void next_tab() noexcept;

    /*  Set expiration state for the widget and all the childs. It
     *  will cause full redrawing.
     */
    void refresh() noexcept;

    /*  Set the expiration state for this widget.
     */
    void set_expired(bool is_expired) noexcept;

    [[nodiscard]] auto is_expired() const noexcept -> bool;
    [[nodiscard]] auto has_childs_expired() const noexcept -> bool;

    [[nodiscard]] auto is_visible() const noexcept -> bool;
    [[nodiscard]] auto is_enabled() const noexcept -> bool;
    [[nodiscard]] auto is_attached() const noexcept -> bool;
    [[nodiscard]] auto has_focus() const noexcept -> bool;
    [[nodiscard]] auto get_child_count() const noexcept -> sl::whole;

    /*  Get parent widget.
     */
    [[nodiscard]] auto get_parent() const noexcept -> ptr_widget;

    /*  Get child widget by index.
     */
    [[nodiscard]] auto get_child(sl::index index) const noexcept
        -> ptr_widget;

  protected:
    /*  Set if the widget can handle focus.
     */
    void set_handler(bool is_handler) noexcept;

    void update_root() noexcept;
    void update_child() noexcept;
    void draw_childs(context const &con) noexcept;
    void up_to_date() noexcept;

    void widget_tick(sl::time                 delta_msec,
                     core::cref_input_handler in) noexcept;

    void widget_render(context const &con) noexcept;

    [[nodiscard]] auto is_widget_changed() const noexcept -> bool;

  private:
    void update_indices(sl::index begin) noexcept;
    void adjust_layout() noexcept;
    void refresh_childs() noexcept;

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

    vptr_widget                 m_childs;
    std::weak_ptr<basic_widget> m_parent;
  };

  using ptr_widget = basic_widget::ptr_widget;
}

#endif
