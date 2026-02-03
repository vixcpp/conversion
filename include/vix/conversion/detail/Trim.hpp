#pragma once

#include <string_view>

#include <vix/conversion/detail/Ascii.hpp>

namespace vix::conversion::detail
{

  /**
   * @brief Trim ASCII whitespace on the left.
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
   * @brief Trim ASCII whitespace on the right.
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
   */
  [[nodiscard]] constexpr std::string_view trim(std::string_view s) noexcept
  {
    return rtrim(ltrim(s));
  }

} // namespace vix::conversion::detail
