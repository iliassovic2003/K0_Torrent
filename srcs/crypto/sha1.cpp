#include "../../include/crypto/sha1.hpp"

std::array<uint8_t, 20>     sha1_raw(const std::string& input) {

    std::array<uint8_t, 20>  result;

    SHA1(
        reinterpret_cast<const uint8_t *>(input.data()),
        input.size(),
        result.data()
    );

    return result;
}

std::string                 sha1_hex(const std::string& input) {
    
    std::ostringstream          ss;
    std::array<uint8_t, 20>     result = sha1_raw(input);

    for (uint8_t byte : result)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;

    return ss.str();
}