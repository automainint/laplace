#pragma once

#include "defs.h"

namespace laplace::embedded
{
    auto scan(std::wstring_view file_name) -> bool;
    auto exists(std::wstring_view file_name) -> bool;
    auto open(std::wstring_view file_name) -> cref_vbyte;
}
