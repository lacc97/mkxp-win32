#include "rgssad.hpp"

#include <cassert>

#include <filesystem>
#include <fstream>
#include <regex>
#include <string_view>

#include "common/log.h"

namespace fs = std::filesystem;

using namespace std::string_view_literals;

/*
 * https://github.com/luxrck/rgssad/blob/master/src/main.rs
 */

constexpr std::string_view RGSSAD_VERSION = "0.1.4"sv;

inline uint32_t advanceMagic(uint32_t& magic) {
    uint32_t old = magic;

    magic *= 7;
    magic += 3;

    return old;
}

inline bool ru32(std::istream& is, uint32_t& buf) {
    auto buff = reinterpret_cast<uint8_t*>(&buf);
    is.read(reinterpret_cast<char*>(buff), sizeof(buf));

    buf = ((uint32_t(buff[0]) << 0x00) & 0x000000FF) |
          ((uint32_t(buff[1]) << 0x08) & 0x0000FF00) |
          ((uint32_t(buff[2]) << 0x10) & 0x00FF0000) |
          ((uint32_t(buff[3]) << 0x18) & 0xFF000000);

    return bool(is);
}

inline bool wu32(std::ostream& os, uint32_t data) {
    uint8_t buf[4];

    buf[0] = uint8_t((data & 0x000000FF) >> 0x00);
    buf[1] = uint8_t((data & 0x0000FF00) >> 0x08);
    buf[2] = uint8_t((data & 0x00FF0000) >> 0x10);
    buf[3] = uint8_t((data & 0xFF000000) >> 0x18);

    os.write(reinterpret_cast<char*>(buf), 0);

    return bool(os);
}

RGSS::Archive::Entry::Entry(uint32_t size, uint32_t offs, uint32_t mag)
    : m_Size(size),
      m_Offset(offs),
      m_Magic(mag),
      m_Data() {
}

void RGSS::Archive::Entry::read(std::istream& is) {
    uint32_t myMagic = m_Magic, myOffset = 0;

    m_Data.resize(m_Size);

    is.seekg(m_Offset);
    is.read(reinterpret_cast<char*>(m_Data.data()), m_Data.size());

    auto dat = reinterpret_cast<uint32_t*>(m_Data.data());
    for(uint32_t ii = 0; ii < (m_Data.size() / 4); ii++)
        dat[ii] ^= advanceMagic(myMagic);

    myOffset += 4 * (m_Data.size() / 4);

    for(uint32_t ii = 0; ii < (m_Data.size() - myOffset); ii++)
        m_Data[myOffset + ii] ^= uint8_t((myMagic >> (ii * 8)) & 0xff);
}

RGSS::Archive::Archive()
    : m_Magic{0},
      m_Version{0},
      m_Entries{},
      mp_IStream{nullptr} {
}

bool RGSS::Archive::open(const std::string& loc) {
    delete mp_IStream;

    m_Entries.clear();
    mp_IStream = new std::ifstream(loc);

    if(!(*mp_IStream)) {
        spdlog::error("Failed to open file '{}'", loc);
        return false;
    }

    uint8_t header[8];
    mp_IStream->read(reinterpret_cast<char*>(header), 8);

    std::string fileT(reinterpret_cast<char*>(header), 6);

    if(fileT != "RGSSAD") {
        spdlog::error("File '{}' is not a RGSS archive", loc);
        return false;
    }

    switch(m_Version = header[7]) {
        case 1:
        case 2:
            return openRGSSAD(*mp_IStream);

        case 3:
            return openRGSS3A(*mp_IStream);

        default:
            spdlog::error("Unrecognised version number {}", m_Version);
            return false;
    }
}

bool RGSS::Archive::openRGSSAD(std::istream& is) {
    m_Magic = 0xDEADCAFE;

    while(true) {
        uint32_t eNameLen{0};

        if(!ru32(is, eNameLen))
            break;

        eNameLen ^= advanceMagic(m_Magic);

        std::string eName(eNameLen, 0);
        is.read(eName.data(), eName.length());

        for(char& c : eName) {
            (*reinterpret_cast<uint8_t*>(&c)) ^= static_cast<uint8_t>(advanceMagic(m_Magic) & 0xff);

            if(c == '\\')
                c = '/';
        }

        uint32_t eSize, eOffset, eMagic;

        ru32(is, eSize);
        eSize ^= advanceMagic(m_Magic);
        eOffset = uint32_t(is.tellg());
        eMagic = m_Magic;

        if(m_Entries.find(eName) != m_Entries.end()) {
            spdlog::warn("Entry '{}' already exists. Skipping...", eName);
            is.seekg(eSize, std::ios::cur);
            continue;
        }

        m_Entries.try_emplace(eName, eSize, eOffset, eMagic);

        is.seekg(eSize, std::ios::cur);
    }

    return true;
}

