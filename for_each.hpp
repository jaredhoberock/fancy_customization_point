#pragma once

#include "customization_point.hpp"
#include "invoke.hpp"
#include <algorithm>

namespace experimental
{


template<class Iterator, class Function>
Function default_for_each(Iterator first, Iterator last, Function f)
{
  return std::for_each(first, last, f);
}

template<class ExecutionPolicy, class Iterator, class Function>
void default_for_each(ExecutionPolicy&&, Iterator first, Iterator last, Function f)
{
  std::for_each(first, last, f);
}


namespace detail
{

struct adl_for_each
{
  template<class... Args>
  constexpr auto operator()(Args&&... args) const ->
    decltype(for_each(std::forward<Args>(args)...))
  {
    return for_each(std::forward<Args>(args)...);
  }
};

struct default_for_each
{
  template<class... Args>
  constexpr auto operator()(Args&&... args) const ->
    decltype(experimental::default_for_each(std::forward<Args>(args)...))
  {
    return experimental::default_for_each(std::forward<Args>(args)...);
  }
};


} // end detail

class for_each_t : public experimental::customization_point<for_each_t, detail::adl_for_each, detail::default_for_each> {};

constexpr for_each_t for_each{};

} // end experimental

