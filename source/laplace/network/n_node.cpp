#include "node.h"
#include "utils.h"
#include <thread>

using namespace laplace;
using namespace network;
using namespace std;

node::node(uint16_t port)
{
    memset(&m_remote, 0, sizeof m_remote);

    m_listen = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_listen == INVALID_SOCKET)
    {
        done();

        cerr << "TCP: " << __FUNCTION__ << " - socket failed." << endl;
        return;
    }

    sockaddr_in name;
    name.sin_family = AF_INET;
    name.sin_port = ::htons(port);

    if (::inet_pton(AF_INET, localhost, &name.sin_addr.s_addr) != 1)
    {
        cerr << "TCP: " << __FUNCTION__ << " - inet_pton failed." << endl;
        closesocket(m_listen);
        m_listen = INVALID_SOCKET;
        done();
        return;
    }

    if (::bind(m_listen, reinterpret_cast<const sockaddr *>(&name), sizeof name) == SOCKET_ERROR)
    {
        cerr << "TCP: " << __FUNCTION__ << " - bind failed." << endl;
        ::closesocket(m_listen);
        m_listen = INVALID_SOCKET;
        done();
        return;
    }

    if (port == any_port)
    {
        int len = sizeof name;

        if (::getsockname(m_listen, reinterpret_cast<sockaddr *>(&name), &len) == SOCKET_ERROR)
        {
            cerr << "TCP: " << __FUNCTION__ << " - getsockname failed." << endl;
            ::closesocket(m_listen);
            m_listen = INVALID_SOCKET;
            done();
            return;
        }

        m_port = ::ntohs(name.sin_port);
    }
    else
    {
        m_port = port;
    }

    if (::listen(m_listen, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "TCP: " << __FUNCTION__ << " - listen failed." << endl;
        closesocket(m_listen);
        m_listen = INVALID_SOCKET;
        done();
        return;
    }

    if (!set_mode(m_listen, async))
    {
        cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
        done();
    }
}

node::~node()
{
    if (m_listen != INVALID_SOCKET)
    {
        closesocket(m_listen);
        m_listen = INVALID_SOCKET;
        done();
    }
}

void node::on_accept(event_accept fn)
{
    m_on_accept = fn;
}

void node::tick()
{
    if (!is_done())
    {
        if (m_socket != INVALID_SOCKET)
        {
            if (m_on_accept)
            {
                m_on_accept(*this);
            }
        }
        else
        {
            int len = sizeof m_remote;
            memset(&m_remote, 0, len);

            m_socket = ::accept(m_listen, reinterpret_cast<sockaddr *>(&m_remote), &len);

            if (m_socket != INVALID_SOCKET)
            {
                if (!set_mode(m_socket, async))
                {
                    cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
                    done();
                }
                else if (m_on_accept)
                {
                    m_on_accept(*this);
                }
            }
            else if (socket_error() != socket_wouldblock())
            {
                cerr << "TCP: " << __FUNCTION__ << " - " << socket_error() << endl;
                done();
            }
        }
    }
}

void node::close_node()
{
    done();
}

auto node::is_connected() const -> bool
{
    return m_socket != INVALID_SOCKET && !is_done();
}

auto node::get_port() const -> uint16_t
{
    return m_port;
}

auto node::get_remote_address() const -> string
{
    return to_string(reinterpret_cast<const sockaddr &>(m_remote));
}

auto node::get_remote_port() const -> uint16_t
{
    return ::ntohs(m_remote.sin_port);
}
