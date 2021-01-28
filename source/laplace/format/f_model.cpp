#include "model.h"
#include "binary.h"
#include "text.h"
#include "buffer.h"
#include <iostream>

using namespace laplace;
using namespace core;
using namespace format;
using namespace graphics;
using namespace std;

auto model::extract(const_pack_type pack) -> model::data_type
{
    return make_shared<render::model>();
}

auto model::package(const_data_type data) -> pack_type
{
    return make_shared<family>();
}

auto model::decode(fn_read read) -> model::data_type
{
    data_type result;

    if (read)
    {
        buffer buf(read);

        auto buffering = [&buf](size_t n) -> vbyte {
            return buf.read(n);
        };

        auto pack = binary::decode(buffering);

        if (!pack)
        {
            buf.restore();
            pack = text::decode(buffering);
        }

        if (pack)
        {
            result = extract(*pack);
        }
    }

    return result;
}

auto model::encode(fn_write write, const_data_type data, mode encoding_mode) -> bool
{
    bool result = false;

    if (write)
    {
        auto pack = package(data);

        if (pack)
        {
            if (encoding_mode == mode::text)
            {
                result = text::encode(write, *pack);
            }
            else /*if (encoding_mode == mode::binary)*/
            {
                result = binary::encode(write, *pack);
            }
        }
    }

    return result;
}
