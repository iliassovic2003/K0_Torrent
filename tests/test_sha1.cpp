#include "../include/crypto/sha1.hpp"
#include <cassert>
#include <iostream>

static void pass(const char* name) {
    std::cout << "  [PASS] " << name << "\n";
}

static void test_empty_string_hex() {
    assert(sha1_hex("") == "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    pass("empty string hex");
}

static void test_abc_hex() {
    assert(sha1_hex("abc") == "a9993e364706816aba3e25717850c26c9cd0d89d");
    pass("abc hex");
}

static void test_long_string_hex() {
    assert(sha1_hex("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq")
        == "84983e441c3bd26ebaae4aa1f95129e5e54670f1");
    pass("long string hex");
}


static void test_raw_size() {
    auto digest = sha1_raw("hello");
    assert(digest.size() == 20);
    pass("raw digest is 20 bytes");
}

static void test_raw_matches_hex() {
    std::string input = "abc";
    auto raw = sha1_raw(input);
    auto hex = sha1_hex(input);

    std::string converted;
    for (uint8_t b : raw) {
        char buf[3];
        std::snprintf(buf, sizeof(buf), "%02x", b);
        converted += buf;
    }
    assert(converted == hex);
    pass("raw digest matches hex");
}

static void test_empty_raw_not_zero() {
    auto digest = sha1_raw("");
    std::array<uint8_t, 20> zero{};
    assert(digest != zero);
    pass("empty string raw not all zeros");
}


static void test_same_input_same_output() {
    assert(sha1_hex("katch_wire") == sha1_hex("katch_wire"));
    pass("same input same output");
}

static void test_different_input_different_output() {
    assert(sha1_hex("katch_wire") != sha1_hex("katch_wire2"));
    pass("different input different output");
}


static void test_binary_input() {
    std::string binary;
    binary += '\x00';
    binary += '\x01';
    binary += '\xff';

    auto digest = sha1_raw(binary);
    assert(digest.size() == 20);
    pass("binary input with null bytes");
}

static void test_null_byte_differs_from_empty() {
    std::string null_byte;
    null_byte += '\x00';
    assert(sha1_hex(null_byte) != sha1_hex(""));
    pass("null byte differs from empty string");
}


static void test_hex_length() {
    assert(sha1_hex("anything").size() == 40);
    pass("hex string is always 40 characters");
}

static void test_hex_lowercase() {
    auto hex = sha1_hex("abc");
    for (char c : hex)
        assert(std::islower(c) || std::isdigit(c));
    pass("hex string is lowercase");
}

static void test_hex_no_spaces() {
    auto hex = sha1_hex("test");
    assert(hex.find(' ') == std::string::npos);
    pass("hex string has no spaces");
}


int main() {
    std::cout << "───> running test_sha1\n\n";

    std::cout << "known vectors:\n";
    test_empty_string_hex();
    test_abc_hex();
    test_long_string_hex();

    std::cout << "\nraw digest:\n";
    test_raw_size();
    test_raw_matches_hex();
    test_empty_raw_not_zero();

    std::cout << "\ndeterministic:\n";
    test_same_input_same_output();
    test_different_input_different_output();

    std::cout << "\nbinary safety:\n";
    test_binary_input();
    test_null_byte_differs_from_empty();

    std::cout << "\nhex format:\n";
    test_hex_length();
    test_hex_lowercase();
    test_hex_no_spaces();

    std::cout << "\nall tests passed.\n";
    return 0;
}