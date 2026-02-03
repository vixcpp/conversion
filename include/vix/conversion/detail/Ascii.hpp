#pragma once

#include <cstdint>

namespace vix::conversion::detail
{

  /**
   * @brief ASCII character helpers.
   *
   * These helpers are intentionally minimal and locale-free.
   * They must behave deterministically across platforms.
   */

  [[nodiscard]] constexpr bool is_space(char c) noexcept
  {
    return c == ' ' || c == '\t' ||
           c == '\n' || c == '\r' ||
           c == '\f' || c == '\v';
  }

  [[nodiscard]] constexpr bool is_digit(char c) noexcept
  {
    return c >= '0' && c <= '9';
  }

  [[nodiscard]] constexpr bool is_alpha(char c) noexcept
  {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
  }

  [[nodiscard]] constexpr bool is_alnum(char c) noexcept
  {
    return is_alpha(c) || is_digit(c);
  }

  [[nodiscard]] constexpr bool is_lower(char c) noexcept
  {
    return c >= 'a' && c <= 'z';
  }

  [[nodiscard]] constexpr bool is_upper(char c) noexcept
  {
    return c >= 'A' && c <= 'Z';
  }

  [[nodiscard]] constexpr char to_lower(char c) noexcept
  {
    return is_upper(c) ? static_cast<char>(c + ('a' - 'A')) : c;
  }

  [[nodiscard]] constexpr char to_upper(char c) noexcept
  {
    return is_lower(c) ? static_cast<char>(c - ('a' - 'A')) : c;
  }

} // namespace vix::conversion::detail
