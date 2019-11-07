#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>

namespace String {
    std::string replace_all(std::string_view str, std::string_view pattern, std::string_view replacement);

//    std::vector<std::string_view> tokenize(std::string_view str, std::string_view delim);
}

#endif
