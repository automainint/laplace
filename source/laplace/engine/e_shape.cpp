#include "shape.h"
#include <cassert>

using namespace laplace::engine;
using namespace std;

shape::shape() { }
shape::~shape() { }

static void build(ref_octree tree, int64_t delta, cref_box bounds)
{
    tree.bounds = bounds;

    int64_t d[3] = {
        bounds.max.x - bounds.min.x,
        bounds.max.y - bounds.min.y,
        bounds.max.z - bounds.min.z
    };

    bool f[3] = {
        d[0] > delta,
        d[1] > delta,
        d[2] > delta
    };

    int64_t s[3] = {
        f[0] ? d[0] / 2 : d[0],
        f[1] ? d[1] / 2 : d[1],
        f[2] ? d[2] / 2 : d[2]
    };

    int n = (f[0] ? 2 : 1) * (f[1] ? 2 : 1) * (f[2] ? 2 : 1);

    if (n == 1)
    {
        tree.childs = vtriangle();
    }
    else
    {
        tree.childs = voctree(n);
        auto &v = get<0>(tree.childs);

        size_t index = 0;

        for (size_t i = 0; i < 2; i++)
        {
            if (i == 1 && !f[0]) continue;

            for (size_t j = 0; j < 2; j++)
            {
                if (j == 1 && !f[1]) continue;

                for (size_t k = 0; k < 2; k++)
                {
                    if (k == 1 && !f[2]) continue;

                    assert(index < v.size());

                    build(v[index], delta, box {
                        vec3i { 
                            i == 0 ? bounds.min.x : bounds.min.x + s[0],
                            j == 0 ? bounds.min.y : bounds.min.y + s[1],
                            k == 0 ? bounds.min.z : bounds.min.z + s[2]
                        },
                        vec3i { 
                            i == 0 && f[0] ? bounds.min.x + s[0] : bounds.max.x,
                            j == 0 && f[1] ? bounds.min.y + s[1] : bounds.max.y,
                            k == 0 && f[2] ? bounds.min.z + s[2] : bounds.max.z
                        }
                    });

                    index++;
                }
            }
        }
    }
}

static void put(ref_octree tree, cref_triangle tr)
{
    if (intersects(tree.bounds, tr))
    {
        if (tree.childs.index() == 0)
        {
            for (size_t i = 0; i < get<0>(tree.childs).size(); i++)
            {
                put(get<0>(tree.childs)[i], tr);
            }
        }
        else
        {
            assert(tree.childs.index() == 1);
            get<1>(tree.childs).emplace_back(tr);
        }
    }
}

static void cleanup(ref_octree tree)
{
    auto is_empty = [](ref_octree tree) -> bool {
        if (tree.childs.index() == 0)
        {
            return get<0>(tree.childs).empty();
        }

        assert(tree.childs.index() == 1);
        return get<1>(tree.childs).empty();
    };

    if (tree.childs.index() == 0)
    {
        auto &v = get<0>(tree.childs);

        for (size_t i = 0; i < v.size();)
        {
            cleanup(v[i]);

            if (is_empty(v[i]))
            {
                v.erase(v.begin() + i);
            }
            else
            {
                i++;
            }
        }
    }
}

void shape::granulate(cref_vtriangle triangles, int64_t delta)
{
    build(reset_octree(), delta, bounds_of(triangles));

    for (size_t i = 0; i < triangles.size(); i++)
    {
        put(get<n_octree>(m_data), triangles[i]);
    }

    cleanup(get<n_octree>(m_data));
}

auto shape::contains(cref_vec3i point) const -> bool
{
    return
        is_box()        ? ::contains(get<n_box>(m_data), point) :
        is_cylinder()   ? ::contains(get<n_cylinder>(m_data), point) :
        is_sphere()     ? ::contains(get<n_sphere>(m_data), point) :
        is_octree()     ? ::contains(get<n_octree>(m_data), point)
                        : false;
}

auto shape::get_bounds() const -> box
{
    return
        is_box()        ? get<n_box>(m_data) :
        is_cylinder()   ? bounds_of(get<n_cylinder>(m_data)) :
        is_sphere()     ? bounds_of(get<n_sphere>(m_data)) :
        is_octree()     ? get<n_octree>(m_data).bounds
                        : box { };
}

void shape::set_empty()
{
    m_data = monostate();
}

void shape::set_box(cref_box value)
{
    m_data = value;
}

void shape::set_cylinder(cref_cylinder value)
{
    m_data = value;
}

void shape::set_sphere(cref_sphere value)
{
    m_data = value;
}

void shape::set_octree(cref_octree value)
{
    m_data = value;
}

