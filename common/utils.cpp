#include "utils.h"

void String::tokenize(std::vector< std::string >& v, const std::string& s, const std::string& delim) {
    std::size_t last = 0;
    std::size_t next = 0;

    while((next = s.find(delim, last)) != std::string::npos) {
        v.push_back(s.substr(last, next - last));
        last = next + delim.length();
    }

    v.push_back(s.substr(last));
}
