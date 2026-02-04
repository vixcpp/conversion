/**
 *
 *  @file IntegerParse.hpp
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
#ifndef VIX_CONVERSION_INTEGER_PARSE_HPP
#define VIX_CONVERSION_INTEGER_PARSE_HPP

#include <cstddef>
#include <limits>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/detail/Ascii.hpp>

namespace vix::conversion::detail
{

  /**
   * @brief Parse a signed or unsigned integer from ASCII (base 10, strict).
   *
   * Rules:
   * - ASCII only, locale-free
   * - no prefixes (no 0x, no 0b)
   * - optional leading '+' or '-'
   * - at least one digit required
   * - the entire input must be digits (after optional sign)
   * - overflow and underflow are detected
   *
   * Error codes:
   * - EmptyInput when input is empty
   * - InvalidCharacter when a non digit is encountered, or sign without digits
   * - Overflow / Underflow when value does not fit the target type
   *
   * @tparam Int Target integral type.
   * @param input Source input (already trimmed by caller when needed).
   * @return expected<Int, ConversionError> containing the parsed value or an error.
   */
  template <typename Int>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Int, ConversionError>
  parse_integer(std::string_view input) noexcept
  {
    static_assert(std::is_integral_v<Int>, "parse_integer<Int>: Int must be integral");

    using Limits = std::numeric_limits<Int>;

    if (input.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput,
          input});
    }

    std::size_t i = 0;
    bool negative = false;

    // Optional sign
    if (input[i] == '+' || input[i] == '-')
    {
      negative = (input[i] == '-');
      ++i;

      if (i == input.size())
      {
        return make_unexpected(ConversionError{
            ConversionErrorCode::InvalidCharacter,
            input,
            i - 1});
      }
    }

    Int value = 0;
    bool has_digit = false;

    for (; i < input.size(); ++i)
    {
      const char c = input[i];

      if (!is_digit(c))
      {
        return make_unexpected(ConversionError{
            ConversionErrorCode::InvalidCharacter,
            input,
            i});
      }

      has_digit = true;
      const Int digit = static_cast<Int>(c - '0');

      if constexpr (std::is_signed_v<Int>)
      {
        if (negative)
        {
          // value = value * 10 - digit
          if (value < (Limits::min() + digit) / 10)
          {
            return make_unexpected(ConversionError{
                ConversionErrorCode::Underflow,
                input,
                i});
          }
          value = value * 10 - digit;
        }
        else
        {
          // value = value * 10 + digit
          if (value > (Limits::max() - digit) / 10)
          {
            return make_unexpected(ConversionError{
                ConversionErrorCode::Overflow,
                input,
                i});
          }
          value = value * 10 + digit;
        }
      }
      else
      {
        // Unsigned
        if (negative)
        {
          return make_unexpected(ConversionError{
              ConversionErrorCode::Underflow,
              input,
              0});
        }

        if (value > (Limits::max() - digit) / 10)
        {
          return make_unexpected(ConversionError{
              ConversionErrorCode::Overflow,
              input,
              i});
        }

        value = value * 10 + digit;
      }
    }

    if (!has_digit)
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidCharacter,
          input});
    }

    return value;
  }

} // namespace vix::conversion::detail

#endif
