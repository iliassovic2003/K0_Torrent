#pragma once

# include <array>
# include <cstdint>
# include <random>
# include <cstring>

using       PeerId = std::array<uint8_t, 20>;

PeerId      generate_peer_id();