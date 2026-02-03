#include <cassert>
#include <iostream>
#include <string_view>

#include <vix/conversion/ToBool.hpp>

using vix::conversion::ConversionErrorCode;
using vix::conversion::to_bool;

int main()
{
  // -------------------------
  // Valid true values
  // -------------------------
  {
    auto r = to_bool("true");
    assert(r && r.value() == true);
  }

  {
    auto r = to_bool(" TRUE ");
    assert(r && r.value() == true);
  }

  {
    auto r = to_bool("1");
    assert(r && r.value() == true);
  }

  {
    auto r = to_bool("yes");
    assert(r && r.value() == true);
  }

  {
    auto r = to_bool("On");
    assert(r && r.value() == true);
  }

  // -------------------------
  // Valid false values
  // -------------------------
  {
    auto r = to_bool("false");
    assert(r && r.value() == false);
  }

  {
    auto r = to_bool(" FALSE ");
    assert(r && r.value() == false);
  }

  {
    auto r = to_bool("0");
    assert(r && r.value() == false);
  }

  {
    auto r = to_bool("no");
    assert(r && r.value() == false);
  }

  {
    auto r = to_bool("off");
    assert(r && r.value() == false);
  }

  // -------------------------
  // Empty input
  // -------------------------
  {
    auto r = to_bool("");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  {
    auto r = to_bool("   ");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  // -------------------------
  // Invalid values
  // -------------------------
  {
    auto r = to_bool("maybe");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::InvalidBoolean);
  }

  {
    auto r = to_bool("truee");
    assert(!r);
    assert(r.error().code == ConversionErrorCode::InvalidBoolean);
  }

  std::cout << "[conversion] to_bool smoke tests passed\n";
  return 0;
}
