#include "customization_point.hpp"
#include <algorithm>
#include <numeric>
#include <iostream>
#include <type_traits>

struct adl_for_each
{
  template<class... Args>
  constexpr auto operator()(Args&&... args) const ->
    decltype(for_each(std::forward<Args>(args)...))
  {
    return for_each(std::forward<Args>(args)...);
  }
};

// XXX requires C++17
//constexpr auto for_each = experimental::customization_point(
//  adl_for_each{},
//  [](auto... args)
//  {
//    // call std::for_each
//    return std::for_each(args...);
//  }
//);

struct std_for_each
{
  template<class Iterator, class Function>
  constexpr auto operator()(Iterator first, Iterator last, Function f) const
  {
    return std::for_each(first, last, f);
  }

  template<class ExecutionPolicy, class Iterator, class Function>
  constexpr auto operator()(ExecutionPolicy&&, Iterator first, Iterator last, Function f) const
  {
    return std::for_each(first, last, f);
  }
};

constexpr auto for_each = experimental::customization_point(
  adl_for_each{},
  std_for_each{}
);


namespace mine
{


struct policy {};
constexpr policy seq{};


struct fancy_policy {};
constexpr fancy_policy fancy{};


template<class FancyExecutionPolicy, class Iterator, class Function,
         class = std::enable_if_t<
           std::is_same<fancy_policy, std::decay_t<FancyExecutionPolicy>>::value
         >>
void for_each(FancyExecutionPolicy&& policy, Iterator first, Iterator last, Function f)
{
  std::cout << "mine::for_each(fancy_policy)" << std::endl;

  std::for_each(first, last, f);
}


}

int main()
{
  std::vector<int> vec(10);
  std::iota(vec.begin(), vec.end(), 0);

  // should print the numbers
  ::for_each(vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  // should print mine::for_each, and then the numbers
  ::for_each(mine::fancy, vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  // should print the numbers
  ::for_each(mine::seq, vec.begin(), vec.end(), [](int x)
  {
    std::cout << x << " ";
  });

  std::cout << std::endl;
  std::cout << std::endl;

  std::cout << "OK" << std::endl;
}

