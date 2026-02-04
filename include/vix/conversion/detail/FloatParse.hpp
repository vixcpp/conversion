/**
 *
 *  @file FloatParse.hpp
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
#ifndef VIX_CONVERSION_FLOAT_PARSE_HPP
#define VIX_CONVERSION_FLOAT_PARSE_HPP

#include <cstddef>
#include <cerrno>
#include <cstdlib>
#include <limits>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>

namespace vix::conversion::detail
{

  /**
   * @brief Parse a floating point value from ASCII input (strict, C parsing).
   *
   * Notes:
   * - Uses std::strtof / std::strtod / std::strtold internally.
   * - Requires '.' as decimal separator (typical C locale behavior).
   * - Entire input must be consumed (no trailing characters).
   *
   * Error codes:
   * - EmptyInput if input is empty
   * - InvalidFloat if no conversion was performed
   * - TrailingCharacters if extra characters remain after the number
   * - Overflow or Underflow on ERANGE or range checks
   *
   * @tparam Float Target floating point type.
   * @param input Source input (already trimmed by caller when needed).
   * @return expected<Float, ConversionError> containing the parsed value or an error.
   */
  template <typename Float>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Float, ConversionError>
  parse_float(std::string_view input) noexcept
  {
    static_assert(std::is_floating_point_v<Float>, "parse_float<Float>: Float must be floating point");

    if (input.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput,
          input});
    }

    const char *begin = input.data();
    char *end = nullptr;

    errno = 0;

    long double value = 0.0L;

    if constexpr (std::is_same_v<Float, float>)
    {
      value = std::strtof(begin, &end);
    }
    else if constexpr (std::is_same_v<Float, double>)
    {
      value = std::strtod(begin, &end);
    }
    else
    {
      value = std::strtold(begin, &end);
    }

    // No conversion performed
    if (begin == end)
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidFloat,
          input});
    }

    // Trailing characters not allowed
    const std::size_t consumed = static_cast<std::size_t>(end - begin);
    if (consumed != input.size())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::TrailingCharacters,
          input,
          consumed});
    }

    if (errno == ERANGE)
    {
      if (value == 0.0L)
      {
        return make_unexpected(ConversionError{
            ConversionErrorCode::Underflow,
            input});
      }
      return make_unexpected(ConversionError{
          ConversionErrorCode::Overflow,
          input});
    }

    // Final range check for target type
    if (value < static_cast<long double>(std::numeric_limits<Float>::lowest()) ||
        value > static_cast<long double>(std::numeric_limits<Float>::max()))
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::Overflow,
          input});
    }

    return static_cast<Float>(value);
  }

} // namespace vix::conversion::detail

#endif
