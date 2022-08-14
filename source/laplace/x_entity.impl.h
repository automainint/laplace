#ifndef LAPLACE_X_ENTITY_IMPL_H
#define LAPLACE_X_ENTITY_IMPL_H

namespace laplace {
  /*  TODO
   *  Constexpr
   */

  inline auto entity::error() const noexcept -> bool {
    return m_is_error;
  }

  inline auto entity::size() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_fields.size(); },
                            index_undefined);
  }

  inline auto entity::point(ptrdiff_t value_id) const noexcept
      -> value_point {
    return { .id = id(), .index = index_of(value_id) };
  }

  inline auto entity::index_of(ptrdiff_t id) const noexcept
      -> ptrdiff_t {
    return _bind<ptrdiff_t>(
        [&]() {
          auto i = lower_bound(
              m_fields.begin(), m_fields.end(), id,
              [](auto const &a, auto const &b) { return a.id < b; });
          return i != m_fields.end() && i->id == id
                     ? i - m_fields.begin()
                     : index_undefined;
        },
        index_undefined);
  }

  inline auto entity::id() const noexcept -> ptrdiff_t {
    return _bind<ptrdiff_t>([&]() { return m_id; }, id_undefined);
  }

  inline auto entity::setup(std::vector<field> fields) const noexcept
      -> entity {
    return _bind([&]() {
      auto e = entity { *this };

      for (auto i = fields.begin(); i != fields.end(); i++)
        for (auto j = fields.begin(); j != i; j++)
          if (i->id == j->id)
            return _error();

      for (auto &f : fields) {
        auto const id = f.id;
        if (id < 0)
          return _error();
        auto i = lower_bound(
            e.m_fields.begin(), e.m_fields.end(), id,
            [](auto const &a, auto const &b) { return a.id < b; });
        if (i != e.m_fields.end() && i->id == id)
          i->id = id;
        else
          e.m_fields.emplace(i, f);
      }

      return e;
    });
  }

  inline auto entity::set_id(ptrdiff_t id) const noexcept -> entity {
    return _bind([&]() {
      auto e = entity { *this };
      e.m_id = id;
      return e;
    });
  }

  inline auto entity::_error() const noexcept -> entity {
    auto e       = entity { *this };
    e.m_is_error = true;
    return e;
  }

  inline auto entity::_bind(std::function<entity()> f) const noexcept
      -> entity {
    if (error())
      return *this;
    return f();
  }

  template <typename type_>
  inline auto entity::_bind(std::function<type_()> f,
                            type_ def) const noexcept -> type_ {
    if (error())
      return def;
    return f();
  }
}

#endif
