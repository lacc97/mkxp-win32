#include "rpcrt4.h"

#include <charconv>
#include <optional>

#include "log.h"

namespace {
  enum RPC_RETVAL : RPC_STATUS { RPC_S_OK, RPC_S_INVALID_STRING_UUID };

  template <typename I>
  inline std::optional<std::enable_if_t<std::is_integral_v<I>, I>> parse_integer(std::string_view s, int base = 10) noexcept {
    I i;
    auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), i, base);
    if(ec == std::errc{})
      return i;
    else
      return std::nullopt;
  }
}    // namespace

WIN32_API RPC_STATUS rpcrt4_UuidFromString(RPC_CSTR StringUuid, UUID* Uuid) {
  SPDLOG_TRACE("rpcrt4::UuidFromString(StringUuid=\"{}\", Uuid={})", StringUuid, (void*) Uuid);

  std::string_view uuid_s = StringUuid;

  if(auto d1 = parse_integer<uint32_t>(uuid_s.substr(0, 8), 16)) {
    Uuid->Data1 = *d1;
  } else {
    spdlog::error("Invalid Uuid string: {}", StringUuid);
    return RPC_S_INVALID_STRING_UUID;
  }

  if(auto d2 = parse_integer<uint16_t>(uuid_s.substr(9, 4), 16)) {
    Uuid->Data2 = *d2;
  } else {
    spdlog::error("Invalid Uuid string: {}", StringUuid);
    return RPC_S_INVALID_STRING_UUID;
  }

  if(auto d3 = parse_integer<uint16_t>(uuid_s.substr(14, 4), 16)) {
    Uuid->Data3 = *d3;
  } else {
    spdlog::error("Invalid Uuid string: {}", StringUuid);
    return RPC_S_INVALID_STRING_UUID;
  }

  {
    uint64_t Data4;
    if(auto d4_1 = parse_integer<uint64_t>(uuid_s.substr(19, 4), 16)) {
      Data4 = (*d4_1 << 48u);
    } else {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }

    if(auto d4_2 = parse_integer<uint64_t>(uuid_s.substr(24, 12), 16)) {
      Data4 |= *d4_2;
    } else {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }

    std::memcpy(Uuid->Data4, &Data4, sizeof Data4);
  }

  return RPC_S_OK;
}
