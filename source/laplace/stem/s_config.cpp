/*  Copyright (c) 2021 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#include "config.h"

#include "../core/embedded.h"
#include "../core/string.h"
#include "../core/utils.h"
#include "../format/text.h"
#include "../format/utils.h"
#include <filesystem>
#include <fstream>

namespace laplace::stem::config {
  namespace text = format::text;
  namespace fs   = std::filesystem;

  using std::string_view, std::string, std::pair, std::ifstream,
      std::ofstream, core::unival, format::wrap;

  auto scan_flag(int argc, char **argv, char c) -> bool {
    for (auto i = 0; i < argc; i++)
      if (argv[i] != nullptr && argv[i][0] == '-')
        for (auto k = 1; argv[i][k] != '\0'; k++)
          if (argv[i][k] == c)
            return true;

    return false;
  }

  auto scan_flag(int argc, char **argv, string_view name) -> bool {
    for (auto i = 0; i < argc; i++)
      if (argv[i] != nullptr && argv[i][0] == '-' &&
          argv[i][1] == '-')
        if (string { argv[i] + 2 } == name)
          return true;

    return false;
  }

  auto scan_flag(int argc, char **argv, char c, string_view name)
      -> bool {
    return scan_flag(argc, argv, c) || scan_flag(argc, argv, name);
  }

  static constexpr auto argv_size        = 64;
  static constexpr auto argv_string_size = 256;

  static int   g_argc                                   = 0;
  static char *g_argv[argv_size]                        = { nullptr };
  static char  g_argv_data[argv_size][argv_string_size] = { 0 };

  auto parse_cmdline(const char *args, log_handler log)
      -> pair<int, char **> {
    if (args == nullptr)
      return { g_argc, g_argv };

    for (auto i = 0; args[i] != '\0'; i++) {
      if (args[i] == ' ')
        continue;

      auto j     = i + 1;
      bool quote = false;

      for (; args[j] != '\0' && args[j] != ' ' && !quote; j++) {
        if (args[j] == '\\' && args[j + 1] != '\0') {
          j++;
        } else if (args[j] == '"') {
          quote = !quote;
        }
      }

      if (i >= j) {
        i += j;
        continue;
      }

      if (g_argc >= argv_size) {
        log(log_event::error, "Too many command line arguments.",
            __FUNCTION__);
        break;
      }

      const auto size = j - i;

      if (size >= argv_string_size) {
        log(log_event::error, "Too long command line argument.",
            __FUNCTION__);
        break;
      }

      memcpy(g_argv_data[g_argc], args + i, size);
      g_argv[g_argc] = g_argv_data[g_argc];
      g_argc++;
    }

    return { g_argc, g_argv };
  }

  auto get_default() -> unival {
    auto cfg = unival {};

    cfg[k_frame][0] = 960;
    cfg[k_frame][1] = 720;
    cfg[k_frame][2] = 24;

    cfg[k_font]    = default_font;
    cfg[k_caption] = default_caption;

    cfg[k_shaders][k_folder] = default_shaders_folder;
    cfg[k_shaders][k_flat_solid][k_vertex] =
        default_shader_flat_solid_vertex;
    cfg[k_shaders][k_flat_solid][k_fragment] =
        default_shader_flat_solid_fragment;
    cfg[k_shaders][k_flat_sprite][k_vertex] =
        default_shader_flat_sprite_vertex;
    cfg[k_shaders][k_flat_sprite][k_fragment] =
        default_shader_flat_sprite_fragment;

    return cfg;
  }

  auto read_config(char **tag, char **end, unival &cfg) -> int {
    if (tag < end && tag[0] != nullptr) {
      cfg[k_file] = string { tag[0] };

      auto in   = ifstream { tag[0] };
      auto data = text::decode(wrap(in));

      if (data) {
        cfg.merge(*data);
      }
    }

    return 1;
  }

  auto read_frame_size(char **tag, char **end, unival &cfg) -> int {
    if (tag + 2 < end && tag[0] != nullptr && tag[1] != nullptr &&
        tag[2] != nullptr) {
      cfg[k_frame][0] = atoi(tag[0]);
      cfg[k_frame][1] = atoi(tag[1]);
      cfg[k_frame][2] = atoi(tag[2]);
    }

    return 3;
  }

  auto process_tag(char **arg, char **end, unival &cfg,
                   log_handler log) -> char ** {
    if (arg && *arg && arg < end) {
      auto tag = *arg;

      if (tag[0] == '-') {
        if (tag[1] == '-') {
          auto name = tag + 2;

          if (strcmp(name, a_tests) == 0) {
            /*  Run unit-tests. Handles by the user.
             */
          } else if (strcmp(name, a_benchmarks) == 0) {
            /*  Run benchmarks. Handles by the user.
             */
          } else if (strcmp(name, a_run) == 0) {
            /*  Run the app. Handles by the user.
             */
          } else if (strcmp(name, a_config) == 0) {
            arg += read_config(arg + 1, end, cfg);
          } else if (strcmp(name, a_frame) == 0) {
            arg += read_frame_size(arg + 1, end, cfg);
          } else {
            log(log_event::warning,
                fmt("Unknown command line argument '%s'.", tag),
                "Stem/Config");
          }
        } else {
          for (auto i = 1; tag[i]; i++) {
            if (tag[i] == f_tests) {
              /*  Run unit-tests. Handles by the user.
               */
            } else if (tag[i] == f_benchmarks) {
              /*  Run benchmarks. Handles by the user.
               */
            } else if (tag[i] == f_run) {
              /*  Run the app. Handles by the user.
               */
            } else if (tag[i] == f_config) {
              arg += read_config(arg + 1, end, cfg);
            } else if (tag[i] == f_frame) {
              arg += read_frame_size(arg + 1, end, cfg);
            } else {
              log(log_event::warning,
                  fmt("Unknown command line flag '%c' in argument "
                      "'%s'.",
                      tag[i], tag),
                  "Stem/Config");
            }
          }
        }
      } else {
        log(log_event::warning,
            fmt("Command line argument '%s' ignored.", tag),
            "Stem/Config");
      }

      return arg + 1;
    }

    return nullptr;
  }

  auto load(int argc, char **argv, unival const &def_cfg,
            log_handler log) -> unival {
    auto cfg = unival { def_cfg };

    auto arg = argv + 1;
    auto end = argv + argc;

    while (arg) { arg = process_tag(arg, end, cfg, log); }

    return cfg;
  }

  void save(unival const &cfg, log_handler log) {
    if (!cfg.has(k_file))
      return;

    auto file_name = to_wstring(cfg[k_file].get_string());

    if (embedded::scan(file_name))
      return;

    auto out = ofstream(fs::path(file_name));
    auto w   = wrap(out);

    if (!text::encode(w, cfg))
      log(log_event::error,
          fmt("Unable to save config file '%s'.",
              to_string(file_name).c_str()),
          __FUNCTION__);
  }
}
