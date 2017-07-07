#pragma once

#include <utility>

namespace experimental
{


template<class... Implementations>
class multi_function;

template<>
class multi_function<> {};


// a multi_function has several different implementations
// when called, the multi_function selects the first implementation that is not ill-formed
template<class Implementation1, class... Implementations>
class multi_function<Implementation1,Implementations...> : multi_function<Implementations...>
{
  private:
    using super_t = multi_function<Implementations...>;

    mutable Implementation1 impl_;

    template<class... Args>
    static constexpr auto impl(const multi_function& self, Args&&... args) ->
      decltype(self.impl_(std::forward<Args>(args)...))
    {
      return self.impl_(std::forward<Args>(args)...);
    }

    template<class... Args>
    static constexpr auto impl(const super_t& super, Args&&... args) ->
      decltype(super(std::forward<Args>(args)...))
    {
      return super(std::forward<Args>(args)...); 
    }

  public:
    constexpr multi_function() = default;

    constexpr multi_function(Implementation1 impl1, Implementations... impls)
      : multi_function<Implementations>(impls)..., impl_(impl1)
    {}

    template<class... Args>
    constexpr auto operator()(Args&&... args) const ->
      decltype(multi_function::impl(*this, std::forward<Args>(args)...))
    {
      return multi_function::impl(*this, std::forward<Args>(args)...);
    }
};


}

