#include "native_api.h"

using namespace laplace;
using namespace stem;
using namespace std;

native_api::native_api() { }
native_api::~native_api() { }

auto native_api::perform(size_t id, cref_vbyte args) -> vbyte
{
    return { };
}
