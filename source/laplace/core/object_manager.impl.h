#pragma once

template <typename type>
inline auto laplace::core::object_manager::load(std::wstring_view file_name) -> std::shared_ptr<type>
{
    std::shared_ptr<type> result;

    auto p = load_raw(file_name);

    if (p && p->get_type_id() == type::type_id())
    {
        result = std::reinterpret_pointer_cast<type>(p);
    }

    return result;
}
