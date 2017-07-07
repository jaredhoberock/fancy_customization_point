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

// XXX we should move this into customization_point so we don't have to superfluously define it for each derivation of customization_point
//     the problem is that experimental::invoke is itself a customization_point, and experimental::invoke is used below
struct invoke_customization_point
{
  template<class CustomizationPoint, class Customizer, class... Args>
  constexpr auto operator()(CustomizationPoint&& self, Customizer&& customizer, Args&&... args) const ->
    decltype(experimental::invoke(std::forward<Customizer>(customizer), std::forward<CustomizationPoint>(self), std::forward<Args>(args)...))
  {
    return experimental::invoke(std::forward<Customizer>(customizer), std::forward<CustomizationPoint>(self), std::forward<Args>(args)...);
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

class for_each_t : public experimental::customization_point<for_each_t, detail::adl_for_each, detail::invoke_customization_point, detail::std_for_each> {};

constexpr for_each_t for_each{};

} // end experimental

