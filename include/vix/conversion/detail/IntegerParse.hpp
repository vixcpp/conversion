#pragma once

#include <limits>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/detail/Ascii.hpp>

namespace vix::conversion::detail
{

  /**
   * @brief Parse a signed or unsigned integer from ASCII (base 10).
   *
   * Rules:
   * - No locale
   * - No prefixes (no 0x, no 0b)
   * - Optional leading '+' or '-'
   * - At least one digit required
   * - Entire string must be consumed
   */
  template <typename Int>
  [[nodiscard]] constexpr expected<Int, ConversionError>
  parse_integer(std::string_view input) noexcept
  {
    static_assert(std::is_integral_v<Int>, "parse_integer<Int>: Int must be integral");

    using Limits = std::numeric_limits<Int>;

    if (input.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput, input});
    }

    std::size_t i = 0;
    bool negative = false;

    // Sign
    if (input[i] == '+' || input[i] == '-')
    {
      negative = (input[i] == '-');
      ++i;

      if (i == input.size())
      {
        return make_unexpected(ConversionError{
            ConversionErrorCode::InvalidCharacter, input, i - 1});
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
            ConversionErrorCode::InvalidCharacter, input, i});
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
                ConversionErrorCode::Underflow, input, i});
          }
          value = value * 10 - digit;
        }
        else
        {
          // value = value * 10 + digit
          if (value > (Limits::max() - digit) / 10)
          {
            return make_unexpected(ConversionError{
                ConversionErrorCode::Overflow, input, i});
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
              ConversionErrorCode::Underflow, input, 0});
        }

        if (value > (Limits::max() - digit) / 10)
        {
          return make_unexpected(ConversionError{
              ConversionErrorCode::Overflow, input, i});
        }

        value = value * 10 + digit;
      }
    }

    if (!has_digit)
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidCharacter, input});
    }

    return value;
  }

} // namespace vix::conversion::detail
