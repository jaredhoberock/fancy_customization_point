#include <iostream>
#include <typeinfo>
#include "for_each.hpp"


namespace mine
{


struct policy {};
constexpr policy seq{};


struct fancy_policy {};
constexpr fancy_policy fancy{};


template<class FancyExecutionPolicy,
         class CustomizationPoint, class... Args,
         class = std::enable_if_t<
           std::is_same<fancy_policy, std::decay_t<FancyExecutionPolicy>>::value
         >>
auto invoke(FancyExecutionPolicy&& policy, CustomizationPoint&& customization_point, Args&&... args)
{
  std::cout << "mine::invoke(fancy, " << typeid(CustomizationPoint).name() << ", args...)" << std::endl;

  // call the customization point with seq
  return customization_point(seq, std::forward<Args>(args)...);

  // Because customization_points are callable objects, they can be invoked via experimental::invoke()
  // Therefore, the above call ends up being equivalent to:
  //
  //     return experimental::invoke(customization_point, seq, std::forward<Args>(args)...);
  //
  // Moreover, because there is no specialization of invoke() for seq, the above call is also equivalent to:
  //
  //     return experimental::invoke(seq, customization_point, std::forward<Args>(args)...);
  //
  // This is because experimental::invoke will not find a specialization of invoke(seq, customization_point, args...).
  // Therefore, experimental::invoke will attempt the following call, which will succeed:
  //
  //     return customization_point(std::forward<Args>(args)...);
}


}

int main()
{
  std::vector<int> vec(10);
  std::iota(vec.begin(), vec.end(), 0);

  // should just print the numbers
  experimental::for_each(vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  // should print something fancy like mine::invoke(fancy, for_each_t), 
  // and then the numbers
  experimental::for_each(mine::fancy, vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  // should just print the numbers
  experimental::for_each(mine::seq, vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "OK" << std::endl;
}

