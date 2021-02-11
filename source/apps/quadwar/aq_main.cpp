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

using cfg::scan_flag, cfg::f_tests, cfg::a_tests,
    cfg::f_benchmarks, cfg::a_benchmarks,
    laplace::socket_library, quadwar_app::quadwar;

auto run_tests(int &argc, char **argv) -> bool {
  if (scan_flag(argc, argv, f_tests, a_tests)) {
    socket_library sockets;

    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS() == 0;
  }

  return true;
}

void run_benchmarks(int &argc, char **argv) {
  if (scan_flag(argc, argv, f_benchmarks, a_benchmarks)) {
    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
  }
}

#if defined(_CONSOLE) || \
    (!defined(_WIN32) && !defined(_WINDOWS))
auto main(int argc, char **argv) -> int
#else
auto WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) -> int
#endif
{
  int status = -1;

#ifndef _CONSOLE
  freopen(log_file_name, "w", stdout);

  int    argc = 0;
  char **argv = nullptr;
#endif

  if (run_tests(argc, argv)) {
    run_benchmarks(argc, argv);

    /*  Initialize web socket library.
     */
    socket_library sockets;

    /*  Start the application.
     */
    status = quadwar(argc, argv).run();
  }

#ifdef _CONSOLE
  std::cout << "\n  Press Enter... ";
  static_cast<void>(getchar());
#endif

  return status;
}