bool RGSS::Archive::openRGSS3A(std::istream& is) {
    m_Magic = 0;

    if(!ru32(is, m_Magic)) {
        spdlog::error("Failed to read magic number");
    }

    m_Magic *= 9;
    m_Magic += 3;

    while(true) {
        uint32_t eOffset{0}, eSize{0}, eStartMagic{0}, eNameLen{0};

        if(!ru32(is, eOffset))
            break;

        eOffset ^= m_Magic;

        if(eOffset == 0)
            break;

        if(!ru32(is, eSize))
            break;

        eSize ^= m_Magic;

        if(!ru32(is, eStartMagic))
            break;

        eStartMagic ^= m_Magic;

        if(!ru32(is, eNameLen))
            break;

        eNameLen ^= m_Magic;

        std::string eName(eNameLen, 0);
        is.read(eName.data(), eName.length());

        for(uint32_t ii = 0; ii < eName.length(); ii++) {
            (*reinterpret_cast<uint8_t*>(eName.data() + ii)) ^= static_cast<uint8_t>(((m_Magic >> (8 * (ii % 4))) &
                                                                                      0xff));

            if(eName[ii] == '\\')
                eName[ii] = '/';
        }

        if(m_Entries.find(eName) != m_Entries.end()) {
            spdlog::warn("Entry '{}' already exists. Skipping...", eName);
            continue;
        }

        m_Entries.try_emplace(eName, eSize, eOffset, eStartMagic);
    }

    return true;
}

void printUsage() {
    std::cout << "Extract rgssad/rgss2a/rgss3a files." << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "    help" << std::endl;
    std::cout << "    version" << std::endl;
    std::cout << "    list      <filename>" << std::endl;
    std::cout << "    unpack    <filename> <location> [<filter>]" << std::endl;
}

void list(const RGSS::Archive& arc) {
    for(const auto& e : arc.entries())
        std::cout << fmt::format("{}: Entry {{size: {}, offset: {}, magic: {}}}\n", e.first, e.second.size(),
                                 e.second.offset(), e.second.magic());
    std::cout.flush();
}

void unpack(RGSS::Archive& arc, const std::string& dir, const std::string& filter) {
    auto fn_create = [](const std::string& loc) -> std::ofstream {
        fs::path file(loc);
        fs::create_directories(file.parent_path());
        return std::ofstream(file);
    };

    std::regex filt{filter};

    for(auto& e : arc.entries()) {
        if(!std::regex_match(e.first, filt))
            continue;

        std::cout << fmt::format("Extracting '{}'", e.first) << std::endl;

        RGSS::Archive::Entry& entry = arc[e.first];

        std::string fName = (dir + "/" + e.first);
        std::ofstream fos = fn_create(fName);

        if(!fos) {
            spdlog::error("Failed to open file '{}'", fName);
            entry.clear();
            return;
        }

        fos.write(reinterpret_cast<const char*>(entry.data().data()), entry.data().size());
        entry.clear();
    }
}

int main(int argc, char* argv[]) {
    const std::map<std::string, std::function<void(void)>> optionMap = {
        {
            "help",
            [=]() {
                printUsage();
            }
        },
        {
            "version",
            [=]() {
                assert(argc == 2);
                std::cout << "Version: " << RGSSAD_VERSION << std::endl;
            }
        },
        {
            "list",
            [=]() {
                assert(argc == 3);

                RGSS::Archive arc;

                if(!arc.open(argv[2])) {
                    spdlog::error("Failed to read archive '{}'", argv[2]);
                    return;
                }

                list(arc);
            }
        },
        {
            "unpack",
            [=]() {
                assert(argc > 3 && argc < 6);

                RGSS::Archive arc;

                if(!arc.open(argv[2])) {
                    spdlog::error("Failed to read archive '{}'", argv[2]);
                    return;
                }

                unpack(arc, argv[3], argc == 5 ? argv[4] : ".*");
            }
        }
    };

    if(argc >= 2) {
        auto it = optionMap.find(argv[1]);

        if(it != optionMap.end()) {
            it->second();
            return 0;
        }
    }

    printUsage();
    return 0;
}
