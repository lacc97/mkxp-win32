#include "rijndael.hpp"

#if defined(_WIN32)
#  include <windows.h>
#elif defined(unix) || defined(__unix__) || defined(__unix)
#  include <strings.h>
#endif

#include <algorithm>
#include <array>

#include <buffer/static_buffer.hpp>

#include "utils.h"


#if defined(NDEBUG)
#  if defined(_MSC_VER)
#    define ASSUME(x) __assume(x)
#  elif defined(__GNUC__)
#    define ASSUME(x)                                                                                                  \
      do {                                                                                                             \
        if(!(x))                                                                                                       \
          __builtin_unreachable();                                                                                     \
      } while(0)
#  else
#    define ASSUME(x)
#  endif
#else
#  define ASSUME(x) assert(x)
#endif


namespace {
  inline void secure_zero(void* p, size_t l) noexcept {
#if defined(_WIN32)
    SecureZeroMemory(p, l);
#elif defined(unix) || defined(__unix__) || defined(__unix)
    explicit_bzero(p, l);
#endif
  }
}    // namespace

namespace rijndael {
  namespace {
    namespace checks {
      constexpr inline bool is_valid_block_size(uint32_t nb) noexcept {
        return (nb == 4 || nb == 6 || nb == 8);
      }
    }    // namespace checks

    namespace tables {
      constexpr std::array<uint8_t, 256> k_SBox = {
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82,
        0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26,
        0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96,
        0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
        0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb,
        0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f,
        0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff,
        0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32,
        0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d,
        0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6,
        0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
        0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e,
        0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f,
        0xb0, 0x54, 0xbb, 0x16};

      constexpr std::array<uint8_t, 256> k_RSBox = {
        0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb, 0x7c, 0xe3,
        0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb, 0x54, 0x7b, 0x94, 0x32,
        0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e, 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9,
        0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25, 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16,
        0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92, 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15,
        0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84, 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05,
        0xb8, 0xb3, 0x45, 0x06, 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13,
        0x8a, 0x6b, 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
        0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e, 0x47, 0xf1,
        0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b, 0xfc, 0x56, 0x3e, 0x4b,
        0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4, 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07,
        0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f, 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d,
        0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef, 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb,
        0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61, 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63,
        0x55, 0x21, 0x0c, 0x7d};

      constexpr std::array<uint8_t, 16 * 256> generate_galois_multiplication_table() noexcept {
        auto fn_galois_mult = [](uint8_t a, uint8_t b) -> uint8_t {
          uint8_t p = 0;
          for(uint32_t counter = 0; counter < 8; counter++) {
            if((b & 1u) == 1u)
              p ^= a;

            bool hi_bit_set = (a & 0x80u) == 0x80u;
            a <<= 1u;
            if(hi_bit_set)
              a ^= 0x1bu;
            b >>= 1u;
          }
          return p;
        };

        std::array<uint8_t, 16 * 256> tab{};
        for(uint32_t ii = 0; ii < 16; ++ii) {
          for(uint32_t jj = 0; jj < 256; ++jj) {
            tab[ii * 256 + jj] = fn_galois_mult(jj, ii);
          }
        }

        return tab;
      }

      constexpr std::array<uint8_t, 16 * 256> k_GaloisMultiplication = generate_galois_multiplication_table();


      constexpr std::array<uint8_t, 256> generate_rcon_table() noexcept {
        std::array<uint8_t, 256> tab{};
        uint8_t prod = 1;
        for(uint32_t ii = 1; ii < 256; ++ii) {
          tab[ii] = prod;
          prod    = k_GaloisMultiplication[2 * 256 + prod];
        }
        return tab;
      }

      constexpr std::array<uint8_t, 256> k_RCon = generate_rcon_table();
    }    // namespace tables

    void key_expand(const buffer::byte_buffer_span expanded, const buffer::byte_buffer_view key) noexcept {
      constexpr auto fn_rotate = [](buffer::byte_buffer_span word) noexcept {
        uint8_t a = word.first();
        for(uint32_t ii = 0; ii < word.size() - 1; ii++)
          word[ii] = word[ii + 1];
        word.last() = a;
      };

      constexpr auto fn_subst = [](buffer::byte_buffer_span word) noexcept {
        std::transform(word.begin(), word.end(), word.begin(), [](uint8_t b) -> uint8_t { return tables::k_SBox[b]; });
      };

      ASSUME(key.size() % sizeof(uint32_t) == 0);

      uint32_t nk = key.size() / sizeof(uint32_t);

      ASSUME(nk == 4 || nk == 6 || nk == 8);

      key.copy_to(expanded);

      for(uint32_t ii = sizeof(uint32_t) * nk, ri = 0; ii < expanded.size(); ii += sizeof(uint32_t)) {
        buffer::static_byte_buffer<sizeof(uint32_t)> word;
        buffer::byte_buffer_span t = buffer::to_byte_span(word);

        t.copy_from(expanded.subview(ii - sizeof(uint32_t), sizeof(uint32_t)));

        if(ii % (sizeof(uint32_t) * nk) == 0) {
          fn_rotate(t);
          fn_subst(t);
          t[0] ^= tables::k_RCon[++ri];
        }

        if(nk == 8 && (ii % nk) == 4)
          fn_subst(t);

        for(uint32_t jj = 0; jj < sizeof(uint32_t); ++jj)
          expanded[ii + jj] = expanded[ii - sizeof(uint32_t) * nk + jj] ^ t[jj];
      }
    }

