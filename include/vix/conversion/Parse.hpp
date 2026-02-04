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
#include <vix/conversion/ToEnum.hpp>
#include <vix/conversion/ToFloat.hpp>
#include <vix/conversion/ToInt.hpp>

namespace vix::conversion
{

  /**
   * @brief Parse a scalar value from string input.
   *
   * Converts a string representation into a strongly typed scalar.
   * The result is returned as an expected-like object:
   * - success: contains the parsed value
   * - failure: contains ConversionError describing the failure
   *
   * Supported types:
   * - bool
   * - integral types
   * - floating point types
   *
   * For enums:
   * - use parse_enum (mapping table)
   * - or call to_enum directly
   *
   * @tparam T Target type to parse.
   * @param input Source input as string_view.
   * @return expected<T, ConversionError> with value on success, error on failure.
   */
  template <typename T>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<T, ConversionError>
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
                    "For enums, use parse_enum<T>(...).");
    }
  }

  /**
   * @brief Parse an enum from string input using an explicit mapping table.
   *
   * @tparam Enum Enum type.
   * @param input Source input as string_view.
   * @param entries Pointer to mapping table entries.
   * @param count Number of entries in the mapping table.
   * @param case_insensitive Whether the match should ignore ASCII case.
   * @return expected<Enum, ConversionError> with value on success, error on failure.
   */
  template <typename Enum>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Enum, ConversionError>
  parse_enum(
      std::string_view input,
      const EnumEntry<Enum> *entries,
      std::size_t count,
      bool case_insensitive = true) noexcept
  {
    return to_enum<Enum>(input, entries, count, case_insensitive);
  }

  /**
   * @brief Parse an enum from string input using a static array mapping table.
   *
   * @tparam Enum Enum type.
   * @tparam N Mapping table size.
   * @param input Source input as string_view.
   * @param entries Mapping table array.
   * @param case_insensitive Whether the match should ignore ASCII case.
   * @return expected<Enum, ConversionError> with value on success, error on failure.
   */
  template <typename Enum, std::size_t N>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Enum, ConversionError>
  parse_enum(
      std::string_view input,
      const EnumEntry<Enum> (&entries)[N],
      bool case_insensitive = true) noexcept
  {
    return to_enum<Enum>(input, entries, N, case_insensitive);
  }

} // namespace vix::conversion

#endif
