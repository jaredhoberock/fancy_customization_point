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


class invoke_t : public customization_point<invoke_t, detail::adl_invoke_with_customizer, detail::drop_customizer_and_invoke_with_self> {};


constexpr invoke_t invoke{};
  

} // end experimental

