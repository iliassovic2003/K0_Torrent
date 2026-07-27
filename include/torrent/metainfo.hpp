#pragma once

# include "torrent_file.hpp"
# include <string>
# include <fstream>
# include <sstream>
# include <cstring>
# include <stdexcept>

struct MetainfoError : public std::runtime_error {
    explicit MetainfoError(const std::string& msg) : std::runtime_error("metainfo: " + msg) {}
};

TorrentFile         parse_torrent(const std::string& path);