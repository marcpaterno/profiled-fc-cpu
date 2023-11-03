#ifndef PROFILED_FC_CPU_PROTECTED_ENGINE_HH
#define PROFILED_FC_CPU_PROTECTED_ENGINE_HH

namespace pfc {

  // This class encapsulates an mt19937 engine for shared used between threads.
  // It uses a mutex to protect calls to the function call operator of the
  // contained engine. It provides the minimial interface to be used as a
  // UniformRandomBitGenerator, and thus can be used with the function call
  // operator of any of the distributions in the C++ Standard Library.
  class protected_engine {
  public:
    using result_type = std::mt19937::result_type;
    result_type operator()();

    static constexpr result_type min();
    static constexpr result_type max();

  private:
    using engine_type = std::mt19937;

    std::mutex m_;
    engine_type e_;
  };

  inline protected_engine::result_type
  protected_engine::operator()()
  {
    std::scoped_lock lock(m_);
    return e_();
  }

  inline constexpr protected_engine::result_type
  protected_engine::min()
  {
    return engine_type::min();
  }

  inline constexpr protected_engine::result_type
  protected_engine::max()
  {
    return engine_type::max();
  }
}

#endif