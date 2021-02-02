#ifndef __quadwar__mainmenu__
#define __quadwar__mainmenu__

#include <laplace/ui/elem/textarea.h>
#include <laplace/ui/elem/textbutton.h>
#include <laplace/ui/elem/textedit.h>
#include <laplace/ui/widget.h>

namespace quadwar_app {
  using namespace laplace;

  class mainmenu {
  public:
    struct create_info {
      std::u8string game_name;
      std::u8string player_name;
      size_t        map_size     = 0;
      size_t        player_count = 0;
      size_t        unit_count   = 0;
    };

    struct join_info {
      std::string   server_ip;
      std::u8string game_name;
      std::u8string player_name;
    };

    using event_create = std::function<void(create_info)>;
    using event_join   = std::function<void(join_info)>;
    using event_quit   = std::function<void()>;

    static constexpr size_t spacing     = 4;
    static constexpr size_t line_count  = 7;
    static constexpr size_t menu_width  = 640;
    static constexpr size_t info_height = 80;
    static constexpr size_t line_height = 60;

    static constexpr auto c_welcome      = u8"Welcome";
    static constexpr auto c_create       = u8"Create";
    static constexpr auto c_join         = u8"Join";
    static constexpr auto c_quit         = u8"Quit";
    static constexpr auto c_create_game  = u8"Create Game";
    static constexpr auto c_join_game    = u8"Join Game";
    static constexpr auto c_server_ip    = u8"Server IP";
    static constexpr auto c_game_name    = u8"Game name";
    static constexpr auto c_player_name  = u8"Player name";
    static constexpr auto c_map_size     = u8"Map size";
    static constexpr auto c_player_count = u8"Player count";
    static constexpr auto c_unit_count   = u8"Unit count";
    static constexpr auto c_cancel       = u8"Cancel";
    static constexpr auto c_continue     = u8"Continue";

    mainmenu();
    ~mainmenu();

    void on_create(event_create ev);
    void on_join(event_join ev);
    void on_quit(event_quit ev);

    void refresh();

    void set_visible(bool is_visible);

    void attach_to(ui::ptr_widget w);
    void adjust_layout(size_t width, size_t height);

  private:
    enum class page { root, create, join };

    page m_current_page = page::root;
    bool m_is_visible   = true;

    ui::elem::ptr_textarea m_info =
        std::make_shared<ui::elem::textarea>();
    ui::ptr_widget m_page_root = std::make_shared<ui::widget>();
    ui::ptr_widget m_page_create =
        std::make_shared<ui::widget>();
    ui::ptr_widget m_page_join = std::make_shared<ui::widget>();

    ui::elem::ptr_textbutton m_create =
        std::make_shared<ui::elem::textbutton>();
    ui::elem::ptr_textbutton m_join =
        std::make_shared<ui::elem::textbutton>();
    ui::elem::ptr_textbutton m_quit =
        std::make_shared<ui::elem::textbutton>();

    ui::elem::ptr_textarea m_c_game_name_label =
        std::make_shared<ui::elem::textarea>();
    ui::elem::ptr_textarea m_c_player_name_label =
        std::make_shared<ui::elem::textarea>();
    ui::elem::ptr_textarea m_c_map_size_label =
        std::make_shared<ui::elem::textarea>();
    ui::elem::ptr_textarea m_c_player_count_label =
        std::make_shared<ui::elem::textarea>();
    ui::elem::ptr_textarea m_c_unit_count_label =
        std::make_shared<ui::elem::textarea>();

    ui::elem::ptr_textedit m_c_game_name =
        std::make_shared<ui::elem::textedit>();
    ui::elem::ptr_textedit m_c_player_name =
        std::make_shared<ui::elem::textedit>();
    ui::elem::ptr_textedit m_c_map_size =
        std::make_shared<ui::elem::textedit>();
    ui::elem::ptr_textedit m_c_player_count =
        std::make_shared<ui::elem::textedit>();
    ui::elem::ptr_textedit m_c_unit_count =
        std::make_shared<ui::elem::textedit>();

    ui::elem::ptr_textbutton m_c_cancel =
        std::make_shared<ui::elem::textbutton>();
    ui::elem::ptr_textbutton m_c_continue =
        std::make_shared<ui::elem::textbutton>();

    ui::elem::ptr_textarea m_j_server_ip_label =
        std::make_shared<ui::elem::textarea>();
    ui::elem::ptr_textarea m_j_game_name_label =
        std::make_shared<ui::elem::textarea>();
    ui::elem::ptr_textarea m_j_player_name_label =
        std::make_shared<ui::elem::textarea>();

    ui::elem::ptr_textedit m_j_server_ip =
        std::make_shared<ui::elem::textedit>();
    ui::elem::ptr_textedit m_j_game_name =
        std::make_shared<ui::elem::textedit>();
    ui::elem::ptr_textedit m_j_player_name =
        std::make_shared<ui::elem::textedit>();

    ui::elem::ptr_textbutton m_j_cancel =
        std::make_shared<ui::elem::textbutton>();
    ui::elem::ptr_textbutton m_j_continue =
        std::make_shared<ui::elem::textbutton>();
  };

  using ptr_mainmenu = std::shared_ptr<mainmenu>;
}

#endif
