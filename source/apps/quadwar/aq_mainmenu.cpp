#include "mainmenu.h"
#include <laplace/core/utils.h>
#include <iostream>

using namespace quadwar_app;
using namespace std;

mainmenu::mainmenu()
{
    m_page_root->attach(m_create);
    m_page_root->attach(m_join);
    m_page_root->attach(m_quit);

    m_page_create->attach(m_c_game_name_label);
    m_page_create->attach(m_c_player_name_label);
    m_page_create->attach(m_c_map_size_label);
    m_page_create->attach(m_c_player_count_label);
    m_page_create->attach(m_c_unit_count_label);
    m_page_create->attach(m_c_game_name);
    m_page_create->attach(m_c_player_name);
    m_page_create->attach(m_c_map_size);
    m_page_create->attach(m_c_player_count);
    m_page_create->attach(m_c_unit_count);
    m_page_create->attach(m_c_cancel);
    m_page_create->attach(m_c_continue);

    m_page_join->attach(m_j_server_ip_label);
    m_page_join->attach(m_j_game_name_label);
    m_page_join->attach(m_j_player_name_label);
    m_page_join->attach(m_j_server_ip);
    m_page_join->attach(m_j_game_name);
    m_page_join->attach(m_j_player_name);
    m_page_join->attach(m_j_cancel);
    m_page_join->attach(m_j_continue);

    m_info      ->set_text(c_welcome);
    m_create    ->set_text(c_create);
    m_join      ->set_text(c_join);
    m_quit      ->set_text(c_quit);

    m_c_game_name_label     ->set_text(c_game_name);
    m_c_player_name_label   ->set_text(c_player_name);
    m_c_map_size_label      ->set_text(c_map_size);
    m_c_player_count_label  ->set_text(c_player_count);
    m_c_unit_count_label    ->set_text(c_unit_count);
    m_c_cancel              ->set_text(c_cancel);
    m_c_continue            ->set_text(c_continue);

    m_j_server_ip_label     ->set_text(c_server_ip);
    m_j_game_name_label     ->set_text(c_game_name);
    m_j_player_name_label   ->set_text(c_player_name);
    m_j_cancel              ->set_text(c_cancel);
    m_j_continue            ->set_text(c_continue);

    auto num_filter = [](char32_t c) -> bool {
        return c >= '0' && c <= '9';
    };

    m_c_map_size->setup_filter(num_filter);
    m_c_map_size->set_length_limit(4);
    m_c_player_count->setup_filter(num_filter);
    m_c_player_count->set_length_limit(3);
    m_c_unit_count->setup_filter(num_filter);
    m_c_unit_count->set_length_limit(3);

    m_j_server_ip->setup_filter([](char32_t c) -> bool {
        return (c >= '0' && c <= '9') || c == '.' || c == ':';
    });

    m_page_create->set_visible(false);
    m_page_join->set_visible(false);

    m_create->on_click([=](ui::ptr_widget) {
        m_current_page = page::create;

        m_page_root->set_visible(false);
        m_page_create->set_visible(true);

        m_info->set_text(c_create_game);
    });

    m_join->on_click([=](ui::ptr_widget) {
        m_current_page = page::join;

        m_page_root->set_visible(false);
        m_page_join->set_visible(true);

        m_info->set_text(c_join_game);
    });

    m_c_cancel->on_click([=](ui::ptr_widget) {
        m_current_page = page::root;

        m_page_root->set_visible(true);
        m_page_create->set_visible(false);

        m_info->set_text(c_welcome);
    });

    m_j_cancel->on_click([=](ui::ptr_widget) {
        m_current_page = page::root;

        m_page_root->set_visible(true);
        m_page_join->set_visible(false);

        m_info->set_text(c_welcome);
    });
}

mainmenu::~mainmenu()
{
    if (auto p = m_info->get_parent(); p)
    {
        p->detach(m_info);
        p->detach(m_page_root);
        p->detach(m_page_create);
        p->detach(m_page_join);
    }
}

