#pragma once

#include "customization_point.hpp"
#include <utility>


namespace experimental
{
namespace detail
{


struct adl_invoke_with_customizer
{
  template<class Invoker, class Customizer, class... Args>
  constexpr auto operator()(Invoker&&, Customizer&& customizer, Args&&... args) const ->
    decltype(invoke(std::forward<Customizer>(customizer), std::forward<Args>(args)...))
  {
    return invoke(std::forward<Customizer>(customizer), std::forward<Args>(args)...);
  }
};


struct invoke_function_directly
{
  template<class Invoker, class Function, class... Args>
  constexpr auto operator()(Invoker&&, Function&& f, Args&&... args) const ->
    decltype(std::forward<Function>(f)(std::forward<Args>(args)...))
  {
    return std::forward<Function>(f)(std::forward<Args>(args)...);
  }
};


struct drop_customizer_and_invoke_with_self
{
  template<class Invoker, class Customizer, class... Args>
  constexpr auto operator()(Invoker&& self, Customizer&&, Args&&... args) const ->
    decltype(std::forward<Invoker>(self)(std::forward<Args>(args)...))
  {
    return std::forward<Invoker>(self)(std::forward<Args>(args)...);
  }
};


} // end detail

// invoke(arg1, args...) has three cases:
// 1. Assume arg1 is the customizer. Try calling invoke(arg1, args...) via ADL
// 2. Assume arg1 is a function. Try calling arg1(args...) like a function
// 3. Drop the first argument (presumably a customizer type which didn't happen to provide a customization) and recurse to experimental::invoke(args...)

class invoke_t : public customization_point<invoke_t, detail::adl_invoke_with_customizer, detail::invoke_function_directly, detail::drop_customizer_and_invoke_with_self> {};


constexpr invoke_t invoke{};
  

} // end experimental

