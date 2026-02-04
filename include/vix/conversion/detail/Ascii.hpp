/**
 *
 *  @file Ascii.hpp
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
#ifndef VIX_CONVERSION_ASCII_HPP
#define VIX_CONVERSION_ASCII_HPP

namespace vix::conversion::detail
{

  /**
   * @brief Minimal ASCII helpers (locale-free).
   *
   * These helpers are deterministic across platforms and do not depend on locale.
   * They are intended for parsing and validation of ASCII input.
   */

  /**
   * @brief Return true if c is an ASCII whitespace character.
   */
  [[nodiscard]] constexpr bool is_space(char c) noexcept
  {
    return c == ' ' || c == '\t' ||
           c == '\n' || c == '\r' ||
           c == '\f' || c == '\v';
  }

  /**
   * @brief Return true if c is an ASCII digit [0-9].
   */
  [[nodiscard]] constexpr bool is_digit(char c) noexcept
  {
    return c >= '0' && c <= '9';
  }

  /**
   * @brief Return true if c is an ASCII letter [A-Z] or [a-z].
   */
  [[nodiscard]] constexpr bool is_alpha(char c) noexcept
  {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
  }

  /**
   * @brief Return true if c is an ASCII letter or digit.
   */
  [[nodiscard]] constexpr bool is_alnum(char c) noexcept
  {
    return is_alpha(c) || is_digit(c);
  }

  /**
   * @brief Return true if c is an ASCII lowercase letter [a-z].
   */
  [[nodiscard]] constexpr bool is_lower(char c) noexcept
  {
    return c >= 'a' && c <= 'z';
  }

  /**
   * @brief Return true if c is an ASCII uppercase letter [A-Z].
   */
  [[nodiscard]] constexpr bool is_upper(char c) noexcept
  {
    return c >= 'A' && c <= 'Z';
  }

  /**
   * @brief Convert an ASCII uppercase letter to lowercase, otherwise return c unchanged.
   */
  [[nodiscard]] constexpr char to_lower(char c) noexcept
  {
    return is_upper(c) ? static_cast<char>(c + ('a' - 'A')) : c;
  }

  /**
   * @brief Convert an ASCII lowercase letter to uppercase, otherwise return c unchanged.
   */
  [[nodiscard]] constexpr char to_upper(char c) noexcept
  {
    return is_lower(c) ? static_cast<char>(c - ('a' - 'A')) : c;
  }

} // namespace vix::conversion::detail

#endif
