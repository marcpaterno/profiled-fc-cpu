#ifndef PROFILED_FC_CPU_PROTECTED_ENGINE_HH
#define PROFILED_FC_CPU_PROTECTED_ENGINE_HH

#include <ctime>
#include <random>

namespace pfc {

  // This class encapsulates any UnformRandomBitGenerator  for shared used
  // between threads. It uses a mutex to protect calls to the function call
  // operator of the contained engine. It provides the minimial interface to be
  // used as a UniformRandomBitGenerator, and thus can be used with the function
  // call operator of any of the distributions in the C++ Standard Library.
  template <std::uniform_random_bit_generator URBG>
  class protected_engine {
  public:
    using result_type = typename URBG::result_type;

    protected_engine() = default;
    protected_engine(std::time_t t);
    result_type operator()();

    static constexpr result_type min();
    static constexpr result_type max();

  private:
    using engine_type = URBG;

    std::mutex m_;
    engine_type e_;
  };

  template <std::uniform_random_bit_generator URBG>
  protected_engine<URBG>::protected_engine(std::time_t t) : e_(t)
  {}

  template <std::uniform_random_bit_generator URBG>
  typename protected_engine<URBG>::result_type
  protected_engine<URBG>::operator()()
  {
    std::scoped_lock lock(m_);
    return e_();
  }

  template <std::uniform_random_bit_generator URBG>
  constexpr typename protected_engine<URBG>::result_type
  protected_engine<URBG>::min()
  {
    return engine_type::min();
  }

  template <std::uniform_random_bit_generator URBG>
  constexpr typename protected_engine<URBG>::result_type
  protected_engine<URBG>::max()
  {
    return engine_type::max();
  }
}

#endif