    void add_round_key(const buffer::byte_buffer_span state, const buffer::byte_buffer_view roundKey) noexcept {
      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));
      ASSUME(roundKey.size() == state.size());

      std::transform(state.begin(), state.end(), roundKey.begin(), state.begin(),
                     [](uint8_t a, uint8_t k) -> uint8_t { return a ^ k; });
    }

    void mix_columns(const buffer::byte_buffer_span state) noexcept {
      constexpr auto fn_galois_mult = [](uint8_t a, uint8_t b) -> uint8_t {
        ASSUME(b < 16);

        return tables::k_GaloisMultiplication[b * 256 + a];
      };

      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));

      for(uint32_t ii = 0; ii < state.size() / sizeof(uint32_t); ++ii) {
        buffer::byte_buffer_span a = state.subspan(sizeof(uint32_t) * ii, sizeof(uint32_t));
        buffer::static_byte_buffer<32> b;

        b[0] = fn_galois_mult(a[0], 2) ^ fn_galois_mult(a[3], 1) ^ fn_galois_mult(a[2], 1) ^ fn_galois_mult(a[1], 3);
        b[1] = fn_galois_mult(a[1], 2) ^ fn_galois_mult(a[0], 1) ^ fn_galois_mult(a[3], 1) ^ fn_galois_mult(a[2], 3);
        b[2] = fn_galois_mult(a[2], 2) ^ fn_galois_mult(a[1], 1) ^ fn_galois_mult(a[0], 1) ^ fn_galois_mult(a[3], 3);
        b[3] = fn_galois_mult(a[3], 2) ^ fn_galois_mult(a[2], 1) ^ fn_galois_mult(a[1], 1) ^ fn_galois_mult(a[0], 3);

        buffer::to_byte_view(b).copy_to(a);
      }
    }

    void inv_mix_columns(const buffer::byte_buffer_span state) noexcept {
      constexpr auto fn_galois_mult = [](uint8_t a, uint8_t b) -> uint8_t {
        ASSUME(b < 16);

        return tables::k_GaloisMultiplication[b * 256 + a];
      };

      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));

      for(uint32_t ii = 0; ii < state.size() / sizeof(uint32_t); ++ii) {
        buffer::byte_buffer_span a = state.subspan(sizeof(uint32_t) * ii, sizeof(uint32_t));
        buffer::static_byte_buffer<32> b;

        b[0] = fn_galois_mult(a[0], 14) ^ fn_galois_mult(a[3], 9) ^ fn_galois_mult(a[2], 13) ^ fn_galois_mult(a[1], 11);
        b[1] = fn_galois_mult(a[1], 14) ^ fn_galois_mult(a[0], 9) ^ fn_galois_mult(a[3], 13) ^ fn_galois_mult(a[2], 11);
        b[2] = fn_galois_mult(a[2], 14) ^ fn_galois_mult(a[1], 9) ^ fn_galois_mult(a[0], 13) ^ fn_galois_mult(a[3], 11);
        b[3] = fn_galois_mult(a[3], 14) ^ fn_galois_mult(a[2], 9) ^ fn_galois_mult(a[1], 13) ^ fn_galois_mult(a[0], 11);

        buffer::to_byte_view(b).copy_to(a);
      }
    }

    void shift_rows(const buffer::byte_buffer_span state) noexcept {
      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));

      const uint32_t nb = state.size() / sizeof(uint32_t);

      auto fn_shift_row = [nb](buffer::byte_buffer_span state, uint32_t row, uint32_t count) {
        for(uint32_t ii = 0; ii < count; ++ii) {
          uint8_t tmp = state[row];
          for(uint32_t jj = 0; jj < nb - 1; ++jj)
            state[sizeof(uint32_t) * jj + row] = state[sizeof(uint32_t) * (jj + 1) + row];
          state[sizeof(uint32_t) * (nb - 1) + row] = tmp;
        }
      };

      fn_shift_row(state, 1, 1);
      fn_shift_row(state, 2, (nb < 8 ? 2 : 3));
      fn_shift_row(state, 3, (nb < 8 ? 3 : 4));
    }

    void inv_shift_rows(const buffer::byte_buffer_span state) noexcept {
      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));

      const uint32_t nb = state.size() / sizeof(uint32_t);

      auto fn_shift_row = [nb](buffer::byte_buffer_span state, uint32_t row, uint32_t count) {
        for(uint32_t ii = 0; ii < count; ++ii) {
          uint8_t tmp = state[sizeof(uint32_t) * (nb - 1) + row];
          for(uint32_t jj = nb - 1; jj > 0; --jj)
            state[sizeof(uint32_t) * jj + row] = state[sizeof(uint32_t) * (jj - 1) + row];
          state[row] = tmp;
        }
      };

      fn_shift_row(state, 1, 1);
      fn_shift_row(state, 2, (nb < 8 ? 2 : 3));
      fn_shift_row(state, 3, (nb < 8 ? 3 : 4));
    }

    void sub_bytes(const buffer::byte_buffer_span state) noexcept {
      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));

      std::transform(state.begin(), state.end(), state.begin(), [](uint8_t b) { return tables::k_SBox[b]; });
    }

    void inv_sub_bytes(const buffer::byte_buffer_span state) noexcept {
      ASSUME(state.size() % sizeof(uint32_t) == 0 && checks::is_valid_block_size(state.size() / sizeof(uint32_t)));

      std::transform(state.begin(), state.end(), state.begin(), [](uint8_t b) { return tables::k_RSBox[b]; });
    }
  }    // namespace
}    // namespace rijndael

