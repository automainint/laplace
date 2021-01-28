#include "joint.h"
#include "utils.h"
#include <thread>
#include <algorithm>

using namespace laplace;
using namespace network;
using namespace std;

joint::joint(string_view address, uint16_t port)
{
    auto size = min(address.size(), sizeof m_address - 1);
    copy(address.begin(), address.begin() + size, m_address);
    m_address[size] = '\0';

    m_port = port;

    m_socket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_socket == INVALID_SOCKET)
    {
        cerr << "TCP: " << __FUNCTION__ << " - socket failed." << endl;
        done();
        return;
    }

    if (!set_mode(m_socket, async))
    {
        cerr << "TCP: " << __FUNCTION__ << " - ioctlsocket failed." << endl;
        done();
    }
}

joint::~joint() { }

void joint::on_connect(joint::event_connect fn)
{
    m_on_connect = fn;
}

void joint::tick()
{
    if (!is_done())
    {
        if (m_socket == INVALID_SOCKET)
        { 
            done();
        }
        else if (m_is_ready)
        {
            if (m_on_connect)
            {
                m_on_connect(*this);
            }
        }
        else
        {
            sockaddr_in name;
            name.sin_family = AF_INET;
            name.sin_port = ::htons(m_port);

            if (::inet_pton(AF_INET, m_address, &name.sin_addr.s_addr) != 1)
            {
                cerr << "TCP: " << __FUNCTION__ << " - inet_pton failed." << endl;
                done();
            }
            else if (
                ::connect(m_socket, reinterpret_cast<const sockaddr *>(&name), sizeof name) != SOCKET_ERROR
                || socket_error() == socket_isconn())
            {
                m_is_ready = true;

                if (m_on_connect)
                {
                    m_on_connect(*this);
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
