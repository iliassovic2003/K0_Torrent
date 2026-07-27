#pragma once

# include <iostream>
# include <iomanip>
# include <sstream>
# include <openssl/sha.h>
# include <string>
# include <cstring>
# include <array>
# include <cstdint>
# include <vector>
# include <cstddef>

std::string                 sha1_hex(const std::string& input);
std::array<uint8_t, 20>     sha1_raw(const std::string& input);
