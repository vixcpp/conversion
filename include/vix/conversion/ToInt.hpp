/**
 *
 *  @file ToInt.hpp
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
#ifndef VIX_CONVERSION_TO_INT_HPP
#define VIX_CONVERSION_TO_INT_HPP

#include <cstdint>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/detail/IntegerParse.hpp>
#include <vix/conversion/detail/Trim.hpp>

namespace vix::conversion
{

  /**
   * @brief Parse an integer value from string input (base 10, strict).
   *
   * Rules:
   * - ASCII trim is applied
   * - optional leading '+' or '-'
   * - at least one digit required
   * - entire trimmed input must be numeric
   * - overflow and underflow are detected
   *
   * Error codes:
   * - EmptyInput if trimmed input is empty
   * - InvalidCharacter if a non digit appears where a digit is expected
   * - TrailingCharacters if extra characters remain after the number
   * - Overflow or Underflow if the value does not fit Int
   *
   * @tparam Int Target integral type.
   * @param input Source input.
   * @return expected<Int, ConversionError> containing the parsed value or an error.
   */
  template <typename Int>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Int, ConversionError>
  to_int(std::string_view input) noexcept
  {
    static_assert(std::is_integral_v<Int>, "to_int<Int>: Int must be integral");

    const std::string_view trimmed = detail::trim(input);

    if (trimmed.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput,
          input});
    }

    // Parse using the trimmed view, keep original input in the error for context.
    auto r = detail::parse_integer<Int>(trimmed);
    if (!r)
    {
      ConversionError e = r.error();
      e.input = input; // preserve original input to improve upstream diagnostics
      return make_unexpected(e);
    }

    return r.value();
  }

  /**
   * @brief Parse a signed 32-bit integer from string input.
   *
   * @param input Source input.
   * @return expected<int, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<int, ConversionError>
  to_int32(std::string_view input) noexcept
  {
    return to_int<int>(input);
  }

  /**
   * @brief Parse a signed 64-bit integer from string input.
   *
   * @param input Source input.
   * @return expected<std::int64_t, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::int64_t, ConversionError>
  to_int64(std::string_view input) noexcept
  {
    return to_int<std::int64_t>(input);
  }

  /**
   * @brief Parse an unsigned 32-bit integer from string input.
   *
   * @param input Source input.
   * @return expected<unsigned, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<unsigned, ConversionError>
  to_uint32(std::string_view input) noexcept
  {
    return to_int<unsigned>(input);
  }

  /**
   * @brief Parse an unsigned 64-bit integer from string input.
   *
   * @param input Source input.
   * @return expected<std::uint64_t, ConversionError> containing the parsed value or an error.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::uint64_t, ConversionError>
  to_uint64(std::string_view input) noexcept
  {
    return to_int<std::uint64_t>(input);
  }

} // namespace vix::conversion

#endif
