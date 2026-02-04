/**
 *
 *  @file ConversionError.hpp
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
#ifndef VIX_CONVERSION_CONVERSION_ERROR_HPP
#define VIX_CONVERSION_CONVERSION_ERROR_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>

namespace vix::conversion
{

  /**
   * @brief Error codes for low-level type conversion.
   *
   * These errors are purely technical and must not contain any business
   * or validation logic. They are intended for diagnostics, logs, and for
   * higher layers to map into domain level errors.
   */
  enum class ConversionErrorCode : std::uint8_t
  {
    None = 0,

    // Generic
    EmptyInput,
    InvalidCharacter,
    TrailingCharacters,

    // Numeric
    Overflow,
    Underflow,

    // Boolean
    InvalidBoolean,

    // Enum
    UnknownEnumValue,

    // Float
    InvalidFloat
  };

  /**
   * @brief Structured conversion error.
   *
   * Lightweight, no allocations. Carries an error code, the original input,
   * and an optional position hint for failures that point to a character index.
   */
  struct ConversionError
  {
    ConversionErrorCode code{ConversionErrorCode::None};
    std::string_view input{};
    std::size_t position{0};

    constexpr ConversionError() = default;

    constexpr ConversionError(
        ConversionErrorCode c,
        std::string_view in,
        std::size_t pos = 0) noexcept
        : code(c), input(in), position(pos)
    {
    }

    /**
     * @brief True when no error occurred.
     */
    [[nodiscard]] constexpr bool ok() const noexcept
    {
      return code == ConversionErrorCode::None;
    }
  };

  /**
   * @brief Short string representation of ConversionErrorCode.
   *
   * Notes:
   * - Not localized
   * - Not user-facing UI text
   * - Intended for logs and developer diagnostics
   *
   * @param code Error code.
   * @return A stable string representation.
   */
  [[nodiscard]] constexpr std::string_view
  to_string(ConversionErrorCode code) noexcept
  {
    switch (code)
    {
    case ConversionErrorCode::None:
      return "no error";
    case ConversionErrorCode::EmptyInput:
      return "empty input";
    case ConversionErrorCode::InvalidCharacter:
      return "invalid character";
    case ConversionErrorCode::TrailingCharacters:
      return "trailing characters";
    case ConversionErrorCode::Overflow:
      return "numeric overflow";
    case ConversionErrorCode::Underflow:
      return "numeric underflow";
    case ConversionErrorCode::InvalidBoolean:
      return "invalid boolean value";
    case ConversionErrorCode::UnknownEnumValue:
      return "unknown enum value";
    case ConversionErrorCode::InvalidFloat:
      return "invalid floating-point value";
    default:
      return "unknown conversion error";
    }
  }

} // namespace vix::conversion

#endif
