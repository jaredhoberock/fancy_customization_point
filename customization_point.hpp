#pragma once

#include "multi_function.hpp"
#include <utility>

// Inspired by
// * Eric Niebler's suggested design for customization points and
// * Cris Cecka's design for customizing customization points

namespace experimental
{


template<class Derived, class ADLImplementation, class... FallbackImplementations>
class customization_point : private multi_function<ADLImplementation, FallbackImplementations...>
{
  private:
    using super_t = multi_function<ADLImplementation, FallbackImplementations...>;

    const Derived& self() const
    {
      return static_cast<const Derived&>(*this);
    }

  public:
    using super_t::super_t;

    template<class... Args>
    constexpr auto operator()(Args&&... args) const ->
      decltype(super_t::operator()(self(), std::forward<Args>(args)...))
    {
      // when we are called like a function, we insert ourself as the first parameter to the call to the multi_function
      return super_t::operator()(self(), std::forward<Args>(args)...);
    }
};


template<class Derived, class ADLImplementation, class... FallbackImplementations>
constexpr customization_point<Derived,ADLImplementation,FallbackImplementations...> make_customization_point(ADLImplementation adl_impl, FallbackImplementations... fallback_impls)
{
  return customization_point<Derived,ADLImplementation,FallbackImplementations...>(adl_impl, fallback_impls...);
}


} // end experimental

