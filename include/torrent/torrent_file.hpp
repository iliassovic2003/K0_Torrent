#pragma once

# include <string>
# include <vector>
# include <array>
# include <cstdint>

struct FileInfo {
    int64_t                                 length;            // size in bytes
    std::vector<std::string>                path;
};

struct TorrentFile {
    std::array<uint8_t, 20>                 info_hash;
    std::string                             name;

    std::string                             announce;         // primary tracker URL
    std::vector<std::vector<std::string>>   announce_list;    // backup trackers

    int64_t                                 piece_length;     // bytes per piece
    std::vector<std::array<uint8_t, 20>>    pieces;           // one hash per piece

    int64_t                                 length;           // set if single file
    std::vector<FileInfo>                   files;            // set if multi file

    std::string                             comment;
    std::string                             created_by;
    int64_t                                 creation_date;

    bool        is_multi_file()     const;
    int64_t     total_size()        const;
    size_t      piece_count()       const;
};