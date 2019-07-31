#include "win32.h"

#include <cassert>

#include <algorithm>
#include <vector>

//#include <map>
//
//namespace {
//    std::map<void*, HANDLE> s_PtrToHANDLE = {{nullptr, 0}};
//    std::map<HANDLE, void*> s_HANDLEToPtr = {{0, nullptr}};
//}
//
//template <>
//HANDLE newHANDLE(void* ptr) {
//    auto it = s_PtrToHANDLE.find(ptr);
//
//    if(it == s_PtrToHANDLE.end()) {
//        auto handle = s_PtrToHANDLE.crbegin()->second + 1;
//        s_PtrToHANDLE[ptr] = handle;
//        s_HANDLEToPtr[handle] = ptr;
//
//        return handle;
//    }
//
//    return it->second;
//}
//
//template <>
//void* fromHANDLE(HANDLE handle) {
//    return s_HANDLEToPtr.at(handle);
//}

namespace {
    std::vector<void*> s_HandledPtrs = {nullptr};
}

template <>
HANDLE newHANDLE(void* ptr) {
    auto it = std::find(s_HandledPtrs.begin(), s_HandledPtrs.end(), ptr);

    if(it == s_HandledPtrs.end()) {
        s_HandledPtrs.push_back(ptr);
        return s_HandledPtrs.size()-1;
    }

    return std::distance(s_HandledPtrs.begin(), it);
}

template <>
void* fromHANDLE(HANDLE handle) {
    return s_HandledPtrs[handle];
}