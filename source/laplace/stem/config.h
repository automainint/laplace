/*  Copyright (c) 2022 Mitya Selivanov
 *
 *  This file is part of the Laplace project.
 *
 *  Laplace is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty
 *  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
 *  the MIT License for more details.
 */

#ifndef LAPLACE_STEM_CONFIG_H
#define LAPLACE_STEM_CONFIG_H

#include "../core/unival.h"

namespace laplace::stem::config {
  /*  Command-line arguments.
   *
   *  Run unit-tests:
   *      -T
   *      --tests
   *
   *  Run benchmarks:
   *      -B
   *      --bench
   *
   *  Run the app:
   *      -R
   *      --run
   *
   *  Set the config file name:
   *      -c <file_name>
   *      --config <file_name>
   *
   *  Set the frame size:
   *      -f <width> <height> <rate>
   *      --frame <width> <height> <rate>
   *
   *  Multiple flags are allowed, e.g.
   *      -Tcf <config_file> <frame_width> <frame_height> <frame_rate>
   */

  constexpr auto f_tests = 'T';
  constexpr auto a_tests = "tests";

  constexpr auto f_benchmarks = 'B';
  constexpr auto a_benchmarks = "bench";

  constexpr auto f_run = 'R';
  constexpr auto a_run = "run";

  constexpr auto f_config = 'c';
  constexpr auto a_config = "config";

  constexpr auto f_frame = 'f';
  constexpr auto a_frame = "frame";

  /*  Config key-strings.
   */

  constexpr auto k_file    = "file";
  constexpr auto k_frame   = "frame";
  constexpr auto k_font    = "font";
  constexpr auto k_caption = "caption";

  constexpr auto k_shaders     = "shaders";
  constexpr auto k_folder      = "folder";
  constexpr auto k_geometry    = "geometry";
  constexpr auto k_vertex      = "vertex";
  constexpr auto k_fragment    = "fragment";
  constexpr auto k_flat_solid  = "flat_solid";
  constexpr auto k_flat_sprite = "flat_sprite";

  /*  Default config.
   */

  constexpr auto default_caption = u8"Laplace";
  constexpr auto default_font    = u8":/default.ttf";

  constexpr auto default_shaders_folder = u8":/shaders/";

  constexpr auto default_shader_flat_solid_vertex =
      u8"flat/solid.vert";

  constexpr auto default_shader_flat_solid_fragment =
      u8"flat/solid.frag";

  constexpr auto default_shader_flat_sprite_vertex =
      u8"flat/sprite.vert";

  constexpr auto default_shader_flat_sprite_fragment =
      u8"flat/sprite.frag";

  auto scan_flag(int argc, char **argv, char c) -> bool;

  auto scan_flag(int argc, char **argv, std::string_view name)
      -> bool;

  auto scan_flag(int argc, char **argv, char c, std::string_view name)
      -> bool;

  auto get_default() -> core::unival;

  auto parse_cmdline(const char *args, log_handler log)
      -> std::pair<int, char **>;

  auto load(int argc, char **argv, core::unival const &def_cfg,
            log_handler log) -> core::unival;

  void save(core::unival const &cfg, log_handler log);
}

#endif
