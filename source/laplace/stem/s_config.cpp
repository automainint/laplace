#include "config.h"
#include "../core/utils.h"
#include "../core/embedded.h"
#include "../format/text.h"
#include "../format/utils.h"
#include <fstream>

using namespace laplace;
using namespace stem;
using namespace config;
using namespace core;
using namespace platform;
using namespace format;
using namespace std;

auto config::scan_flag(int argc, char **argv, char c) -> bool
{
    for (size_t i = 0; i < argc; i++)
    {
        if (argv[i] && argv[i][0] == '-')
        {
            for (size_t k = 1; argv[i][k]; k++)
            {
                if (argv[i][k] == c)
                    return true;
            }
        }
    }

    return false;
}

auto config::scan_flag(int argc, char **argv, string_view name) -> bool
{
    for (size_t i = 0; i < argc; i++)
    {
        if (argv[i] && argv[i][0] == '-' && argv[i][1] == '-')
        {
            if (memcmp(argv[i] + 2, name.data(), name.size()) == 0)
                return true;
        }
    }

    return false;
}

auto config::scan_flag(int argc, char **argv, char c, string_view name) -> bool
{
    return scan_flag(argc, argv, c) || scan_flag(argc, argv, name);
}

auto config::get_default() -> family
{
    family cfg;

    cfg[k_frame][0] = window::default_frame_width;
    cfg[k_frame][1] = window::default_frame_height;
    cfg[k_frame][2] = window::default_frame_rate;

    cfg[k_font]     = default_font;
    cfg[k_caption]  = default_caption;

    cfg[k_shaders][k_folder]                    = default_shaders_folder;
    cfg[k_shaders][k_flat_solid][k_vertex]      = default_shader_flat_solid_vertex;
    cfg[k_shaders][k_flat_solid][k_fragment]    = default_shader_flat_solid_fragment;
    cfg[k_shaders][k_flat_sprite][k_vertex]     = default_shader_flat_sprite_vertex;
    cfg[k_shaders][k_flat_sprite][k_fragment]   = default_shader_flat_sprite_fragment;

    return cfg;
}

auto read_config(char **tag, char **end, ref_family cfg) -> size_t
{
    if (tag < end && tag[0])
    {
        cfg[k_file] = string(tag[0]);

        ifstream in(tag[0]);

        auto data = text::decode(wrap(in));

        if (data)
        {
            cfg.merge(*data);
        }
    }

    return 1;
}

auto read_frame_size(char **tag, char **end, ref_family cfg) -> size_t
{
    if (tag + 2 < end && tag[0] && tag[1] && tag[2])
    {
        cfg[k_frame][0] = atoi(tag[0]);
        cfg[k_frame][1] = atoi(tag[1]);
        cfg[k_frame][2] = atoi(tag[2]);
    }

    return 3;
}

auto process_tag(char **arg, char **end, ref_family cfg) -> char **
{
    if (arg && *arg && arg < end)
    {
        auto tag = *arg;

        if (tag[0] == '-')
        {
            if (tag[1] == '-')
            {
                auto name = tag + 2;

                if (strcmp(name, a_tests) == 0)
                {
                    /*  Run unit-tests. Handles by the user.
                     */
                }
                else if (strcmp(name, a_benchmarks) == 0)
                {
                    /*  Run benchmarks. Handles by the user.
                     */
                }
                else if (strcmp(name, a_config) == 0)
                {
                    arg += read_config(arg + 1, end, cfg);
                }
                else if (strcmp(name, a_frame) == 0)
                {
                    arg += read_frame_size(arg + 1, end, cfg);
                }
                else
                {
                    cerr    << "Unknown command line argument '"
                            << tag << "'." << endl;
                }
            }
            else
            {
                for (size_t i = 1; tag[i]; i++)
                {
                    if (tag[i] == f_tests)
                    {
                        /*  Run unit-tests. Handles by the user.
                         */
                    }
                    else if (tag[i] == f_benchmarks)
                    {
                        /*  Run benchmarks. Handles by the user.
                         */
                    }
                    else if (tag[i] == f_config)
                    {
                        arg += read_config(arg + 1, end, cfg);
                    }
                    else if (tag[i] == f_frame)
                    {
                        arg += read_frame_size(arg + 1, end, cfg);
                    }
                    else
                    {
                        cerr    << "Unknown command line flag '"
                                << tag[i] << "' in argument '"
                                << tag << "'." << endl;
                    }
                }
            }
        }
        else
        {
            cerr    << "Command line argument '"
                    << tag <<"' ignored." << endl;
        }

        return arg + 1;
    }

    return nullptr;
}

auto config::load(int argc, char **argv, cref_family def_cfg) -> family
{
    family cfg = def_cfg;

    auto arg = argv + 1;
    auto end = argv + argc;

    while (arg = process_tag(arg, end, cfg));

    return cfg;
}

void config::save(cref_family cfg)
{
    if (cfg.has(k_file))
    {
        auto file_name = to_wstring(cfg[k_file].get_string());

        if (!embedded::scan(file_name))
        {
            ofstream out(file_name);

            if (!text::encode(wrap(out), cfg))
            {
                wcerr << L"Unable to save config file " << file_name << L"." << endl;
            }
        }
    }
}
