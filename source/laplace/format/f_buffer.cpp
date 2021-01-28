#include "buffer.h"
#include <algorithm>

using namespace laplace::format;
using namespace std;

buffer::buffer(fn_read read)
{
    m_read = read;
    m_offset = 0;
}

buffer::~buffer() { }

auto buffer::read(size_t count) -> vbyte
{
    auto piece = m_data.size() - m_offset;

    if (count > piece)
    {
        auto v = m_read(count - piece);
        m_data.insert(m_data.end(), v.begin(), v.end());
    }

    auto p = m_data.begin() + m_offset;
    m_offset += m_data.size();

    return vbyte(p, p + count);
}

void buffer::keep()
{
    m_offset = 0;
    m_data.clear();
}

void buffer::restore()
{
    m_offset = 0;
}
