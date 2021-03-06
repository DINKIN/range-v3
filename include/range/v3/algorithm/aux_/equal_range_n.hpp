/// \file
// Range v3 library
//
//  Copyright Eric Niebler 2014-present
//
//  Use, modification and distribution is subject to the
//  Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// Project home: https://github.com/ericniebler/range-v3
//
#ifndef RANGES_V3_ALGORITHM_AUX_EQUAL_RANGE_N_HPP
#define RANGES_V3_ALGORITHM_AUX_EQUAL_RANGE_N_HPP

#include <functional>
#include <range/v3/range_fwd.hpp>
#include <range/v3/range/access.hpp>
#include <range/v3/algorithm/aux_/lower_bound_n.hpp>
#include <range/v3/algorithm/aux_/upper_bound_n.hpp>
#include <range/v3/functional/comparisons.hpp>
#include <range/v3/functional/identity.hpp>
#include <range/v3/functional/invoke.hpp>
#include <range/v3/iterator/operations.hpp>
#include <range/v3/range/concepts.hpp>
#include <range/v3/range/traits.hpp>
#include <range/v3/utility/static_const.hpp>
#include <range/v3/view/subrange.hpp>

namespace ranges
{
    namespace aux
    {
        struct equal_range_n_fn
        {
            template<typename I, typename V, typename R = less, typename P = identity>
            auto operator()(I begin, iter_difference_t<I> dist, V const & val, R pred = R{},
                    P proj = P{}) const ->
                CPP_ret(subrange<I>)(
                    requires ForwardIterator<I> &&
                        IndirectStrictWeakOrder<R, V const *, projected<I, P>>)
            {
                if(0 < dist)
                {
                    do
                    {
                        auto half = dist / 2;
                        auto middle = ranges::next(begin, half);
                        auto && v = *middle;
                        auto && pv = invoke(proj, (decltype(v) &&) v);
                        if(invoke(pred, pv, val))
                        {
                            begin = std::move(++middle);
                            dist -= half + 1;
                        }
                        else if(invoke(pred, val, pv))
                        {
                            dist = half;
                        }
                        else
                        {
                            return {
                                lower_bound_n(std::move(begin), half, val,
                                    std::ref(pred), std::ref(proj)),
                                upper_bound_n(ranges::next(middle), dist - (half + 1),
                                    val, std::ref(pred), std::ref(proj))
                            };
                        }
                    } while(0 != dist);
                }
                return {begin, begin};
            }
        };

        RANGES_INLINE_VARIABLE(equal_range_n_fn, equal_range_n)
    }
} // namespace ranges

#endif // include guard
