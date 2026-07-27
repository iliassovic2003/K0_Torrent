#pragma once

# include <array>
# include <cstdint>
# include <random>

using       PeerId = std::array<uint8_t, 20>;

PeerId      generate_peer_id();