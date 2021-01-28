#include "factory.h"
#include "protocol/ids.h"
#include "protocol/elementary_events.h"
#include "protocol/public_key.h"
#include "protocol/request_events.h"
#include "protocol/server_clock.h"
#include "protocol/server_seed.h"
#include "protocol/client_ping.h"
#include "protocol/debug.h"
#include "protocol/slot_create.h"
#include "protocol/slot_remove.h"
#include "../core/utils.h"
#include "../core/parser.h"

using namespace laplace;
using namespace core;
using namespace engine;
using namespace protocol;
using namespace std;

factory::factory() { }
factory::~factory() { }

auto factory::parse(string_view command) -> vbyte
{
    return parse_native(
        { ids::table, ids::_native_count },
        command);
}

auto factory::print(cref_vbyte seq) -> string
{
    return print_native(
        { ids::table, ids::_native_count },
        seq);
}

auto factory::decode(cref_vbyte seq) -> ptr_impact
{
    return decode_native(seq);
}

auto factory::parse_native(cref_vstring table, string_view command) -> vbyte
{
    auto in = parser::wrap(command);

    u8string u8_id;

    if (!in.parse(" %a ", &u8_id))
    {
        return { };
    }

    uint16_t id = ids::undefined;
    string s_id = to_string(u8_id);

    for (size_t i = 0; i < table.size(); i++)
    {
        if (table[i] == s_id)
        {
            id = static_cast<uint16_t>(i);
            break;
        }
    }

    if (id == ids::undefined)
    {
        cerr << __FUNCTION__ << ": Unknown command." << endl;
        return { };
    }

    vbyte seq(2);
    wr<uint16_t>(seq, 0, id);

    uint64_t x;

    while (in.parse(" %X ", &x))
    {
        if (x != (x & 0xff))
        {
            cerr << __FUNCTION__ << ": Invalid syntax." << endl;
            return { };
        }

        seq.emplace_back(static_cast<uint8_t>(x));
    }

    if (!in.is_eof())
    {
        cerr << __FUNCTION__ << ": Invalid syntax." << endl;
        return { };
    }

    return seq;
}

auto factory::print_native(cref_vstring table, cref_vbyte seq) -> string
{
    string s;

    if (seq.size() >= 2)
    {
        auto id = impact::get_id_unsafe(seq);

        if (id < table.size())
        {
            s.append(table[id]);

            for (size_t i = 2; i < seq.size(); i++)
            {
                s.append(to_string(" %02x", seq[i]));
            }
        }
    }

    return s;
}

auto factory::decode_native(cref_vbyte seq) -> ptr_impact
{
    if (public_key::scan(seq))      return make<public_key>(seq);
    if (request_events::scan(seq))  return make<request_events>(seq);
    if (server_launch::scan(seq))   return make<server_launch>(seq);
    if (server_action::scan(seq))   return make<server_action>(seq);
    if (server_pause::scan(seq))    return make<server_pause>(seq);
    if (server_clock::scan(seq))    return make<server_clock>(seq);
    if (server_seed::scan(seq))     return make<server_seed>(seq);
    if (server_quit::scan(seq))     return make<server_quit>(seq);
    if (client_ping::scan(seq))     return make<client_ping>(seq);
    if (client_enter::scan(seq))    return make<client_enter>(seq);
    if (client_leave::scan(seq))    return make<client_leave>(seq);
    if (client_ready::scan(seq))    return make<client_ready>(seq);
    if (debug::scan(seq))           return make<debug>(seq);
    if (slot_create::scan(seq))     return make<slot_create>(seq);
    if (slot_remove::scan(seq))     return make<slot_remove>(seq);

    return ptr_impact();
}
