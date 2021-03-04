/*  apps/quadwar/aq_main.cpp
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

#include "../../laplace/platform/socket.h"
#include "../../laplace/platform/wrap.h"
#include "../../laplace/stem/config.h"
#include "quadwar.h"
#include <benchmark/benchmark.h>
#include <gtest/gtest.h>

namespace cfg = laplace::stem::config;

using cfg::scan_flag, cfg::f_tests, cfg::a_tests, cfg::f_benchmarks,
    cfg::a_benchmarks, cfg::f_run, cfg::a_run,
    laplace::socket_library, quadwar_app::quadwar;

auto run_tests(int &argc, char **argv) -> int {
  testing::InitGoogleTest(&argc, argv);
  socket_library sockets;
  return RUN_ALL_TESTS();
}

void run_benchmarks(int &argc, char **argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}

#if !defined(_CONSOLE) && (defined(_WIN32) || defined(_WINDOWS))
#  define USE_WINMAIN 1
#endif

#ifndef USE_WINMAIN
auto main(int argc, char **argv) -> int
#else
auto WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR args, int) -> int
#endif
{
#ifndef _CONSOLE
  freopen(log_file_name, "w", stdout);
#endif

#ifdef USE_WINMAIN
  auto [argc, argv] = parse_cmdline(args);
#endif

  int  status = 0;
  bool run    = true;

  if (scan_flag(argc, argv, f_tests, a_tests)) {
    run    = false;
    status = run_tests(argc, argv);
  }

  if (status == 0 && scan_flag(argc, argv, f_benchmarks, a_benchmarks)) {
    run = false;
    run_benchmarks(argc, argv);
  }

  if (scan_flag(argc, argv, f_run, a_run)) {
    run = true;
  }

  if (status == 0 && run) {
    socket_library sockets;
    status = quadwar(argc, argv).run();
  }

#if defined(_CONSOLE) && defined(LAPLACE_WAIT_FOR_ANY_KEY)
  std::cout << "\n  Press Enter... ";
  static_cast<void>(getchar());
#endif

  return status;
}
