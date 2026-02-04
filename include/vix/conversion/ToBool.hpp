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
     * @brief ASCII case-insensitive equality for string_view.
     *
     * Compares two strings using ASCII lowercasing for each character.
     * This is intended for parsing keywords like "true", "false", "yes", "no".
     *
     * @param a First string.
     * @param b Second string.
     * @return True if both strings have same size and match ignoring ASCII case.
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
   * @brief Parse a boolean value from string input.
   *
   * The input is trimmed before parsing. Matching is case-insensitive for
   * keyword forms.
   *
   * Accepted values:
   * - "true" / "false"
   * - "1" / "0"
   * - "yes" / "no"
   * - "on" / "off"
   *
   * Error codes:
   * - EmptyInput if trimmed input is empty
   * - InvalidBoolean if value is not recognized
   *
   * @param input Source input.
   * @return expected<bool, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<bool, ConversionError>
  to_bool(std::string_view input) noexcept
  {
    const std::string_view s = vix::conversion::detail::trim(input);

    if (s.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput,
          input});
    }

    if (s == "1" ||
        detail::iequals(s, "true") ||
        detail::iequals(s, "yes") ||
        detail::iequals(s, "on"))
    {
      return true;
    }

    if (s == "0" ||
        detail::iequals(s, "false") ||
        detail::iequals(s, "no") ||
        detail::iequals(s, "off"))
    {
      return false;
    }

    return make_unexpected(ConversionError{
        ConversionErrorCode::InvalidBoolean,
        input});
  }

} // namespace vix::conversion

#endif
