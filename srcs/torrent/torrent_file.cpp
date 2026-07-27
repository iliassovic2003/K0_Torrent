#include "../../include/torrent/torrent_file.hpp"

bool    TorrentFile::is_multi_file() const {
    return !files.empty();
}

size_t  TorrentFile::piece_count() const {
    return pieces.size();
}

int64_t TorrentFile::total_size() const {
    if (!is_multi_file())
        return length;

    int64_t total = 0;
    for (const auto& file : files)
        total += file.length;

    return total;
}
