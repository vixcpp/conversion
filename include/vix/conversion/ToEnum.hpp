/**
 *
 *  @file ToEnum.hpp
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
#ifndef VIX_CONVERSION_TO_ENUM_HPP
#define VIX_CONVERSION_TO_ENUM_HPP

#include <cstddef>
#include <string_view>
#include <type_traits>

#include <vix/conversion/ConversionError.hpp>
#include <vix/conversion/Expected.hpp>
#include <vix/conversion/detail/Ascii.hpp>
#include <vix/conversion/detail/Trim.hpp>

namespace vix::conversion
{

  /**
   * @brief Mapping entry used to convert between string and enum values.
   *
   * @tparam Enum Enum type.
   */
  template <typename Enum>
  struct EnumEntry
  {
    std::string_view name;
    Enum value;
  };

  /**
   * @brief Parse an enum value from string input using an explicit mapping table.
   *
   * The input is trimmed before matching. Matching can be ASCII case-insensitive.
   *
   * Example:
   *   static constexpr EnumEntry<Role> roles[] = {
   *     {"admin", Role::Admin},
   *     {"user",  Role::User}
   *   };
   *
   *   auto r = to_enum<Role>(input, roles, 2);
   *
   * Error codes:
   * - EmptyInput if trimmed input is empty
   * - UnknownEnumValue if no entry matches
   *
   * @tparam Enum Enum type.
   * @param input Source input.
   * @param entries Mapping table entries.
   * @param count Number of entries.
   * @param case_insensitive Whether comparison ignores ASCII case.
   * @return expected<Enum, ConversionError> containing the enum value or an error.
   */
  template <typename Enum>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Enum, ConversionError>
  to_enum(
      std::string_view input,
      const EnumEntry<Enum> *entries,
      std::size_t count,
      bool case_insensitive = true) noexcept
  {
    static_assert(std::is_enum_v<Enum>, "to_enum<Enum>: Enum must be an enum type");

    const std::string_view s = vix::conversion::detail::trim(input);

    if (s.empty())
    {
      return make_unexpected(ConversionError{
          ConversionErrorCode::EmptyInput,
          input});
    }

    for (std::size_t i = 0; i < count; ++i)
    {
      const std::string_view name = entries[i].name;

      if (name.size() != s.size())
      {
        continue;
      }

      bool match = true;

      for (std::size_t j = 0; j < s.size(); ++j)
      {
        char a = s[j];
        char b = name[j];

        if (case_insensitive)
        {
          a = vix::conversion::detail::to_lower(a);
          b = vix::conversion::detail::to_lower(b);
        }

        if (a != b)
        {
          match = false;
          break;
        }
      }

      if (match)
      {
        return entries[i].value;
      }
    }

    return make_unexpected(ConversionError{
        ConversionErrorCode::UnknownEnumValue,
        input});
  }

  /**
   * @brief Parse an enum value from string input using a static array mapping table.
   *
   * @tparam Enum Enum type.
   * @tparam N Mapping table size.
   * @param input Source input.
   * @param entries Mapping table array.
   * @param case_insensitive Whether comparison ignores ASCII case.
   * @return expected<Enum, ConversionError> containing the enum value or an error.
   */
  template <typename Enum, std::size_t N>
  [[nodiscard]] VIX_EXPECTED_CONSTEXPR expected<Enum, ConversionError>
  to_enum(
      std::string_view input,
      const EnumEntry<Enum> (&entries)[N],
      bool case_insensitive = true) noexcept
  {
    return to_enum(input, entries, N, case_insensitive);
  }

} // namespace vix::conversion

#endif
