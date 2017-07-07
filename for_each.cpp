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
  std::cout << "mine::invoke(fancy_policy, " << typeid(CustomizationPoint).name() << ")" << std::endl;

  // call the customization point with seq
  return customization_point(seq, std::forward<Args>(args)...);
}


}

int main()
{
  std::vector<int> vec(10);
  std::iota(vec.begin(), vec.end(), 0);

  // should print the numbers
  experimental::for_each(vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  // should print something like mine::invoke(fancy_policy, for_each_t), 
  experimental::for_each(mine::fancy, vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  // should print the numbers
  experimental::for_each(mine::seq, vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "OK" << std::endl;
}

