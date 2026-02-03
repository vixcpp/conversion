#include <cassert>
#include <iostream>
#include <string_view>

#include <vix/conversion/ToEnum.hpp>

using vix::conversion::ConversionErrorCode;
using vix::conversion::EnumEntry;
using vix::conversion::to_enum;

enum class Role
{
  Admin,
  User,
  Guest
};

static constexpr EnumEntry<Role> role_entries[] = {
    {"admin", Role::Admin},
    {"user", Role::User},
    {"guest", Role::Guest}};

int main()
{
  // -------------------------
  // Valid cases
  // -------------------------
  {
    auto r = to_enum("admin", role_entries);
    assert(r && r.value() == Role::Admin);
  }

  {
    auto r = to_enum(" USER ", role_entries);
    assert(r && r.value() == Role::User);
  }

  {
    auto r = to_enum("Guest", role_entries);
    assert(r && r.value() == Role::Guest);
  }

  // -------------------------
  // Case-sensitive mode
  // -------------------------
  {
    auto r = to_enum("Admin", role_entries, /*case_insensitive=*/false);
    assert(!r);
    assert(r.error().code == ConversionErrorCode::UnknownEnumValue);
  }

  // -------------------------
  // Empty input
  // -------------------------
  {
    auto r = to_enum("", role_entries);
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  {
    auto r = to_enum("   ", role_entries);
    assert(!r);
    assert(r.error().code == ConversionErrorCode::EmptyInput);
  }

  // -------------------------
  // Unknown value
  // -------------------------
  {
    auto r = to_enum("moderator", role_entries);
    assert(!r);
    assert(r.error().code == ConversionErrorCode::UnknownEnumValue);
  }

  std::cout << "[conversion] to_enum smoke tests passed\n";
  return 0;
}
