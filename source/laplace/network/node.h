#pragma once

#include "base.h"

namespace laplace::network
{
    class node final  : public base
    {
    public:
        using ref_node = node &;
        using event_accept = std::function<void(ref_node n)>;

        node(uint16_t port = any_port);
        ~node();

        void on_accept(event_accept fn);

        /*  Listening loop.
         */
        void tick();

        /*  Close the current session and
         *  stop the listening loop.
         */
        void close_node();

        auto is_connected() const -> bool;

        auto get_port() const -> uint16_t;

        /*  Current session remote address.
         */
        auto get_remote_address() const -> std::string;

        /*  Current session remote port.
         */
        auto get_remote_port() const -> uint16_t;

    private:
        event_accept    m_on_accept;
        uint16_t        m_port = 0;
        sockaddr_in     m_remote;
        SOCKET          m_listen;
    };

    using ref_node = node::ref_node;
    using ptr_node = std::shared_ptr<node>;
}
