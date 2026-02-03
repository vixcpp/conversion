#pragma once

#include <type_traits>
#include <utility>

#if __has_include(<expected>)
#include <expected>
#define VIX_CONVERSION_HAS_STD_EXPECTED 1
#else
#define VIX_CONVERSION_HAS_STD_EXPECTED 0
#endif

namespace vix::conversion
{

/**
 * @brief expected<T, E> alias used by the conversion module.
 *
 * Policy:
 * - Prefer std::expected when available.
 * - Provide a small fallback when std::expected is not available.
 *
 * This type is used to avoid exceptions in conversion code.
 */
#if VIX_CONVERSION_HAS_STD_EXPECTED

  template <typename T, typename E>
  using expected = std::expected<T, E>;

  template <typename E>
  using unexpected = std::unexpected<E>;

#else

  template <typename E>
  class unexpected
  {
  public:
    using error_type = E;

    constexpr explicit unexpected(const E &e) : error_(e) {}
    constexpr explicit unexpected(E &&e) noexcept(std::is_nothrow_move_constructible_v<E>)
        : error_(std::move(e)) {}

    [[nodiscard]] constexpr const E &error() const & noexcept { return error_; }
    [[nodiscard]] constexpr E &error() & noexcept { return error_; }
    [[nodiscard]] constexpr E &&error() && noexcept { return std::move(error_); }

  private:
    E error_;
  };

  template <typename T, typename E>
  class expected
  {
  public:
    using value_type = T;
    using error_type = E;

    // Constructors
    constexpr expected(const T &v) : has_(true) { new (&storage_.value) T(v); }
    constexpr expected(T &&v) noexcept(std::is_nothrow_move_constructible_v<T>)
        : has_(true) { new (&storage_.value) T(std::move(v)); }

    constexpr expected(const unexpected<E> &u) : has_(false) { new (&storage_.error) E(u.error()); }
    constexpr expected(unexpected<E> &&u) noexcept(std::is_nothrow_move_constructible_v<E>)
        : has_(false) { new (&storage_.error) E(std::move(u).error()); }

    constexpr expected(const expected &o)
        : has_(o.has_)
    {
      if (has_)
        new (&storage_.value) T(o.storage_.value);
      else
        new (&storage_.error) E(o.storage_.error);
    }

    constexpr expected(expected &&o) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_move_constructible_v<E>)
        : has_(o.has_)
    {
      if (has_)
        new (&storage_.value) T(std::move(o.storage_.value));
      else
        new (&storage_.error) E(std::move(o.storage_.error));
    }

    constexpr expected &operator=(const expected &o)
    {
      if (this == &o)
        return *this;
      destroy_();
      has_ = o.has_;
      if (has_)
        new (&storage_.value) T(o.storage_.value);
      else
        new (&storage_.error) E(o.storage_.error);
      return *this;
    }

    constexpr expected &operator=(expected &&o) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_move_constructible_v<E>)
    {
      if (this == &o)
        return *this;
      destroy_();
      has_ = o.has_;
      if (has_)
        new (&storage_.value) T(std::move(o.storage_.value));
      else
        new (&storage_.error) E(std::move(o.storage_.error));
      return *this;
    }

    ~expected() { destroy_(); }

    // Observers
    [[nodiscard]] constexpr bool has_value() const noexcept { return has_; }
    [[nodiscard]] constexpr explicit operator bool() const noexcept { return has_; }

    [[nodiscard]] constexpr T &value() & { return storage_.value; }
    [[nodiscard]] constexpr const T &value() const & { return storage_.value; }
    [[nodiscard]] constexpr T &&value() && { return std::move(storage_.value); }

    [[nodiscard]] constexpr E &error() & { return storage_.error; }
    [[nodiscard]] constexpr const E &error() const & { return storage_.error; }
    [[nodiscard]] constexpr E &&error() && { return std::move(storage_.error); }

    // Convenience
    [[nodiscard]] constexpr T value_or(T fallback) const
    {
      return has_ ? storage_.value : fallback;
    }

  private:
    constexpr void destroy_() noexcept
    {
      if (has_)
        storage_.value.~T();
      else
        storage_.error.~E();
    }

    union Storage
    {
      constexpr Storage() {}
      ~Storage() {}

      T value;
      E error;
    } storage_{};

    bool has_{false};
  };

#endif // VIX_CONVERSION_HAS_STD_EXPECTED

  /**
   * @brief Helper to build an unexpected error in a consistent namespace.
   */
  template <typename E>
  [[nodiscard]] constexpr unexpected<E> make_unexpected(E err)
  {
    return unexpected<E>(std::move(err));
  }

} // namespace vix::conversion
