#include "player.h"
#include "sets.h"
#include <laplace/core/utils.h>

using namespace quadwar_app;
using namespace engine;
using namespace std;

player player::m_proto(entity::proto);

player::player(proto_tag) : object::actor(proto)
{
}

player::player(bool is_local) : object::actor(dummy)
{
    *this = m_proto;

    init(n_is_local, is_local);
}

void player::set_name(access::entity en, u8string_view name)
{
    en.modify(sets::player_name, pack_to_bytes(name));
}

auto player::get_name(access::entity en) -> u8string
{
    const auto data = en.request(sets::player_name);

    u8string result(data.size(), ' ');
    memcpy(result.data(), data.data(), data.size());

    return result;
}

auto player::do_request(size_t id, cref_vbyte args) const -> vbyte
{
    if (id == sets::player_name)
    {
        return pack_to_bytes(u8string_view { m_name });
    }

    return { };
}

void player::do_modify(size_t id, cref_vbyte args)
{
    if (id == sets::player_name)
    {
        m_name.resize(args.size(), ' ');
        memcpy(m_name.data(), args.data(), args.size());
    }
}
