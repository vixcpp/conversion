#pragma once

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
   * @brief Convert a string_view to an integer type (base 10, strict).
   *
   * Rules:
   * - ASCII trim is applied automatically.
   * - Optional '+' or '-'
   * - At least one digit required
   * - Entire trimmed string must be numeric
   * - Overflow and underflow are detected
   */
  template <typename Int>
  [[nodiscard]] constexpr expected<Int, ConversionError>
  to_int(std::string_view input) noexcept
  {
    static_assert(std::is_integral_v<Int>, "to_int<Int>: Int must be integral");

    const std::string_view trimmed = detail::trim(input);

    if (trimmed.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput, input});
    }

    // Parse using the trimmed view, but keep original input in the error for context.
    auto r = detail::parse_integer<Int>(trimmed);
    if (!r)
    {
      ConversionError e = r.error();
      // Preserve original input to improve diagnostics upstream.
      e.input = input;
      return make_unexpected(e);
    }

    return r.value();
  }

  // Convenience aliases

  [[nodiscard]] constexpr expected<int, ConversionError>
  to_int32(std::string_view input) noexcept
  {
    return to_int<int>(input);
  }

  [[nodiscard]] constexpr expected<std::int64_t, ConversionError>
  to_int64(std::string_view input) noexcept
  {
    return to_int<std::int64_t>(input);
  }

  [[nodiscard]] constexpr expected<unsigned, ConversionError>
  to_uint32(std::string_view input) noexcept
  {
    return to_int<unsigned>(input);
  }

  [[nodiscard]] constexpr expected<std::uint64_t, ConversionError>
  to_uint64(std::string_view input) noexcept
  {
    return to_int<std::uint64_t>(input);
  }

} // namespace vix::conversion
