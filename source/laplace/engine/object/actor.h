#pragma once

#include "../entity.h"
#include "../access/entity.h"

namespace laplace::engine::object
{
    class actor : public entity
    {
    public:
        ~actor() override = default;
        
        static void set_local(access::entity en, bool is_local);
        static auto is_local(access::entity en) -> bool;

    protected:
        actor(dummy_tag);
        actor(proto_tag);

        static size_t n_is_local;

    private:
        static actor m_proto;
    };
}