void mainmenu::on_create(mainmenu::event_create ev)
{
    m_c_continue->on_click([=](ui::ptr_widget) {
        create_info info;

        info.game_name      = m_c_game_name->get_text();
        info.player_name    = m_c_player_name->get_text();
        info.map_size       = to_integer(m_c_map_size->get_text());
        info.player_count   = to_integer(m_c_player_count->get_text());
        info.unit_count     = to_integer(m_c_unit_count->get_text());

        ev(info);
    });
}

void mainmenu::on_join(mainmenu::event_join ev)
{
    m_j_continue->on_click([=](ui::ptr_widget) {
        join_info info;

        info.server_ip      = to_string(m_j_server_ip->get_text());
        info.game_name      = m_j_game_name->get_text();
        info.player_name    = m_j_player_name->get_text();

        ev(info);
    });
}

void mainmenu::on_quit(mainmenu::event_quit ev)
{
    m_quit->on_click([ev](ui::ptr_widget) { ev(); });
}

void mainmenu::refresh()
{
    m_current_page = page::root;

    if (m_is_visible)
    {
        m_page_root->set_visible(true);
        m_page_create->set_visible(false);
        m_page_join->set_visible(false);
    }

    m_info->set_text(c_welcome);
}

void mainmenu::attach_to(ui::ptr_widget w)
{
    if (w)
    {
        w->attach(m_info);

        w->attach(m_page_root);
        w->attach(m_page_create);
        w->attach(m_page_join);
    }
}

void mainmenu::adjust_layout(size_t width, size_t height)
{
    size_t lines_height = line_height * line_count;
    size_t full_height = lines_height + info_height;
    size_t half_menu_width = menu_width / 2;

    int x0 = (static_cast<int>(width) - menu_width) / 2;
    int y0 = (static_cast<int>(height) - full_height) / 2;

    m_info->set_rect(ui::rect {
        static_cast<int>(x0 + spacing),
        static_cast<int>(y0 + spacing),
        menu_width - spacing * 2,
        info_height - spacing * 2
    });

    auto lines_rect = ui::rect {
        x0,
        static_cast<int>(y0 + info_height),
        menu_width,
        lines_height
    };

    m_page_root->set_rect(lines_rect);
    m_page_create->set_rect(lines_rect);
    m_page_join->set_rect(lines_rect);

    /*  Page: Root
     */

    m_create->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing),
        menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_join->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height),
        menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_quit->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 2),
        menu_width - spacing * 2,
        line_height - spacing * 2
    });

    /*  Page: Create
     */
    
    m_c_game_name_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_game_name->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_player_name_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_player_name->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_map_size_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 2),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_map_size->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height * 2),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_player_count_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 3),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_player_count->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height * 3),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_unit_count_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 4),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_unit_count->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height * 4),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_cancel->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 5),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_c_continue->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height * 5),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    /*  Page: Join
     */
    
    m_j_server_ip_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_server_ip->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_game_name_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_game_name->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_player_name_label->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 2),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_player_name->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height * 2),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_cancel->set_rect(ui::rect {
        static_cast<int>(spacing),
        static_cast<int>(spacing + line_height * 3),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });

    m_j_continue->set_rect(ui::rect {
        static_cast<int>(spacing + half_menu_width),
        static_cast<int>(spacing + line_height * 3),
        half_menu_width - spacing * 2,
        line_height - spacing * 2
    });
}

void mainmenu::set_visible(bool is_visible)
{
    if (m_is_visible != is_visible)
    {
        m_is_visible = is_visible;
        m_info->set_visible(is_visible);

        switch (m_current_page)
        {
        case page::root:
            m_page_root->set_visible(is_visible);
            break;

        case page::create:
            m_page_create->set_visible(is_visible);
            break;

        case page::join:
            m_page_join->set_visible(is_visible);
            break;
        }
    }
}
