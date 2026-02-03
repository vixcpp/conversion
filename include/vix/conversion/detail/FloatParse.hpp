#pragma once

#include <cerrno>
#include <cmath>
#include <limits>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>

namespace vix::conversion::detail
{

  /**
   * @brief Parse a floating-point number from ASCII (C locale).
   *
   * Notes:
   * - Uses std::strtod / std::strtof / std::strtold internally
   * - Locale-independent when input is ASCII and decimal separator is '.'
   * - Entire string must be consumed
   */
  template <typename Float>
  [[nodiscard]] inline expected<Float, ConversionError>
  parse_float(std::string_view input) noexcept
  {
    static_assert(std::is_floating_point_v<Float>, "parse_float<Float>: Float must be floating point");

    if (input.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput, input});
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
          ConversionErrorCode::InvalidFloat, input});
    }

    // Trailing characters not allowed
    if (static_cast<std::size_t>(end - begin) != input.size())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::TrailingCharacters, input,
          static_cast<std::size_t>(end - begin)});
    }

    if (errno == ERANGE)
    {
      if (value == 0.0L)
      {
        return make_unexpected(ConversionError{
            ConversionErrorCode::Underflow, input});
      }
      return make_unexpected(ConversionError{
          ConversionErrorCode::Overflow, input});
    }

    // Final range check for target type
    if (value < static_cast<long double>(std::numeric_limits<Float>::lowest()) ||
        value > static_cast<long double>(std::numeric_limits<Float>::max()))
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::Overflow, input});
    }

    return static_cast<Float>(value);
  }

} // namespace vix::conversion::detail
