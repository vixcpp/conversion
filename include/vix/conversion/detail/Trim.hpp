/**
 *
 *  @file Trim.hpp
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
#ifndef VIX_CONVERSION_TRIM_HPP
#define VIX_CONVERSION_TRIM_HPP

#include <cstddef>
#include <string_view>

#include <vix/conversion/detail/Ascii.hpp>

namespace vix::conversion::detail
{

  /**
   * @brief Trim ASCII whitespace on the left side.
   *
   * Whitespace is defined by detail::is_space (ASCII only, locale-free).
   *
   * @param s Input string view.
   * @return View with leading ASCII whitespace removed.
   */
  [[nodiscard]] constexpr std::string_view ltrim(std::string_view s) noexcept
  {
    std::size_t i = 0;
    while (i < s.size() && is_space(s[i]))
    {
      ++i;
    }
    return s.substr(i);
  }

  /**
   * @brief Trim ASCII whitespace on the right side.
   *
   * @param s Input string view.
   * @return View with trailing ASCII whitespace removed.
   */
  [[nodiscard]] constexpr std::string_view rtrim(std::string_view s) noexcept
  {
    std::size_t n = s.size();
    while (n > 0 && is_space(s[n - 1]))
    {
      --n;
    }
    return s.substr(0, n);
  }

  /**
   * @brief Trim ASCII whitespace on both sides.
   *
   * @param s Input string view.
   * @return View with leading and trailing ASCII whitespace removed.
   */
  [[nodiscard]] constexpr std::string_view trim(std::string_view s) noexcept
  {
    return rtrim(ltrim(s));
  }

} // namespace vix::conversion::detail

#endif
