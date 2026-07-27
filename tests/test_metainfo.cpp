#include "../include/torrent/metainfo.hpp"
#include "../include/torrent/torrent_file.hpp"

#include <cassert>
#include <iostream>
#include <fstream>

static void pass(const char* name) {
    std::cout << "  [PASS] " << name << "\n";
}

static void write_test_torrent_single(const std::string& path) {
    std::string pieces(40, '\x01');
    std::string torrent =
        "d"
            "8:announce"
            "30:udp://tracker.example.com:80"
            "4:info"
            "d"
                "6:lengthi1048576e"
                "4:name8:test.iso"
                "12:piece lengthi524288e"
                "6:pieces40:" + pieces +
            "e"
        "e";
    std::ofstream f(path, std::ios::binary);
    f << torrent;
}

static void write_test_torrent_multi(const std::string& path) {
    std::string pieces(20, '\x02');
    std::string torrent =
        "d"
            "8:announce"
            "30:udp://tracker.example.com:80"
            "4:info"
            "d"
                "5:filesl"
                    "d"
                        "6:lengthi512000e"
                        "4:pathl"
                            "6:folder"
                            "8:file.txte"
                    "e"
                    "d"
                        "6:lengthi256000e"
                        "4:pathl"
                            "6:folder"
                            "9:other.txte"
                    "e"
                "e"
                "4:name10:test-multi"
                "12:piece lengthi524288e"
                "6:pieces20:" + pieces +
            "e"
        "e";
    std::ofstream f(path, std::ios::binary);
    f << torrent;
}

static void write_test_torrent_missing_name(const std::string& path) {
    std::string pieces(20, '\x01');
    std::string torrent =
        "d"
            "8:announce"
            "30:udp://tracker.example.com:80"
            "4:info"
            "d"
                "6:lengthi1048576e"
                "12:piece lengthi524288e"
                "6:pieces20:" + pieces +
            "e"
        "e";
    std::ofstream f(path, std::ios::binary);
    f << torrent;
}


static void test_single_file_name() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(tf.name == "test.iso");
    pass("single file name");
}

static void test_single_file_length() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(tf.length == 1048576);
    pass("single file length");
}

static void test_single_file_piece_length() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(tf.piece_length == 524288);
    pass("single file piece length");
}

static void test_single_file_piece_count() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(tf.piece_count() == 2);
    pass("single file piece count");
}

static void test_single_file_announce() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(tf.announce == "udp://tracker.example.com:80");
    pass("single file announce");
}

static void test_single_file_is_not_multi() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(!tf.is_multi_file());
    pass("single file is not multi file");
}

static void test_single_file_total_size() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    assert(tf.total_size() == 1048576);
    pass("single file total size");
}

static void test_single_file_info_hash_not_zero() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    std::array<uint8_t, 20> zero{};
    assert(tf.info_hash != zero);
    pass("single file info hash not zero");
}

static void test_single_file_piece_hash_size() {
    write_test_torrent_single("/tmp/kw_test_single.torrent");
    auto tf = parse_torrent("/tmp/kw_test_single.torrent");
    for (const auto& hash : tf.pieces)
        assert(hash.size() == 20);
    pass("single file piece hash size");
}


static void test_multi_file_is_multi() {
    write_test_torrent_multi("/tmp/kw_test_multi.torrent");
    auto tf = parse_torrent("/tmp/kw_test_multi.torrent");
    assert(tf.is_multi_file());
    pass("multi file is multi file");
}

static void test_multi_file_count() {
    write_test_torrent_multi("/tmp/kw_test_multi.torrent");
    auto tf = parse_torrent("/tmp/kw_test_multi.torrent");
    assert(tf.files.size() == 2);
    pass("multi file count");
}

static void test_multi_file_lengths() {
    write_test_torrent_multi("/tmp/kw_test_multi.torrent");
    auto tf = parse_torrent("/tmp/kw_test_multi.torrent");
    assert(tf.files[0].length == 512000);
    assert(tf.files[1].length == 256000);
    pass("multi file lengths");
}

static void test_multi_file_paths() {
    write_test_torrent_multi("/tmp/kw_test_multi.torrent");
    auto tf = parse_torrent("/tmp/kw_test_multi.torrent");
    assert(tf.files[0].path[0] == "folder");
    assert(tf.files[0].path[1] == "file.txt");
    assert(tf.files[1].path[1] == "other.txt");
    pass("multi file paths");
}

static void test_multi_file_total_size() {
    write_test_torrent_multi("/tmp/kw_test_multi.torrent");
    auto tf = parse_torrent("/tmp/kw_test_multi.torrent");
    assert(tf.total_size() == 768000);
    pass("multi file total size");
}

static void test_multi_file_name() {
    write_test_torrent_multi("/tmp/kw_test_multi.torrent");
    auto tf = parse_torrent("/tmp/kw_test_multi.torrent");
    assert(tf.name == "test-multi");
    pass("multi file name");
}


static void test_missing_file_throws() {
    bool caught = false;
    try { parse_torrent("/tmp/does_not_exist.torrent"); }
    catch (const MetainfoError&) { caught = true; }
    assert(caught);
    pass("missing file throws");
}

static void test_missing_name_throws() {
    write_test_torrent_missing_name("/tmp/kw_test_noname.torrent");
    bool caught = false;
    try { parse_torrent("/tmp/kw_test_noname.torrent"); }
    catch (const MetainfoError&) { caught = true; }
    assert(caught);
    pass("missing name throws MetainfoError");
}

static void test_empty_file_throws() {
    std::ofstream f("/tmp/kw_test_empty.torrent");
    f << "";
    bool caught = false;
    try { parse_torrent("/tmp/kw_test_empty.torrent"); }
    catch (const std::exception&) { caught = true; }
    assert(caught);
    pass("empty file throws");
}


int main() {
    std::cout << "───> running test_metainfo\n\n";

    std::cout << "single file:\n";
    test_single_file_name();
    test_single_file_length();
    test_single_file_piece_length();
    test_single_file_piece_count();
    test_single_file_announce();
    test_single_file_is_not_multi();
    test_single_file_total_size();
    test_single_file_info_hash_not_zero();
    test_single_file_piece_hash_size();

    std::cout << "\nmulti file:\n";
    test_multi_file_is_multi();
    test_multi_file_count();
    test_multi_file_lengths();
    test_multi_file_paths();
    test_multi_file_total_size();
    test_multi_file_name();

    std::cout << "\nerror handling:\n";
    test_missing_file_throws();
    test_missing_name_throws();
    test_empty_file_throws();

    std::cout << "\nall tests passed.\n";
    return 0;
}