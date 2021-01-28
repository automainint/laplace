#include "quadwar.h"
#include <laplace/platform/wrap.h>
#include <laplace/platform/socket.h>
#include <laplace/stem/config.h>
#include <gtest/gtest.h>

using namespace quadwar_app;
using namespace platform;
using namespace stem::config;
using namespace std;

auto run_tests(int &argc, char **argv) -> bool
{
    if (scan_flag(argc, argv, f_tests, a_tests))
    {
        socket_library sockets;

        testing::InitGoogleTest(&argc, argv);

        return RUN_ALL_TESTS() == 0;
    }

    return true;
}

#if defined(_CONSOLE) || (!defined(_WIN32) && !defined(_WINDOWS))
auto main(int argc, char **argv) -> int
#else
auto WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) -> int
#endif
{
    int status = -1;

#   ifndef _CONSOLE
    freopen(log_file_name, "w", stdout);
#   endif

    if (run_tests(argc, argv)) {
        /*  Initialize web socket library.
         */
        socket_library sockets;

        /*  Start the application.
         */
        status = quadwar(argc, argv).run();
    }

#   ifdef _CONSOLE
    cout << endl << "  Press Enter... ";
    getchar();
#   endif

    return status;
}
