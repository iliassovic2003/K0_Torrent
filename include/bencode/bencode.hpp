#pragma once

# include <string>
# include <vector>
# include <map>
# include <variant>
# include <stdexcept>
# include <cstdint>

class BencodeValue {
    public:
        using Integer = int64_t;
        using String  = std::string;
        using List    = std::vector<BencodeValue>;
        using Dict    = std::map<std::string, BencodeValue>;

    std::variant<Integer, String, List, Dict> data;

    /*───────────────────────────────────────────────────*/
    bool is_int()    const {
        return std::holds_alternative<Integer>(data);
    }

    bool is_string() const {
        return std::holds_alternative<String>(data);
    }

    bool is_list()   const {
        return std::holds_alternative<List>(data);
    }

    bool is_dict()   const {
        return std::holds_alternative<Dict>(data);
    }
    
    /*───────────────────────────────────────────────────*/
    Integer&     as_int()    {
        return std::get<Integer>(data);
    }

    String&      as_string() {
        return std::get<String>(data);
    }

    List&        as_list()   {
        return std::get<List>(data);
    }

    Dict&        as_dict()   {
        return std::get<Dict>(data);
    }

    /*───────────────────────────────────────────────────*/
    const Integer& as_int()    const {
        return std::get<Integer>(data);
    }

    const String&  as_string() const {
        return std::get<String>(data);
    }

    const List&    as_list()   const {
        return std::get<List>(data);
    }
    
    const Dict&    as_dict()   const {
        return std::get<Dict>(data);
    }
};

BencodeValue decode(const std::string& data, size_t& pos);