/**
 *
 *  @file ToString.hpp
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
#ifndef VIX_CONVERSION_TO_STRING_HPP
#define VIX_CONVERSION_TO_STRING_HPP

#include <charconv>
#include <cstddef>
#include <string>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/ToEnum.hpp>

namespace vix::conversion
{

  /**
   * @brief Convert an integral value to a decimal string (base 10).
   *
   * Uses std::to_chars for fast, allocation-free formatting into a stack buffer.
   *
   * Note: bool is excluded from this overload (use to_string(bool)).
   *
   * @tparam Int Integral type (excluding bool).
   * @param value Value to format.
   * @return expected<std::string, ConversionError> containing the string or an error.
   */
  template <typename Int>
    requires(std::is_integral_v<Int> && !std::is_same_v<std::remove_cv_t<Int>, bool>)
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::string, ConversionError>
  to_string(Int value) noexcept
  {
    char buffer[64];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

    if (ec != std::errc{})
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidCharacter,
          {}});
    }

    return std::string(buffer, ptr);
  }

  /**
   * @brief Convert a floating point value to string.
   *
   * Uses std::to_chars. If formatting fails, returns InvalidFloat.
   *
   * @tparam Float Floating point type.
   * @param value Value to format.
   * @return expected<std::string, ConversionError> containing the string or an error.
   */
  template <typename Float>
    requires(std::is_floating_point_v<Float>)
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::string, ConversionError>
  to_string(Float value) noexcept
  {
    char buffer[128];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

    if (ec != std::errc{})
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidFloat,
          {}});
    }

    return std::string(buffer, ptr);
  }

  /**
   * @brief Convert bool to string ("true" or "false").
   *
   * @param value Boolean value.
   * @return expected<std::string, ConversionError> containing the string.
   */
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::string, ConversionError>
  to_string(bool value) noexcept
  {
    return value ? std::string("true") : std::string("false");
  }

  /**
   * @brief Convert an enum to string using an explicit mapping table.
   *
   * If the enum value is not found in the mapping table, returns UnknownEnumValue.
   *
   * @tparam Enum Enum type.
   * @param value Enum value.
   * @param entries Mapping table entries.
   * @param count Number of entries.
   * @return expected<std::string, ConversionError> containing the string or an error.
   */
  template <typename Enum>
    requires(std::is_enum_v<Enum>)
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::string, ConversionError>
  to_string(
      Enum value,
      const EnumEntry<Enum> *entries,
      std::size_t count) noexcept
  {
    for (std::size_t i = 0; i < count; ++i)
    {
      if (entries[i].value == value)
      {
        return std::string(entries[i].name);
      }
    }

    return make_unexpected(ConversionError{
        ConversionErrorCode::UnknownEnumValue,
        {}});
  }

  /**
   * @brief Convert an enum to string using a static array mapping table.
   *
   * @tparam Enum Enum type.
   * @tparam N Mapping table size.
   * @param value Enum value.
   * @param entries Mapping table array.
   * @return expected<std::string, ConversionError> containing the string or an error.
   */
  template <typename Enum, std::size_t N>
    requires(std::is_enum_v<Enum>)
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<std::string, ConversionError>
  to_string(Enum value, const EnumEntry<Enum> (&entries)[N]) noexcept
  {
    return to_string(value, entries, N);
  }

} // namespace vix::conversion

#endif // VIX_CONVERSION_TO_STRING_HPP
