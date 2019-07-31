#ifndef RGSSAD_HPP
#define RGSSAD_HPP

#include <cstdint>

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace RGSS {
    class Archive {
        public:
            class Entry {
                public:
                    explicit Entry(uint32_t size = 0, uint32_t offs = 0, uint32_t mag = 0);

                    void read(std::istream& is);

                    inline void clear() {
                        m_Data.clear();
                        m_Data.shrink_to_fit();
                    }

                    inline uint32_t size() const {
                        return m_Size;
                    }

                    inline uint32_t offset() const {
                        return m_Offset;
                    }

                    inline uint32_t magic() const {
                        return m_Magic;
                    }

                    inline const std::vector<uint8_t>& data() const {
                        return m_Data;
                    }

                private:
                    uint32_t m_Size;
                    uint32_t m_Offset;
                    uint32_t m_Magic;
                    std::vector<uint8_t> m_Data;
            };

        public:
            Archive();

            ~Archive() {
                delete mp_IStream;
            }

            bool open(const std::string& loc);

            inline Entry& operator[](const std::string& key) {
                Entry& e = m_Entries.at(key);

                if(e.data().empty())
                    e.read(*mp_IStream);

                return e;
            }

            inline std::map<std::string, Entry>& entries() {
                return m_Entries;
            }

            inline const std::map<std::string, Entry>& entries() const {
                return m_Entries;
            }

        private:
            bool openRGSSAD(std::istream& is);

            bool openRGSS3A(std::istream& is);

            uint32_t m_Magic;
            uint8_t m_Version;
            std::map<std::string, Entry> m_Entries;
            std::istream* mp_IStream;
    };
}

#endif
