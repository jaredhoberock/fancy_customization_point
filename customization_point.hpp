#pragma once

#include "multi_function.hpp"
#include <utility>

// Inspired by
// * Eric Niebler's suggested design for customization points and
// * Cris Cecka's design for customizing customization points

namespace experimental
{


// customization_point is a class for creating Niebler-style customization points
//
// * Derived is the name of the type derived from this customization_point (e.g., begin_t)
// * ADLFunction is a function type whose job is to call the name of the customization point via ADL
// 
//   for example ADLFunction for begin_t could work like this:
//
//   struct adl_begin
//   {
//     template<class... Args>
//     constexpr auto operator()(const begin_t& self, Args&&... args) const ->
//       decltype(begin(std::forward<Args>(args)...))
//     {
//       // call begin via ADL
//       return begin(std::forward<Args>(args)...);
//     }
//   };
//
//   Note that the first parameter of operator() is the same as *this
//
// * FallbackFunctions... is a list of functions to use if the ADL call fails. They are attempted in order.
//   Their signature is the same as the ADLFunction.


template<class Derived, class ADLFunction, class... FallbackFunctions>
class customization_point : private multi_function<ADLFunction, FallbackFunctions...>
{
  private:
    using super_t = multi_function<ADLFunction, FallbackFunctions...>;

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
      // this allows recursive customization_points (it's like a y combinator)
      return super_t::operator()(self(), std::forward<Args>(args)...);
    }
};


template<class Derived, class ADLFunction, class... FallbackFunctions>
constexpr customization_point<Derived,ADLFunction,FallbackFunctions...> make_customization_point(ADLFunction adl_func, FallbackFunctions... fallback_funcs)
{
  return customization_point<Derived,ADLFunction,FallbackFunctions...>(adl_func, fallback_funcs...);
}


} // end experimental

