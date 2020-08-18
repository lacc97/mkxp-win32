#ifndef WIN32API_RIJNDAEL_HPP
#define WIN32API_RIJNDAEL_HPP

#include <cstdint>

#include <buffer/buffer_span.hpp>
#include <buffer/buffer_view.hpp>

namespace rijndael {
  class Context {
   public:
    Context(uint32_t nb, buffer::byte_buffer_view key) noexcept;
    ~Context();

    void decrypt(buffer::byte_buffer_span block) const noexcept;
    void encrypt(buffer::byte_buffer_span block) const noexcept;

    [[nodiscard]] uint32_t blockSize() const noexcept {
      return sizeof(uint32_t) * m_Nb;
    }

   private:
    [[nodiscard]] uint32_t expandedKeySize() const noexcept {
      return sizeof(uint32_t) * m_Nb * (m_Nr + 1);
    }


    uint32_t m_Nb, m_Nk, m_Nr;
    uint8_t m_ExpandedKey[480];
  };
}    // namespace rijndael

#endif    //WIN32API_RIJNDAEL_HPP
