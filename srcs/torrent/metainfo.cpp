#include "../../include/torrent/metainfo.hpp"
#include "../../include/bencode/bencode.hpp"
#include "../../include/crypto/sha1.hpp"

static std::string                          read_file(const std::string& path) {

    std::ifstream file(path, std::ios::binary);
    if (!file.is_open())
        throw MetainfoError("cannot open file: " + path);

    std::ostringstream ss;
    ss << file.rdbuf();
    
    return ss.str();
}

static size_t find_info_start(const std::string& raw) {
    const std::string key = "4:info";
    size_t pos = raw.find(key);

    if (pos == std::string::npos)
        throw MetainfoError("missing info dict");
    return pos + key.size();
}

static std::vector<std::array<uint8_t, 20>> split_pieces(const std::string& raw) {
    
    if (raw.size() % 20 != 0)
        throw MetainfoError("pieces length is not a multiple of 20");

    std::vector<std::array<uint8_t, 20>> pieces;

    for (size_t i = 0; i < raw.size(); i += 20) {
        std::array<uint8_t, 20> hash;
        std::memcpy(hash.data(), raw.data() + i, 20);

        pieces.push_back(hash);
    }

    return pieces;
}

static std::vector<FileInfo>                parse_files(const BencodeValue::List& list) {
    std::vector<FileInfo> files;

    for (const auto& entry : list) {
        const auto& d = entry.as_dict();
        FileInfo    fi;
        fi.length = d.at("length").as_int();

        for (const auto& p : d.at("path").as_list())
            fi.path.push_back(p.as_string());
        files.push_back(fi);
    }

    return files;
}

static std::vector<std::vector<std::string>> parse_announce_list(const BencodeValue::List& list) {
    std::vector<std::vector<std::string>> result;
    
    for (const auto& tier : list) {
        std::vector<std::string> t;

        for (const auto& url : tier.as_list())
            t.push_back(url.as_string());

        result.push_back(t);
    }
    return result;
}

TorrentFile                                 parse_torrent(const std::string& path) {
    TorrentFile tf;

    std::string raw = read_file(path);

    size_t pos = 0;
    auto root = decode(raw, pos);
    auto& root_dict = root.as_dict();

    size_t info_start = find_info_start(raw);
    size_t info_pos   = info_start;
    decode(raw, info_pos);
    size_t info_end = info_pos;

    std::string raw_info = raw.substr(info_start, info_end - info_start);
    tf.info_hash = sha1_raw(raw_info);

    if (root_dict.count("announce"))
        tf.announce = root_dict.at("announce").as_string();

    if (root_dict.count("announce-list"))
        tf.announce_list = parse_announce_list(root_dict.at("announce-list").as_list());

    if (root_dict.count("comment"))
        tf.comment = root_dict.at("comment").as_string();

    if (root_dict.count("created by"))
        tf.created_by = root_dict.at("created by").as_string();

    if (root_dict.count("creation date"))
        tf.creation_date = root_dict.at("creation date").as_int();

    auto& info = root_dict.at("info").as_dict();

    if (!info.count("name"))
        throw MetainfoError("missing name field");
    tf.name = info.at("name").as_string();

    if (!info.count("piece length"))
        throw MetainfoError("missing piece length field");
    tf.piece_length = info.at("piece length").as_int();

    if (!info.count("pieces"))
        throw MetainfoError("missing pieces field");
    tf.pieces = split_pieces(info.at("pieces").as_string());

    if (info.count("files")) {
        tf.files  = parse_files(info.at("files").as_list());
        tf.length = 0;
    } else {
        if (!info.count("length"))
            throw MetainfoError("missing length field in single file torrent");
        tf.length = info.at("length").as_int();
    }

    return tf;
}