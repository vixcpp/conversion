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

#include <type_traits>
#include <utility>
#include <new>
#include <cassert>

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

namespace vix::conversion
{

// Use std::expected only when it truly exists (C++23 library feature).
#if VIX_HAS_STD_EXPECTED_HEADER && defined(__cpp_lib_expected) && (__cpp_lib_expected >= 202202L)

  template <typename T, typename E>
  using expected = std::expected<T, E>;

  template <typename E>
  using unexpected = std::unexpected<E>;

  template <typename E>
  [[nodiscard]] constexpr unexpected<E> make_unexpected(E err)
  {
    return unexpected<E>(std::move(err));
  }

#else

  // -----------------------------
  // Minimal C++20 fallback expected
  // -----------------------------

  template <typename E>
  class unexpected
  {
  public:
    using error_type = E;

    constexpr explicit unexpected(const E &e)
        : err_(e)
    {
    }

    constexpr explicit unexpected(E &&e)
        : err_(std::move(e))
    {
    }

    [[nodiscard]] constexpr const E &error() const & noexcept { return err_; }
    [[nodiscard]] constexpr E &error() & noexcept { return err_; }
    [[nodiscard]] constexpr E &&error() && noexcept { return std::move(err_); }

  private:
    E err_;
  };

  template <typename E>
  [[nodiscard]] constexpr unexpected<E> make_unexpected(E err)
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
    constexpr expected(const T &v)
        : has_(true)
    {
      ::new (static_cast<void *>(&storage_.val_)) T(v);
    }

    constexpr expected(T &&v)
        : has_(true)
    {
      ::new (static_cast<void *>(&storage_.val_)) T(std::move(v));
    }

    // error constructors
    constexpr expected(const unexpected<E> &u)
        : has_(false)
    {
      ::new (static_cast<void *>(&storage_.err_)) E(u.error());
    }

    constexpr expected(unexpected<E> &&u)
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
        return *this;

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
        return *this;

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
    [[nodiscard]] constexpr bool has_value() const noexcept { return has_; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return has_; }

    [[nodiscard]] constexpr T &value() & noexcept
    {
      assert(has_);
      return storage_.val_;
    }

    [[nodiscard]] constexpr const T &value() const & noexcept
    {
      assert(has_);
      return storage_.val_;
    }

    [[nodiscard]] constexpr T &&value() && noexcept
    {
      assert(has_);
      return std::move(storage_.val_);
    }

    [[nodiscard]] constexpr E &error() & noexcept
    {
      assert(!has_);
      return storage_.err_;
    }

    [[nodiscard]] constexpr const E &error() const & noexcept
    {
      assert(!has_);
      return storage_.err_;
    }

    [[nodiscard]] constexpr E &&error() && noexcept
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
