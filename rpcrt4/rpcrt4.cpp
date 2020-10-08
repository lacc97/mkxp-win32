#include "rpcrt4.h"

#include <charconv>

#include "log.h"

namespace {
  enum RPC_RETVAL {
    RPC_S_OK,
    RPC_S_INVALID_STRING_UUID
  };
}

WIN32_API RPC_STATUS rpcrt4_UuidFromString(RPC_CSTR StringUuid, UUID* Uuid) {
  SPDLOG_TRACE("rpcrt4::UuidFromString(StringUuid=\"{}\", Uuid={})", StringUuid, (void*) Uuid);

  std::from_chars_result r{};
  {
    r = std::from_chars(StringUuid, StringUuid + 8, Uuid->Data1, 16);
    if(r.ec != std::errc{}) {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }
  }
  {
    r = std::from_chars(r.ptr + 1, r.ptr + 1 + 4, Uuid->Data2, 16);
    if(r.ec != std::errc{}) {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }
  }
  {
    r = std::from_chars(r.ptr + 1, r.ptr + 1 + 4, Uuid->Data3, 16);
    if(r.ec != std::errc{}) {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }
  }
  {
    USHORT g3;
    r = std::from_chars(r.ptr + 1, r.ptr + 1 + 4, g3, 16);
    if(r.ec != std::errc{}) {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }

    uint64_t d4;
    r = std::from_chars(r.ptr + 1, r.ptr + 1 + 12, d4, 16);
    if(r.ec != std::errc{}) {
      spdlog::error("Invalid Uuid string: {}", StringUuid);
      return RPC_S_INVALID_STRING_UUID;
    }

    d4 |= (uint64_t(g3) << 48u);
    std::memcpy(Uuid->Data4, &d4, sizeof(d4));
  }

  return RPC_S_OK;
}
