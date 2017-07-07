#pragma once

#include "multi_function.hpp"
#include "invoke.hpp"
#include <utility>

// Inspired by
// * Eric Niebler's suggested design for customization points and
// * Cris Cecka's design for customizing customization points

namespace experimental
{
namespace detail
{


// this function, used as a template parameter to multi_function below,
// converts a call to a customization point object:
//
//     customization_point(customizer, args...)
//
// into a call to experimental::invoke:
//
//     invoke(customizer, customization_point, args...)
//
// The customization point itself as passed as the first parameter,
// followed by the Customizer used as the first parameter to the customization_point call,
// followed by the rest of the arguments to the customization_point call.
struct invoke_customization_point
{
  template<class CustomizationPoint, class Customizer, class... Args>
  constexpr auto operator()(CustomizationPoint&& self, Customizer&& customizer, Args&&... args) const ->
    decltype(experimental::invoke(std::forward<Customizer>(customizer), std::forward<CustomizationPoint>(self), std::forward<Args>(args)...))
  {
    return experimental::invoke(std::forward<Customizer>(customizer), std::forward<CustomizationPoint>(self), std::forward<Args>(args)...);
  }
};


} // end detail


// customization_point is a class for creating Niebler-style customization points
//
// * Derived is the name of the type derived from this customization_point (e.g., begin_t)
// * ADLFunction is a function type whose job is to call the name of the customization point via ADL
// 
//   For example, ADLFunction for begin_t could work like this:
//
//       struct adl_begin
//       {
//         template<class... Args>
//         constexpr auto operator()(const begin_t& self, Args&&... args) const ->
//           decltype(begin(std::forward<Args>(args)...))
//         {
//           // call begin via ADL
//           return begin(std::forward<Args>(args)...);
//         }
//       };
//
//   Note that the first parameter of operator() is *this.
//
// * FallbackFunctions... is a list of functions to use if the ADL call fails. They are attempted in order.
//   Their signature is the same as the ADLFunction.
//
// When a customization_point is called like a function:
//
//    (*this)(arg1, args...);
//
// it tries the following possible implementations, in order:
//
// 1. Try to call the customization_point by name via ADL:
//    
//        customization-point-name(arg1, args...)
//
// 2. Try to call the customization point via experimental::invoke:
//
//        experimental::invoke(arg1, *this, args...)
//
// 3. Try the fallback functions, in order:
//
//        fallback-function(*this, arg1, args...)
//
// If all of these implementations are ill-formed, then the call is ill-formed.


// XXX eliminate Derived
template<class Derived, class ADLFunction, class... FallbackFunctions>
class customization_point : private multi_function<ADLFunction, detail::invoke_customization_point, FallbackFunctions...>
{
  private:
    using super_t = multi_function<ADLFunction, detail::invoke_customization_point, FallbackFunctions...>;

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