auto shape::reset_box() -> ref_box
{
    if (m_data.index() != n_box)
    {
        m_data = box();
    }

    return get<n_box>(m_data);
}

auto shape::reset_cylinder() -> ref_cylinder
{
    if (m_data.index() != n_cylinder)
    {
        m_data = cylinder();
    }

    return get<n_cylinder>(m_data);
}

auto shape::reset_sphere() -> ref_sphere
{
    if (m_data.index() != n_sphere)
    {
        m_data = sphere();
    }

    return get<n_sphere>(m_data);
}

auto shape::reset_octree() -> ref_octree
{
    if (m_data.index() != n_octree)
    {
        m_data = octree();
    }

    return get<n_octree>(m_data);
}

auto shape::is_empty() const -> bool
{
    return m_data.index() == 0;
}

auto shape::is_box() const -> bool
{
    return m_data.index() == n_box;
}

auto shape::is_cylinder() const -> bool
{
    return m_data.index() == n_cylinder;
}

auto shape::is_sphere() const -> bool
{
    return m_data.index() == n_sphere;
}

auto shape::is_octree() const -> bool
{
    return m_data.index() == n_octree;
}

auto shape::get_box() const -> cref_box
{
    assert(m_data.index() == n_box);

    if (m_data.index() != n_box)
    {
        static box nil;
        return nil;
    }

    return get<n_box>(m_data);
}

auto shape::get_cylinder() const -> cref_cylinder
{
    assert(m_data.index() == n_cylinder);

    if (m_data.index() != n_cylinder)
    {
        static cylinder nil;
        return nil;
    }

    return get<n_cylinder>(m_data);
}

auto shape::get_sphere() const -> cref_sphere
{
    assert(m_data.index() == n_sphere);

    if (m_data.index() != n_sphere)
    {
        static sphere nil;
        return nil;
    }

    return get<n_sphere>(m_data);
}

auto shape::get_octree() const -> cref_octree
{
    assert(m_data.index() == n_octree);

    if (m_data.index() != n_octree)
    {
        static octree nil;
        return nil;
    }

    return get<n_octree>(m_data);
}

auto laplace::engine::intersects(cref_shape a, cref_shape b) -> bool
{
    if (!a.is_empty() && !b.is_empty())
    {
        if (a.is_box())
        {
            if (b.is_box())         return intersects(a.get_box(), b.get_box());
            if (b.is_cylinder())    return intersects(a.get_box(), b.get_cylinder());
            if (b.is_sphere())      return intersects(a.get_box(), b.get_sphere());
            if (b.is_octree())      return intersects(a.get_box(), b.get_octree());
        }

        if (a.is_cylinder())
        {
            if (b.is_box())         return intersects(b.get_box(), a.get_cylinder());
            if (b.is_cylinder())    return intersects(a.get_cylinder(), b.get_cylinder());
            if (b.is_sphere())      return intersects(a.get_cylinder(), b.get_sphere());
            if (b.is_octree())      return intersects(a.get_cylinder(), b.get_octree());
        }

        if (a.is_sphere())
        {
            if (b.is_box())         return intersects(b.get_box(), a.get_sphere());
            if (b.is_cylinder())    return intersects(b.get_cylinder(), a.get_sphere());
            if (b.is_sphere())      return intersects(a.get_sphere(), b.get_sphere());
            if (b.is_octree())      return intersects(a.get_sphere(), b.get_octree());
        }

        if (a.is_octree())
        {
            if (b.is_box())         return intersects(b.get_box(), a.get_octree());
            if (b.is_cylinder())    return intersects(b.get_cylinder(), a.get_octree());
            if (b.is_sphere())      return intersects(b.get_sphere(), a.get_octree());
            if (b.is_octree())      return intersects(a.get_octree(), b.get_octree());
        }
    }

    return false;
}

auto laplace::engine::intersects(cref_ray a, cref_shape b) -> bool
{
    return
        b.is_box()      ? intersects(a, b.get_box()) :
        b.is_cylinder() ? intersects(a, b.get_cylinder()) :
        b.is_sphere()   ? intersects(a, b.get_sphere()) :
        b.is_octree()   ? intersects(a, b.get_octree())
                        : false;
}

auto laplace::engine::intersection(cref_ray a, cref_shape b) -> int64_t
{
    return
        b.is_box()      ? intersection(a, b.get_box()) :
        b.is_cylinder() ? intersection(a, b.get_cylinder()) :
        b.is_sphere()   ? intersection(a, b.get_sphere()) :
        b.is_octree()   ? intersection(a, b.get_octree())
                        : -safe_limit;
}
