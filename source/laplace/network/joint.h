#pragma once

#include "base.h"

namespace laplace::network
{
    class joint final : public base
    {
    public:
        using ref_joint = joint &;
        using event_connect = std::function<void(ref_joint j)>;

        joint(std::string_view address, uint16_t port);
        ~joint();

        void on_connect(event_connect fn);

        void tick();

    private:
        event_connect   m_on_connect;
        char            m_address[address_size] = "";
        uint16_t        m_port                  = 0;
        bool            m_is_ready              = false;
    };

    using ref_joint = joint::ref_joint;
    using ptr_joint = std::shared_ptr<joint>;
}