rijndael::Context::Context(uint32_t nb, buffer::byte_buffer_view key) noexcept
    : m_Nb{nb},
      m_Nk{static_cast<uint32_t>(key.size() / sizeof(uint32_t))},
      m_Nr{std::max(m_Nb, m_Nk) + 6},
      m_ExpandedKey{} {
  ASSUME(m_Nb == 4 || m_Nb == 6 || m_Nb == 8);
  ASSUME(key.size() % sizeof(uint32_t) == 0);
  ASSUME(m_Nk == 4 || m_Nk == 6 || m_Nb == 8);

  key_expand(buffer::to_byte_span(m_ExpandedKey).first(expandedKeySize()), key);
}

rijndael::Context::~Context() {
  secure_zero(m_ExpandedKey, sizeof(m_ExpandedKey));
}

void rijndael::Context::decrypt(buffer::byte_buffer_span block) const noexcept {
  ASSUME(block.size() == blockSize());

  const buffer::byte_buffer_view expandedKey = buffer::to_byte_view(m_ExpandedKey);

  buffer::static_buffer<uint8_t, 32> stateBuf;
  const buffer::byte_buffer_span state = buffer::to_byte_span(stateBuf).first(blockSize());

  block.copy_to(buffer::to_byte_span(state));
  add_round_key(state, expandedKey.subspan(blockSize() * m_Nr, blockSize()));

  for(uint32_t ii = m_Nr - 1; ii > 0; --ii) {
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, expandedKey.subspan(blockSize() * ii, blockSize()));
    inv_mix_columns(state);
  }

  inv_shift_rows(state);
  inv_sub_bytes(state);
  add_round_key(state, expandedKey.subspan(0, blockSize()));

  buffer::to_byte_span(state).copy_to(block);
}

void rijndael::Context::encrypt(buffer::byte_buffer_span block) const noexcept {
  ASSUME(block.size() == blockSize());

  const buffer::byte_buffer_view expandedKey = buffer::to_byte_view(m_ExpandedKey);

  buffer::static_byte_buffer<32> stateBuf;
  const buffer::byte_buffer_span state = buffer::to_byte_span(stateBuf).first(blockSize());

  block.copy_to(state);
  add_round_key(state, expandedKey.subspan(0, blockSize()));

  for(uint32_t ii = 1; ii < m_Nr; ++ii) {
    sub_bytes(state);
    shift_rows(state);
    mix_columns(state);
    add_round_key(state, expandedKey.subspan(blockSize() * ii, blockSize()));
  }

  sub_bytes(state);
  shift_rows(state);
  add_round_key(state, expandedKey.subspan(blockSize() * m_Nr, blockSize()));

  buffer::to_byte_span(state).copy_to(block);
}

//void print_expanded_key(const buffer::byte_buffer_span exp, uint32_t nb) noexcept {
//  for(uint32_t ii = 0; ii < exp.size(); ii += sizeof(uint32_t) * nb) {
//    for(uint32_t jj = 0; jj < sizeof(uint32_t) * nb; ++jj)
//      printf("%02x ", exp[ii + jj]);
//    printf("\n");
//  }
//}
//
//void print_state(const buffer::byte_buffer_span state) noexcept {
//  for(uint32_t ii = 0; ii < sizeof(uint32_t); ++ii) {
//    for(uint32_t jj = 0; jj < state.size() / sizeof(uint32_t); ++jj)
//      printf("%02x ", state[jj * sizeof(uint32_t) + ii]);
//    printf("\n");
//  }
//}
//
//int main() {
////    uint32_t t[] = {0x455313db, 0x5c220af2, 0xc6c6c6c6, 0xd5d4d4d4};
////    print_state(buffer::to_span<uint32_t>(t));
////    rijndael::mix_columns(buffer::to_span<uint32_t>(t));
////    printf("\n");
////    print_state(buffer::to_span<uint32_t>(t));
//
////  uint8_t key[16]  = {0};
////  uint8_t exp[176] = {0};
////  rijndael::key_expand(buffer::to_byte_span(exp), buffer::to_byte_span(key));
////  print_expanded_key(buffer::to_byte_span(exp), 4);
//}