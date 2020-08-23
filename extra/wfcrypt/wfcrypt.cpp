#include "wfcrypt.h"

#include <cstring>

#include <algorithm>
#include <random>

#include <buffer/buffer_span.hpp>
#include <buffer/buffer_view.hpp>
#include <buffer/static_buffer.hpp>

#include <kernel32.h>

#include "rijndael.hpp"

namespace wflib {
  namespace {
    enum class Error : DWORD {
      eInvalidMemoryRead  = 0x24000001,
      eWrongNB            = 0x24400002,
      eWrongNK            = 0x24400004,
      eWrongKeyLength     = 0x24400008,
      eInvalidMemoryWrite = 0x24000010,
    };

    inline int error(Error err) noexcept {
      kernel32_SetLastError((DWORD) err);
      return false;
    }

    inline uint32_t random(uint32_t max = UINT32_MAX) {
      thread_local std::mt19937 engine{std::random_device{}()};
      return std::uniform_int_distribution<uint32_t>{0, max}(engine);
    }
  }    // namespace
}    // namespace wflib

int wfcrypt_encrypt(char* plainText, int nb, int nk, char* iv, const char* key, int plainTextLength) {
  using namespace wflib;

  auto fn_is_valid_n = [](uint32_t n) -> bool {
    return (n == 4 || n == 6 || n == 8);
  };

  if(!fn_is_valid_n(nb))
    return error(Error::eWrongNB);
  if(!fn_is_valid_n(nk))
    return error(Error::eWrongNK);
  if(!plainTextLength)
    return error(Error::eInvalidMemoryRead);

  if(plainTextLength % (sizeof(uint32_t) * nb) != 0)
    return error(Error::eInvalidMemoryRead);

  for(uint32_t ii = 0; ii < uint32_t(nb); ++ii) {
    uint32_t rv = wflib::random();
    memcpy(iv + sizeof(uint32_t) * ii, &rv, sizeof(uint32_t));
  }

  buffer::static_byte_buffer<32> ivBuf;
  rijndael::Context ctx{static_cast<uint32_t>(nb), buffer::byte_buffer_view{key, sizeof(uint32_t) * nk}};
  {
    const buffer::byte_buffer_span initVector = buffer::to_byte_span(ivBuf);
    buffer::byte_buffer_view{iv, ctx.blockSize()}.copy_to(initVector);

    buffer::byte_buffer_span text = buffer::byte_buffer_span{plainText, static_cast<uint32_t>(plainTextLength)};
    while(!text.empty()) {
      buffer::byte_buffer_span block = text.take_first(sizeof(uint32_t) * nb);

      std::transform(block.begin(), block.end(), initVector.begin(), block.begin(),
                     [](uint8_t b, uint8_t v) -> uint8_t { return b ^ v; });

      ctx.encrypt(block);

      block.copy_to(initVector);
    }
  }

  kernel32_SetLastError(0);
  return true;
}
int wfcrypt_decrypt(char* cipherText, int nb, int nk, const char* iv, const char* key, int cipherTextLength) {
  using namespace wflib;

  auto fn_is_valid_n = [](uint32_t n) -> bool {
    return (n == 4 || n == 6 || n == 8);
  };

  if(!fn_is_valid_n(nb))
    return error(Error::eWrongNB);
  if(!fn_is_valid_n(nk))
    return error(Error::eWrongNK);
  if(!cipherTextLength)
    return error(Error::eInvalidMemoryRead);

  if(cipherTextLength % (sizeof(uint32_t) * nb) != 0)
    return error(Error::eInvalidMemoryRead);

  buffer::static_byte_buffer<32> ivBuf1;
  buffer::static_byte_buffer<32> ivBuf2;
  rijndael::Context ctx{static_cast<uint32_t>(nb), buffer::byte_buffer_view{key, sizeof(uint32_t) * nk}};
  {
    buffer::byte_buffer_span initVector = buffer::to_byte_span(ivBuf1);
    buffer::byte_buffer_view{iv, ctx.blockSize()}.copy_to(initVector);

    buffer::byte_buffer_span nextInitVector = buffer::to_byte_span(ivBuf2);

    buffer::byte_buffer_span text = buffer::byte_buffer_span{cipherText, static_cast<uint32_t>(cipherTextLength)};
    while(!text.empty()) {
      buffer::byte_buffer_span block = text.take_first(ctx.blockSize());
      block.copy_to(buffer::to_byte_span(nextInitVector));

      ctx.decrypt(block);

      std::transform(block.begin(), block.end(), initVector.begin(), block.begin(),
                     [](uint8_t b, uint8_t v) -> uint8_t { return b ^ v; });

      std::swap(initVector, nextInitVector);
    }
  }

  kernel32_SetLastError(0);
  return true;
}
