#include "image.h"
#include "binary.h"
#include "text.h"
#include "buffer.h"

using namespace laplace;
using namespace core;
using namespace format;
using namespace std;

auto image::extract(const_pack_type pack) -> image::data_type
{
    auto result = make_shared<graphics::image>();

    size_t width    = pack["width"].get_uint();
    size_t height   = pack["height"].get_uint();
    size_t depth    = pack["depth"].get_uint();

    result->set_size(width, height, depth);

    auto pixels = pack["pixels"].get_bytes();

    if (pixels.size_bytes() == result->get_size_bytes())
    {
        copy(pixels.begin(), pixels.end(), result->data());
    }
    else
    {
        result.reset();
    }

    return result;
}

auto image::package(const_data_type data) -> pack_type
{
    auto result = make_shared<family>();

    (*result)["width"] = data.get_width();
    (*result)["height"] = data.get_height();
    (*result)["depth"] = data.get_depth();

    (*result)["pixels"] = cref_vbyte(data.get_data(), data.get_data() + data.get_size_bytes());

    return result;
}

auto image::decode(fn_read read) -> image::data_type
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

auto image::encode(fn_write write, const_data_type data, mode encoding_mode) -> bool
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
