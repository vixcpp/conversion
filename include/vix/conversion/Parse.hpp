/**
 *
 *  @file Parse.hpp
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
#ifndef VIX_CONVERSION_PARSE_HPP
#define VIX_CONVERSION_PARSE_HPP

#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/ToBool.hpp>
#include <vix/conversion/ToFloat.hpp>
#include <vix/conversion/ToInt.hpp>
#include <vix/conversion/ToEnum.hpp>

namespace vix::conversion
{

  /**
   * @brief Generic parse<T> for common scalar types.
   *
   * Supported:
   * - Integral types
   * - Floating point types
   * - bool
   *
   * For enums:
   * - use parse_enum<T>(input, mapping) or to_enum<T>(...)
   */
  template <typename T>
  [[nodiscard]] inline expected<T, ConversionError>
  parse(std::string_view input) noexcept
  {
    if constexpr (std::is_same_v<T, bool>)
    {
      return to_bool(input);
    }
    else if constexpr (std::is_integral_v<T>)
    {
      return to_int<T>(input);
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
      return to_float<T>(input);
    }
    else
    {
      static_assert(!std::is_same_v<T, T>,
                    "vix::conversion::parse<T>: unsupported type. "
                    "Supported: integral, floating point, bool. "
                    "For enums, use parse_enum<T>(input, mapping).");
    }
  }

  /**
   * @brief Parse an enum using an explicit mapping table.
   */
  template <typename Enum>
  [[nodiscard]] inline expected<Enum, ConversionError>
  parse_enum(
      std::string_view input,
      const EnumEntry<Enum> *entries,
      std::size_t count,
      bool case_insensitive = true) noexcept
  {
    return to_enum<Enum>(input, entries, count, case_insensitive);
  }

  /**
   * @brief Helper overload for static arrays.
   */
  template <typename Enum, std::size_t N>
  [[nodiscard]] inline expected<Enum, ConversionError>
  parse_enum(
      std::string_view input,
      const EnumEntry<Enum> (&entries)[N],
      bool case_insensitive = true) noexcept
  {
    return to_enum<Enum>(input, entries, N, case_insensitive);
  }

} // namespace vix::conversion

#endif
