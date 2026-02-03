#include <cassert>
#include <cmath>
#include <iostream>
#include <string_view>

#include <vix/conversion/ToFloat.hpp>

using vix::conversion::to_float32;
using vix::conversion::to_float64;
using vix::conversion::ConversionErrorCode;

int main()
{
  // -------------------------
  // Basic valid cases
  // -------------------------
  {
    auto r = to_float32("3.14");
    assert(r);
    assert(std::fabs(r.value() - 3.14f) < 1e-6f);
  }

  {
    auto r = to_float64("   2.5  ");
    assert(r);
    assert(std::fabs(r.value() - 2.5) < 1e-12);
  }

  {
    auto r = to_float64("-1.25");
    assert(r);
    assert(std::fabs(r.value() + 1.25) < 1e-12);
  }

  // -------------------------
  // Scientific notation
  // -------------------------
  {
    auto r = to_float64("1e3");
    assert(r);
    assert(std::fabs(r.value() - 1000.0) < 1e-9);
  }

  {
    auto r = to_float64("2.5E-2");
    assert(r);
    assert(std::fabs(r.value() - 0.025) < 1e-12);
  }

  // -------------------------
  // Empty input
  // -------------------------
  {
    auto r = to_float32("");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  {
    auto r = to_float64("   ");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  // -------------------------
  // Invalid format
  // -------------------------
  {
    auto r = to_float64("1.2.3");
    assert(!r);
    assert(
      r.error().code == ConversionErrorCode::InvalidFloat ||
      r.error().code == ConversionErrorCode::TrailingCharacters
    );
  }

  {
    auto r = to_float64("abc");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::InvalidFloat);
  }

  // -------------------------
  // Overflow / underflow
  // -------------------------
  {
    auto r = to_float64("1e10000"); // overflow
    assert(!r);
    assert(r.error().code == ConversionErrorCode::Overflow);
  }

  {
    auto r = to_float64("1e-10000"); // underflow
    assert(!r);
    assert(r.error().code == ConversionErrorCode::Underflow);
  }

  std::cout << "[conversion] to_float smoke tests passed\n";
  return 0;
}
