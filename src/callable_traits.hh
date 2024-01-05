#pragma once

#include <tuple>
#include <type_traits>

// This header provides support for determining features of callable objects. It
// is inspired by the function_traits of the LLVM project.

namespace pfc {

  // struct template callable_traits<T> gives compile-time information about a
  // callable type F.
  //   callable_traits<F>::arity is the number of arguments for the call.
  //   callable_traits<F>::result_t is the return type of the call.
  //   callable_traits<F>::arg_t<I> is the type of the I'th argument.

  // General case. Should only be found for a type C that is callable.
  template <typename C, bool is_class = std::is_class_v<C>>
  struct callable_traits : callable_traits<decltype(&C::operator())> {};

  // Specialization for a const-qualified pointer-to-member-function
  template <typename C, typename R, typename... Args>
  struct callable_traits<R (C::*)(Args...) const, false> {
    static std::size_t const arity = sizeof...(Args);
    using result_t = R;
    template <size_t Index>
    using arg_t = typename std::tuple_element<Index, std::tuple<Args...>>::type;
    using all_args_t = std::tuple<Args...>;
  };

  // Specialization for an unqualified pointer-to-member-function
  template <typename C, typename R, typename... Args>
  struct callable_traits<R (C::*)(Args...), false>
    : callable_traits<R (C::*)(Args...) const> {};

  // Specialization for a pointer-to-function.
  template <typename R, typename... Args>
  struct callable_traits<R (*)(Args...), false> {
    static std::size_t const arity = sizeof...(Args);
    using result_t = R;
    template <size_t I>
    using arg_t = typename std::tuple_element<I, std::tuple<Args...>>::type;
    using all_args_t = std::tuple<Args...>;
  };

  // Specialization for a const pointer to function.
  template <typename R, typename... Args>
  struct callable_traits<R (*const)(Args...), false>
    : callable_traits<R (*)(Args...)> {};

  // Specialization for a referene to a function.
  template <typename R, typename... Args>
  struct callable_traits<R (&)(Args...), false>
    : callable_traits<R (*)(Args...)> {};

  // Specialization for a function type.
  template <typename R, typename... Args>
  struct callable_traits<R(Args...), false> : callable_traits<R (*)(Args...)> {
  };

}