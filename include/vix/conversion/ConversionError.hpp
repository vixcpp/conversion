#pragma once

#include <string_view>
#include <cstdint>

namespace vix::conversion
{

  /**
   * @brief Error codes for low-level type conversion.
   *
   * These errors are purely technical and MUST NOT contain
   * business or validation logic.
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
   * Lightweight, trivially copyable, no allocations.
   */
  struct ConversionError
  {
    ConversionErrorCode code{ConversionErrorCode::None};
    std::string_view input{}; // original input (non-owning)
    std::size_t position{0};  // error position if relevant

    constexpr ConversionError() = default;

    constexpr ConversionError(
        ConversionErrorCode c,
        std::string_view in,
        std::size_t pos = 0) noexcept
        : code(c), input(in), position(pos)
    {
    }

    [[nodiscard]] constexpr bool ok() const noexcept
    {
      return code == ConversionErrorCode::None;
    }
  };

  /**
   * @brief Human-readable short description for diagnostics/logs.
   *
   * IMPORTANT:
   * - Not localized
   * - Not user-facing UI text
   * - For logs, debug, and developer diagnostics only
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
