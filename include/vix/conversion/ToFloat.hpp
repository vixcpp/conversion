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
   * @brief Convert a string_view to a floating-point type (strict).
   *
   * Rules:
   * - ASCII trim is applied automatically.
   * - Accepts decimal and scientific notation (e/E) using C parsing.
   * - Entire trimmed string must be consumed (no trailing chars).
   * - Overflow/underflow are detected.
   */
  template <typename Float>
  [[nodiscard]] inline expected<Float, ConversionError>
  to_float(std::string_view input) noexcept
  {
    static_assert(std::is_floating_point_v<Float>, "to_float<Float>: Float must be floating point");

    const std::string_view trimmed = detail::trim(input);

    if (trimmed.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput, input});
    }

    auto r = detail::parse_float<Float>(trimmed);
    if (!r)
    {
      ConversionError e = r.error();
      e.input = input; // keep original for upstream diagnostics
      return make_unexpected(e);
    }

    return r.value();
  }

  // Convenience wrappers

  [[nodiscard]] inline expected<float, ConversionError>
  to_float32(std::string_view input) noexcept
  {
    return to_float<float>(input);
  }

  [[nodiscard]] inline expected<double, ConversionError>
  to_float64(std::string_view input) noexcept
  {
    return to_float<double>(input);
  }

  [[nodiscard]] inline expected<long double, ConversionError>
  to_float80(std::string_view input) noexcept
  {
    return to_float<long double>(input);
  }

} // namespace vix::conversion

#endif
