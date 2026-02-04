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
#include <string>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/ToEnum.hpp>

namespace vix::conversion
{

  /**
   * @brief Convert an integral value to string (base 10).
   */
  template <typename Int>
  [[nodiscard]] inline expected<std::string, ConversionError>
  to_string(Int value) noexcept
  {
    static_assert(std::is_integral_v<Int>, "to_string<Int>: Int must be integral");

    char buffer[64];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

    if (ec != std::errc{})
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidCharacter, {}});
    }

    return std::string(buffer, ptr);
  }

  /**
   * @brief Convert a floating-point value to string.
   *
   * Note:
   * - Uses std::to_chars when available (C++20).
   * - Falls back to std::string formatting behavior defined by the standard.
   */
  template <typename Float>
  [[nodiscard]] inline expected<std::string, ConversionError>
  to_string(Float value) noexcept
  {
    static_assert(std::is_floating_point_v<Float>, "to_string<Float>: Float must be floating point");

    char buffer[128];
    auto [ptr, ec] = std::to_chars(buffer, buffer + sizeof(buffer), value);

    if (ec != std::errc{})
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::InvalidFloat, {}});
    }

    return std::string(buffer, ptr);
  }

  /**
   * @brief Convert bool to string ("true" / "false").
   */
  [[nodiscard]] inline expected<std::string, ConversionError>
  to_string(bool value) noexcept
  {
    return value ? std::string("true") : std::string("false");
  }

  /**
   * @brief Convert enum to string using explicit mapping.
   *
   * Example:
   *   static constexpr EnumEntry<Role> roles[] = {
   *     {"admin", Role::Admin},
   *     {"user",  Role::User}
   *   };
   *
   *   auto s = to_string(Role::Admin, roles);
   */
  template <typename Enum>
  [[nodiscard]] inline expected<std::string, ConversionError>
  to_string(
      Enum value,
      const EnumEntry<Enum> *entries,
      std::size_t count) noexcept
  {
    static_assert(std::is_enum_v<Enum>, "to_string<Enum>: Enum must be an enum");

    for (std::size_t i = 0; i < count; ++i)
    {
      if (entries[i].value == value)
      {
        return std::string(entries[i].name);
      }
    }

    return make_unexpected(ConversionError{
        ConversionErrorCode::UnknownEnumValue, {}});
  }

  /**
   * @brief Helper overload for static arrays.
   */
  template <typename Enum, std::size_t N>
  [[nodiscard]] inline expected<std::string, ConversionError>
  to_string(Enum value, const EnumEntry<Enum> (&entries)[N]) noexcept
  {
    return to_string(value, entries, N);
  }

} // namespace vix::conversion

#endif
