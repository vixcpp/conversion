/**
 *
 *  @file ToFloat.hpp
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
#ifndef VIX_CONVERSION_TO_FLOAT_HPP
#define VIX_CONVERSION_TO_FLOAT_HPP

#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/detail/FloatParse.hpp>
#include <vix/conversion/detail/Trim.hpp>

namespace vix::conversion
{

  /**
   * @brief Parse a floating point value from string input (strict).
   *
   * Rules:
   * - ASCII trim is applied
   * - decimal and scientific notation are supported (e/E)
   * - the entire trimmed input must be consumed (no trailing characters)
   * - overflow and underflow are detected
   *
   * Error codes:
   * - EmptyInput if trimmed input is empty
   * - InvalidFloat if parsing fails
   * - TrailingCharacters if extra characters remain after the number
   * - Overflow or Underflow if the value is outside representable range
   *
   * @tparam Float Target floating point type.
   * @param input Source input.
   * @return expected<Float, ConversionError> containing the parsed value or an error.
   */
  template <typename Float>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Float, ConversionError>
  to_float(std::string_view input) noexcept
  {
    static_assert(std::is_floating_point_v<Float>, "to_float<Float>: Float must be floating point");

    const std::string_view trimmed = detail::trim(input);

    if (trimmed.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput,
          input});
    }

    auto r = detail::parse_float<Float>(trimmed);
    if (!r)
    {
      ConversionError e = r.error();
      e.input = input; // preserve original input for upstream diagnostics
      return make_unexpected(e);
    }

    return r.value();
  }

  /**
   * @brief Parse a 32-bit float from string input.
   *
   * @param input Source input.
   * @return expected<float, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<float, ConversionError>
  to_float32(std::string_view input) noexcept
  {
    return to_float<float>(input);
  }

  /**
   * @brief Parse a 64-bit double from string input.
   *
   * @param input Source input.
   * @return expected<double, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<double, ConversionError>
  to_float64(std::string_view input) noexcept
  {
    return to_float<double>(input);
  }

  /**
   * @brief Parse a long double from string input.
   *
   * Note: the actual precision depends on the platform ABI.
   *
   * @param input Source input.
   * @return expected<long double, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<long double, ConversionError>
  to_float80(std::string_view input) noexcept
  {
    return to_float<long double>(input);
  }

} // namespace vix::conversion

#endif
