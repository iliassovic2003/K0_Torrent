#include "../../include/bencode/bencode.hpp"

static BencodeValue parse(const std::string& data, size_t& pos) {

    if (pos >= data.size())
        throw std::runtime_error("unexpected end of data");

    char c = data[pos];

    if (c == 'i') {
        pos++;

        size_t end = data.find('e', pos);
        if (end == std::string::npos)
            throw std::runtime_error("unterminated integer");

        int64_t val = std::stoll(data.substr(pos, end - pos));
        pos = end + 1;

        return BencodeValue{ val };
    }

    if (std::isdigit((unsigned char)c)) {

        size_t colon = data.find(':', pos);
        if (colon == std::string::npos)
            throw std::runtime_error("malformed string: no colon");

        size_t len = std::stoull(data.substr(pos, colon - pos));
        pos = colon + 1;
        if (pos + len > data.size())
            throw std::runtime_error("string length out of bounds");

        std::string str = data.substr(pos, len);
        pos += len;

        return BencodeValue{ str };
    }

    if (c == 'l') {
        pos++;

        BencodeValue::List list;
        while (pos < data.size() && data[pos] != 'e')
            list.push_back(parse(data, pos));

        if (pos >= data.size())
            throw std::runtime_error("unterminated list");
        pos++;

        return BencodeValue{ list };
    }

    if (c == 'd') {
        pos++;

        BencodeValue::Dict dict;
        while (pos < data.size() && data[pos] != 'e') {
            auto key = parse(data, pos);
            if (!key.is_string())
                throw std::runtime_error("dict key must be a string");
            auto val = parse(data, pos);
            dict[key.as_string()] = val;
        }

        if (pos >= data.size())
            throw std::runtime_error("unterminated dict");
        pos++;

        return BencodeValue{ dict };
    }

    throw std::runtime_error(std::string("unknown bencode type: ") + c);
}

BencodeValue decode(const std::string& data, size_t& pos) {
    return parse(data, pos);
}