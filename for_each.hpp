#pragma once

#include "customization_point.hpp"
#include "invoke.hpp"
#include <algorithm>

namespace experimental
{
namespace detail
{

struct adl_for_each
{
  template<class ForEach, class... Args>
  constexpr auto operator()(ForEach&&, Args&&... args) const ->
    decltype(for_each(std::forward<Args>(args)...))
  {
    return for_each(std::forward<Args>(args)...);
  }
};

struct std_for_each
{
  template<class ForEach, class Iterator, class Function>
  constexpr Function operator()(ForEach&&, Iterator first, Iterator last, Function f) const
  {
    return std::for_each(first, last, f);
  }

  template<class ForEach, class ExecutionPolicy, class Iterator, class Function>
  constexpr void operator()(ForEach&&, ExecutionPolicy&&, Iterator first, Iterator last, Function f) const
  {
    std::for_each(first, last, f);
  }
};


} // end detail

class for_each_t : public experimental::customization_point<for_each_t, detail::adl_for_each, detail::std_for_each> {};

constexpr for_each_t for_each{};

} // end experimental

