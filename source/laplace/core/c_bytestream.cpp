#include "bytestream.h"

using namespace laplace;
using namespace std;

bytestreambuf::bytestreambuf(cref_vbyte bytes)
{
    _Init(
        reinterpret_cast<const char *>(bytes.data()),
        bytes.size(),
        2 /* _Constant from basic_stringbuf. */
    );
}

ibytestream::ibytestream(cref_vbyte bytes) : istream(new bytestreambuf(bytes)) { }

ibytestream::~ibytestream()
{
    delete rdbuf();
}
