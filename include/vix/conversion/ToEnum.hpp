#pragma once

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
   * @brief Enum mapping entry.
   */
  template <typename Enum>
  struct EnumEntry
  {
    std::string_view name;
    Enum value;
  };

  /**
   * @brief Convert a string_view to an enum using an explicit mapping table.
   *
   * Example:
   *   static constexpr EnumEntry<Role> roles[] = {
   *     {"admin", Role::Admin},
   *     {"user",  Role::User}
   *   };
   *
   *   auto r = to_enum<Role>(input, roles);
   */
  template <typename Enum>
  [[nodiscard]] constexpr expected<Enum, ConversionError>
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
          ConversionErrorCode::EmptyInput, input});
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
        ConversionErrorCode::UnknownEnumValue, input});
  }

  /**
   * @brief Helper overload for static arrays.
   */
  template <typename Enum, std::size_t N>
  [[nodiscard]] constexpr expected<Enum, ConversionError>
  to_enum(
      std::string_view input,
      const EnumEntry<Enum> (&entries)[N],
      bool case_insensitive = true) noexcept
  {
    return to_enum(input, entries, N, case_insensitive);
  }

} // namespace vix::conversion
