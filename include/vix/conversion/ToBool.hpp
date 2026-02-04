/**
 *
 *  @file ToBool.hpp
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
#ifndef VIX_CONVERSION_TO_BOOL_HPP
#define VIX_CONVERSION_TO_BOOL_HPP

#include <string_view>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/detail/Ascii.hpp>
#include <vix/conversion/detail/Trim.hpp>

namespace vix::conversion
{

  namespace detail
  {

    /**
     * @brief Case-insensitive ASCII equals for string_view.
     */
    [[nodiscard]] constexpr bool iequals(std::string_view a, std::string_view b) noexcept
    {
      if (a.size() != b.size())
      {
        return false;
      }

      for (std::size_t i = 0; i < a.size(); ++i)
      {
        if (vix::conversion::detail::to_lower(a[i]) != vix::conversion::detail::to_lower(b[i]))
        {
          return false;
        }
      }

      return true;
    }

  } // namespace detail

  /**
   * @brief Convert a string_view to bool (strict).
   *
   * Accepted values (after trim, case-insensitive):
   * - true / false
   * - 1 / 0
   * - yes / no
   * - on / off
   */
  [[nodiscard]] constexpr expected<bool, ConversionError>
  to_bool(std::string_view input) noexcept
  {
    const std::string_view s = vix::conversion::detail::trim(input);

    if (s.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput, input});
    }

    if (s == "1" || detail::iequals(s, "true") || detail::iequals(s, "yes") || detail::iequals(s, "on"))
    {
      return true;
    }

    if (s == "0" || detail::iequals(s, "false") || detail::iequals(s, "no") || detail::iequals(s, "off"))
    {
      return false;
    }

    return make_unexpected(ConversionError{
        ConversionErrorCode::InvalidBoolean, input});
  }

} // namespace vix::conversion

#endif
