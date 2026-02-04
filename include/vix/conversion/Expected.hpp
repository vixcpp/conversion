/**
 *
 *  @file Expected.hpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2025, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 */
#ifndef VIX_CONVERSION_EXPECTED_HPP
#define VIX_CONVERSION_EXPECTED_HPP

#include <cassert>
#include <new>
#include <type_traits>
#include <utility>

#if defined(__has_include)
#if __has_include(<expected>)
#include <expected>
#define VIX_HAS_STD_EXPECTED_HEADER 1
#else
#define VIX_HAS_STD_EXPECTED_HEADER 0
#endif
#else
#define VIX_HAS_STD_EXPECTED_HEADER 0
#endif

/**
 * Use std::expected only when the standard library actually supports it.
 * __cpp_lib_expected is a C++23 library feature macro.
 */
#if VIX_HAS_STD_EXPECTED_HEADER && defined(__cpp_lib_expected) && (__cpp_lib_expected >= 202202L)
#define VIX_CONVERSION_EXPECTED_IS_STD 1
#else
#define VIX_CONVERSION_EXPECTED_IS_STD 0
#endif

/**
 * In C++23 mode with std::expected, functions returning expected can be constexpr.
 * In fallback mode, expected is not a literal type in C++20, so constexpr would warn.
 */
#if VIX_CONVERSION_EXPECTED_IS_STD
#define VIX_EXPECTED_CONSTEXPR constexpr
#else
#define VIX_EXPECTED_CONSTEXPR inline
#endif

namespace vix::conversion
{

  /**
   * @brief expected/unexpected abstraction for Vix conversion APIs.
   *
   * Vix conversion must work in C++20, but std::expected is C++23.
   * This header provides:
   * - std::expected alias when available
   * - otherwise a small C++20 fallback with the minimal API Vix needs
   *
   * The fallback supports:
   * - operator bool / has_value
   * - value() / error()
   * - unexpected<E> and make_unexpected(E)
   */

#if VIX_CONVERSION_EXPECTED_IS_STD

  template <typename T, typename E>
  using expected = std::expected<T, E>;

  template <typename E>
  using unexpected = std::unexpected<E>;

  template <typename E>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR unexpected<E> make_unexpected(E err)
  {
    return unexpected<E>(std::move(err));
  }

#else

  template <typename E>
  class unexpected
  {
  public:
    using error_type = E;

    explicit unexpected(const E &e)
        : err_(e)
    {
    }

    explicit unexpected(E &&e)
        : err_(std::move(e))
    {
    }

    [[nodiscard]] const E &error() const & noexcept { return err_; }
    [[nodiscard]] E &error() & noexcept { return err_; }
    [[nodiscard]] E &&error() && noexcept { return std::move(err_); }

  private:
    E err_;
  };

  template <typename E>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR unexpected<E> make_unexpected(E err)
  {
    return unexpected<E>(std::move(err));
  }

  template <typename T, typename E>
  class expected
  {
  public:
    using value_type = T;
    using error_type = E;

    // value constructors
    expected(const T &v)
        : has_(true)
    {
      ::new (static_cast<void *>(&storage_.val_)) T(v);
    }

    expected(T &&v)
        : has_(true)
    {
      ::new (static_cast<void *>(&storage_.val_)) T(std::move(v));
    }

    // error constructors
    expected(const unexpected<E> &u)
        : has_(false)
    {
      ::new (static_cast<void *>(&storage_.err_)) E(u.error());
    }

    expected(unexpected<E> &&u)
        : has_(false)
    {
      ::new (static_cast<void *>(&storage_.err_)) E(std::move(u).error());
    }

    // copy/move
    expected(const expected &o)
        : has_(o.has_)
    {
      if (has_)
        ::new (static_cast<void *>(&storage_.val_)) T(o.storage_.val_);
      else
        ::new (static_cast<void *>(&storage_.err_)) E(o.storage_.err_);
    }

    expected(expected &&o) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                    std::is_nothrow_move_constructible_v<E>)
        : has_(o.has_)
    {
      if (has_)
        ::new (static_cast<void *>(&storage_.val_)) T(std::move(o.storage_.val_));
      else
        ::new (static_cast<void *>(&storage_.err_)) E(std::move(o.storage_.err_));
    }

    expected &operator=(const expected &o)
    {
      if (this == &o)
      {
        return *this;
      }

      this->~expected();
      has_ = o.has_;

      if (has_)
        ::new (static_cast<void *>(&storage_.val_)) T(o.storage_.val_);
      else
        ::new (static_cast<void *>(&storage_.err_)) E(o.storage_.err_);

      return *this;
    }

    expected &operator=(expected &&o) noexcept(std::is_nothrow_move_constructible_v<T> &&
                                               std::is_nothrow_move_constructible_v<E>)
    {
      if (this == &o)
      {
        return *this;
      }

      this->~expected();
      has_ = o.has_;

      if (has_)
        ::new (static_cast<void *>(&storage_.val_)) T(std::move(o.storage_.val_));
      else
        ::new (static_cast<void *>(&storage_.err_)) E(std::move(o.storage_.err_));

      return *this;
    }

    ~expected()
    {
      if (has_)
        storage_.val_.~T();
      else
        storage_.err_.~E();
    }

    // observers
    [[nodiscard]] bool has_value() const noexcept { return has_; }
    [[nodiscard]] explicit operator bool() const noexcept { return has_; }

    // accessors
    [[nodiscard]] T &value() & noexcept
    {
      assert(has_);
      return storage_.val_;
    }

    [[nodiscard]] const T &value() const & noexcept
    {
      assert(has_);
      return storage_.val_;
    }

    [[nodiscard]] T &&value() && noexcept
    {
      assert(has_);
      return std::move(storage_.val_);
    }

    [[nodiscard]] E &error() & noexcept
    {
      assert(!has_);
      return storage_.err_;
    }

    [[nodiscard]] const E &error() const & noexcept
    {
      assert(!has_);
      return storage_.err_;
    }

    [[nodiscard]] E &&error() && noexcept
    {
      assert(!has_);
      return std::move(storage_.err_);
    }

  private:
    union Storage
    {
      T val_;
      E err_;

      Storage() {}
      ~Storage() {}
    } storage_;

    bool has_{false};
  };

#endif

} // namespace vix::conversion

#endif
