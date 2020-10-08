#ifndef RPCRT4_H
#define RPCRT4_H

#include "wintypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef ULONG RPC_STATUS;
typedef LPCSTR RPC_CSTR;

typedef struct _GUID {
  ULONG Data1;
  USHORT Data2;
  USHORT Data3;
  BYTE Data4[8];
} GUID;

typedef GUID CLSID;
typedef GUID UUID;


WIN32_API RPC_STATUS rpcrt4_UuidFromString(
    RPC_CSTR StringUuid,
    UUID     *Uuid
);


#ifdef __cplusplus
}

inline bool operator==(const GUID& g1, const GUID& g2) noexcept {
  bool b1 = (g1.Data1 == g2.Data1) && (g1.Data2 == g2.Data2) && (g1.Data3 == g2.Data3);
  if(!b1)
    return false;

  for(uint32_t ii = 0; ii < 8; ++ii) {
    if(g1.Data4[ii] != g2.Data4[ii])
      return false;
  }

  return true;
}
#endif

#endif // RPCRT4_H
