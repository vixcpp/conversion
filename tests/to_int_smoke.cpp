#include <cassert>
#include <cstdint>
#include <iostream>
#include <string_view>

#include <vix/conversion/ToInt.hpp>

using vix::conversion::ConversionErrorCode;
using vix::conversion::to_int32;
using vix::conversion::to_int64;

int main()
{
  // -------------------------
  // Basic valid cases
  // -------------------------
  {
    auto r = to_int32("42");
    assert(r && r.value() == 42);
  }

  {
    auto r = to_int32("   42  ");
    assert(r && r.value() == 42);
  }

  {
    auto r = to_int32("+7");
    assert(r && r.value() == 7);
  }

  {
    auto r = to_int32("-7");
    assert(r && r.value() == -7);
  }

  // -------------------------
  // 64-bit
  // -------------------------
  {
    auto r = to_int64("9223372036854775807"); // int64 max
    assert(r && r.value() == INT64_MAX);
  }

  // -------------------------
  // Empty input
  // -------------------------
  {
    auto r = to_int32("");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  {
    auto r = to_int32("   ");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  // -------------------------
  // Invalid characters
  // -------------------------
  {
    auto r = to_int32("12a");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::InvalidCharacter);
  }

  {
    auto r = to_int32("--1");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::InvalidCharacter);
  }

  // -------------------------
  // Overflow / underflow
  // -------------------------
  {
    auto r = to_int32("2147483648"); // int32 overflow
    assert(!r);
    assert(r.error().code == ConversionErrorCode::Overflow);
  }

  {
    auto r = to_int32("-2147483649"); // int32 underflow
    assert(!r);
    assert(r.error().code == ConversionErrorCode::Underflow);
  }

  std::cout << "[conversion] to_int smoke tests passed\n";
  return 0;
}
