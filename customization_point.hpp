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


// this functor wraps another Function
// when drop_first_arg_and_invoke is called,
// it ignores its first argument and calls the Function with the remaining arguments
template<class Function>
struct drop_first_arg_and_invoke
{
  Function f;

  template<class Arg1, class... Args>
  constexpr auto operator()(Arg1&&, Args&&... args) const ->
    decltype(f(std::forward<Args>(args)...))
  {
    return f(std::forward<Args>(args)...);
  }
};


} // end detail


// customization_point is a class for creating Niebler-style customization points
//
// * Derived is the name of the type (if any) derived from this customization_point (e.g., begin_t)
//   If no type derives from customization_point, use void.
//
// * CallMemberFunction is a function type whose job is to call the name of the customization point as a member function.
//
//   For example, CallMemberFunction for begin_t could work like this:
//
//       struct call_member_function_begin
//       {
//         template<class Arg1, class... Args>
//         constexpr auto operator()(Arg1&& arg1, Args&&... args) const ->
//           decltype(std::forward<Arg1>(arg1).begin(std::forward<Args>(args)...))
//         {
//           // call begin as a free function
//           return std::forward<Arg1>(arg1).begin(std::forward<Args>(args)...);
//         }
//       };
//
// * CallFreeFunction is a function type whose job is to call the name of the customization point as a free function.
// 
//   For example, CallFreeFunction for begin_t could work like this:
//
//       struct call_free_function_begin
//       {
//         template<class... Args>
//         constexpr auto operator()(Args&&... args) const ->
//           decltype(begin(std::forward<Args>(args)...))
//         {
//           // call begin via ADL
//           return begin(std::forward<Args>(args)...);
//         }
//       };
//
// * FallbackFunctions... is a list of functions to use if the first two calls fail. They are attempted in order.
//   Their signature is the same as MemberFunction & FreeFunction
//
// To sum up, when a customization_point is called like a function:
//
//    (*this)(arg1, args...);
//
// it tries the following possible implementations, in order:
//
// 1. Try to call the customization_point by name as a member function:
//
//        arg1.customization-point-name(args...)
//
// 2. Try to call the customization_point by name as a free function:
//    
//        customization-point-name(arg1, args...)
//
// 3. Try to call the customization point via experimental::invoke:
//
//        experimental::invoke(arg1, *this, args...)
//
// 4. Try the fallback functions, in order:
//
//        fallback-function(arg1, args...)
//
// If all of these implementations are ill-formed, then the call to the customization_point is ill-formed.


template<class Derived, class CallMemberFunction, class CallFreeFunction, class... FallbackFunctions>
class customization_point : private multi_function<
  detail::drop_first_arg_and_invoke<CallMemberFunction>,
  detail::drop_first_arg_and_invoke<CallFreeFunction>,
  detail::invoke_customization_point,
  detail::drop_first_arg_and_invoke<FallbackFunctions>...
>
{
  private:
    // in order for invoke_customization_point to receive *this as its first argument,
    // we need to insert *this as the first parameter passed to the call to multi_function::operator()
    // however, this "self" parameter is not included in the signature of the other functions ADLFunction & FallbackFunctions...
    // so, wrap them in a wrapper functor which discards its first parameter before calling the wrapped function
    using super_t = multi_function<
      detail::drop_first_arg_and_invoke<CallMemberFunction>,
      detail::drop_first_arg_and_invoke<CallFreeFunction>,
      detail::invoke_customization_point,
      detail::drop_first_arg_and_invoke<FallbackFunctions>...
    >;

    using derived_type = std::conditional_t<
      std::is_void<Derived>::value,
      customization_point,
      Derived
    >;

    const derived_type& self() const
    {
      return static_cast<const derived_type&>(*this);
    }

  public:
    constexpr customization_point()
      : customization_point(CallMemberFunction{}, CallFreeFunction{}, FallbackFunctions{}...)
    {}

    constexpr customization_point(CallMemberFunction call_member_function, CallFreeFunction call_free_function, FallbackFunctions... fallback_funcs)
      : super_t(detail::drop_first_arg_and_invoke<CallMemberFunction>{call_member_function},
                detail::drop_first_arg_and_invoke<CallFreeFunction>{call_free_function},
                detail::invoke_customization_point{},
                detail::drop_first_arg_and_invoke<FallbackFunctions>{fallback_funcs}...)
    {}

    template<class Arg1, class... Args>
    constexpr auto operator()(Arg1&& arg1, Args&&... args) const ->
      decltype(super_t::operator()(self(), std::forward<Arg1>(arg1), std::forward<Args>(args)...))
    {
      return super_t::operator()(self(), std::forward<Arg1>(arg1), std::forward<Args>(args)...);
    }
};


template<class CallMemberFunction, class CallFreeFunction, class... FallbackFunctions>
constexpr customization_point<void,CallMemberFunction,CallFreeFunction,FallbackFunctions...>
  make_customization_point(CallMemberFunction call_member_func, CallFreeFunction call_free_func, FallbackFunctions... fallback_funcs)
{
  return customization_point<void,CallMemberFunction,CallFreeFunction,FallbackFunctions...>(call_member_func, call_free_func, fallback_funcs...);
}


} // end experimental

