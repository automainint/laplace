#pragma once

namespace laplace::render
{
    template <typename vecval>
    inline track<vecval>::track()
    {
        this->m_is_uniform = false;
        this->m_delta = timeval(1);
    }

    template <typename vecval>
    inline track<vecval>::~track() { }

    template <typename vecval>
    inline auto track<vecval>::op(cref_piece p, timeval t) -> bool
    {
        return p.time < t;
    }

    template <typename vecval>
    inline auto track<vecval>::solve(timeval t) const -> vecval
    {
        auto clamp_time = [this](timeval t) -> timeval {
            return
                m_pieces.empty() ? timeval(0) :
                t < m_pieces[0].time ? m_pieces[0].time :
                t > m_pieces.back().time ? m_pieces.back().time
                : t;
        };

        auto solve_uniform = [this](timeval t) -> vecval {
            assert(this->m_delta != 0);

            auto index = static_cast<size_t>(floor(t / this->m_delta));

            return this->solve(index, t);
        };

        auto solve_direct = [this](timeval t) -> vecval {
            auto i = std::lower_bound(
                this->m_pieces.begin() + 1,
                this->m_pieces.end(),
                t, op);

            auto index = (i - this->m_pieces.begin()) - 1;

            return this->solve(index, t);
        };

        t = clamp_time(t);

        return
            this->m_is_uniform ? solve_uniform(t)
            : solve_direct(t);
    }

    template <typename vecval>
    inline void track<vecval>::clear()
    {
        this->m_pieces.clear();
    }

    template <typename vecval>
    inline void track<vecval>::add(track<vecval>::cref_spline spline, timeval time, vecval begin, vecval end)
    {
        this->m_pieces.push_back({
            spline,
            time,
            begin,
            end
            });
    }

    template <typename vecval>
    inline void track<vecval>::adjust()
    {
        auto comp = [](cref_piece a, cref_piece b) -> bool {
            return a.time < b.time;
        };

        this->m_is_uniform = false;
        this->m_delta = timeval(0);

        std::sort(this->m_pieces.begin(), this->m_pieces.end(), comp);
    }

    template <typename vecval>
    inline auto track<vecval>::solve(size_t i, timeval time) const -> vecval
    {
        realmax_t t = 0;

        if (i < this->m_pieces.size() - 1)
        {
            auto t0 = m_pieces[i].time;
            auto t1 = m_pieces[i + 1].time;

            t = static_cast<realmax_t>(time - t0) / (t1 - t0);
        }

        realmax_t curve_t = 0;

        if (this->m_pieces[i].spline.is_linear())
        {
            curve_t = t;
        }
        else
        {
            realmax_t param = 0.5;
            realmax_t delta = 0.25;

            for (size_t i = 0; i < solving_depth; i++)
            {
                auto p = this->m_pieces[i].spline.solve(param);

                curve_t = p[1];

                if (p[0] + numeric_limits<realmax_t>::epsilon() < t)
                {
                    param += delta;
                }
                else if (p[0] - numeric_limits<realmax_t>::epsilon() > t)
                {
                    param -= delta;
                }
                else
                {
                    break;
                }

                delta /= 2;
            }
        }

        return lerp(
            this->m_pieces[i].begin,
            this->m_pieces[i].end,
            this->m_pieces[i].spline.solve(curve_t)[1]
        );
    }

    template <typename vecval>
    inline auto track<vecval>::approximate(timeval delta) const -> track<vecval>
    {
        track<vecval> result;
        result.m_is_uniform = true;
        result.m_delta = delta;

        if (m_pieces.size() > 1)
        {
            vpiece v;

            for (size_t i = 1; i < m_pieces.size(); i++)
            {
                auto &curve = m_pieces[i - 1];

                if (curve.spline.is_linear())
                {
                    v.emplace_back(curve);
                }
                else
                {
                    timeval duration = m_pieces[i].time - curve.time;
                    vecval v0 = curve.begin;

                    double dt = max(1.0, static_cast<double>(delta) / (4 * duration));
                    double t0 = 0;
                    double t1 = curve.spline.solve(0)[1];

                    for (double t = 0; t < 1; t += dt)
                    {
                        piece p;

                        t0 = t1;
                        t1 = curve.spline.solve(t + dt)[1];

                        p.time = static_cast<timeval>(floor(curve.time + duration * t0 + 0.5));
                        p.begin = v0;
                        p.end = math::lerp(curve.begin, curve.end, t1);

                        v0 = p.end;

                        v.emplace_back(p);
                    }
                }
            }

            v.emplace_back(piece {
                splineval::flat(),
                m_pieces.back().time,
                m_pieces.back().begin,
                m_pieces.back().begin
                });

            for (size_t i = 1; i < v.size();)
            {
                if (v[i - 1].time == v[i].time)
                {
                    v.erase(v.begin() + (i - 1));
                }
                else
                {
                    i++;
                }
            }

            if (v.size() > 1)
            {
                auto i = v.begin();
                auto j0 = i;
                auto begin = v[0].begin;
                auto end = begin;

                for (timeval time = delta; time < v.back().time; time += delta)
                {
                    while (i != v.end() && i->time < time)
                    {
                        i++;
                    }

                    if (i != v.begin() && i != v.end())
                    {
                        auto i0 = i - 1;
                        realmax_t t = static_cast<realmax_t>(time - i0->time) / (i->time - i0->time);

                        end = math::lerp(i0->begin, i0->end, i0->spline.solve(t)[1]);
                    }

                    if (j0 + 1 == i && j0->end != i->begin)
                    {
                        begin = i->begin;
                    }

                    result.m_pieces.emplace_back(piece {
                        splineval::flat(),
                        time - delta,
                        begin,
                        end
                        });

                    begin = end;
                    j0 = i;
                }
            }
        }

        return result;
    }

    template <typename vecval>
    inline void track<vecval>::set_count(size_t count)
    {
        this->m_pieces.resize(count);
    }

    template <typename vecval>
    inline auto track<vecval>::get_count() const -> size_t
    {
        return this->m_pieces.size();
    }

    template <typename vecval>
    inline void track<vecval>::set_piece(size_t index, track<vecval>::cref_piece p)
    {
        this->m_pieces[index] = p;
    }

    template <typename vecval>
    inline auto track<vecval>::get_piece(size_t index) -> track<vecval>::ref_piece
    {
        return this->m_pieces[index];
    }

    template <typename vecval>
    inline auto track<vecval>::get_piece(size_t index) const -> track<vecval>::cref_piece
    {
        return this->m_pieces[index];
    }

    template <typename vecval>
    inline auto track<vecval>::operator ()(timeval t) const -> vecval
    {
        return this->solve(t);
    }

    template <typename vecval>
    inline auto track<vecval>::operator [](size_t index) -> track<vecval>::ref_piece
    {
        return this->get_piece(index);
    }

    template <typename vecval>
    inline auto track<vecval>::operator [](size_t index) const -> track<vecval>::cref_piece
    {
        return this->get_piece(index);
    }
}
