#include "../include/bencode/bencode.hpp"
#include <cassert>
#include <iostream>

static void pass(const char* name) {
    std::cout << "  [PASS] " << name << "\n";
}

/* ────────────────── integer ────────────────── */

static void test_integer_positive() {
    size_t pos = 0;
    auto v = decode("i42e", pos);
    assert(v.is_int());
    assert(v.as_int() == 42);
    pass("integer positive");
}

static void test_integer_zero() {
    size_t pos = 0;
    auto v = decode("i0e", pos);
    assert(v.is_int());
    assert(v.as_int() == 0);
    pass("integer zero");
}

static void test_integer_negative() {
    size_t pos = 0;
    auto v = decode("i-7e", pos);
    assert(v.is_int());
    assert(v.as_int() == -7);
    pass("integer negative");
}

static void test_integer_large() {
    size_t pos = 0;
    auto v = decode("i9999999999e", pos);
    assert(v.is_int());
    assert(v.as_int() == 9999999999LL);
    pass("integer large");
}

/* ────────────────── string ────────────────── */

static void test_string_simple() {
    size_t pos = 0;
    auto v = decode("4:spam", pos);
    assert(v.is_string());
    assert(v.as_string() == "spam");
    pass("string simple");
}

static void test_string_empty() {
    size_t pos = 0;
    auto v = decode("0:", pos);
    assert(v.is_string());
    assert(v.as_string().empty());
    pass("string empty");
}

static void test_string_binary() {
    size_t pos = 0;
    std::string raw = "3:\x01\x02\x03";
    auto v = decode(raw, pos);
    assert(v.is_string());
    assert(v.as_string().size() == 3);
    assert((unsigned char)v.as_string()[0] == 0x01);
    pass("string binary safe");
}

/* ────────────────── list ────────────────── */

static void test_list_mixed() {
    size_t pos = 0;
    auto v = decode("l4:spami42ee", pos);
    assert(v.is_list());
    assert(v.as_list().size() == 2);
    assert(v.as_list()[0].is_string());
    assert(v.as_list()[0].as_string() == "spam");
    assert(v.as_list()[1].is_int());
    assert(v.as_list()[1].as_int() == 42);
    pass("list mixed types");
}

static void test_list_empty() {
    size_t pos = 0;
    auto v = decode("le", pos);
    assert(v.is_list());
    assert(v.as_list().empty());
    pass("list empty");
}

static void test_list_nested() {
    size_t pos = 0;
    auto v = decode("lli1ei2eei3ee", pos);
    assert(v.is_list());
    assert(v.as_list().size() == 2);
    assert(v.as_list()[0].is_list());
    assert(v.as_list()[0].as_list()[0].as_int() == 1);
    assert(v.as_list()[0].as_list()[1].as_int() == 2);
    assert(v.as_list()[1].as_int() == 3);
    pass("list nested");
}

/* ────────────────── dict ────────────────── */

static void test_dict_simple() {
    size_t pos = 0;
    auto v = decode("d4:spami42ee", pos);
    assert(v.is_dict());
    assert(v.as_dict().count("spam"));
    assert(v.as_dict().at("spam").as_int() == 42);
    pass("dict simple");
}

static void test_dict_empty() {
    size_t pos = 0;
    auto v = decode("de", pos);
    assert(v.is_dict());
    assert(v.as_dict().empty());
    pass("dict empty");
}

static void test_dict_multiple_keys() {
    size_t pos = 0;
    auto v = decode("d3:bar4:spam3:fooi42ee", pos);
    assert(v.is_dict());
    assert(v.as_dict().at("bar").as_string() == "spam");
    assert(v.as_dict().at("foo").as_int() == 42);
    pass("dict multiple keys");
}

static void test_dict_nested() {
    size_t pos = 0;
    auto v = decode("d3:keyd5:inneri99eee", pos);
    assert(v.is_dict());
    assert(v.as_dict().at("key").is_dict());
    assert(v.as_dict().at("key").as_dict().at("inner").as_int() == 99);
    pass("dict nested");
}

/* ────────────────── offset tracking ────────────────── */

static void test_offset_after_integer() {
    size_t pos = 0;
    decode("i42e", pos);
    assert(pos == 4);
    pass("offset lands after integer");
}

static void test_offset_after_string() {
    size_t pos = 0;
    decode("4:spam", pos);
    assert(pos == 6);
    pass("offset lands after string");
}

static void test_offset_sequential() {
    std::string raw = "i1ei2e";
    size_t pos = 0;
    auto v1 = decode(raw, pos);
    auto v2 = decode(raw, pos);
    assert(v1.as_int() == 1);
    assert(v2.as_int() == 2);
    assert(pos == 6);
    pass("offset sequential parse");
}

static void test_offset_dict_reentry() {
    std::string raw = "d4:infod3:keyi1eee";
    size_t pos = 0;
    auto v = decode(raw, pos);

    assert(v.as_dict().at("info").as_dict().at("key").as_int() == 1);
    assert(pos == raw.size());
    pass("offset dict re-entry (info hash use case)");
}

/* ────────────────── type safety ────────────────── */

static void test_wrong_type_throws() {
    size_t pos = 0;
    auto v = decode("i42e", pos);
    bool caught = false;
    try { v.as_string(); }
    catch (const std::bad_variant_access&) { caught = true; }
    assert(caught);
    pass("wrong type accessor throws");
}

/* ────────────────── error handling ────────────────── */

static void test_unknown_type_throws() {
    size_t pos = 0;
    bool caught = false;
    try { decode("x99e", pos); }
    catch (const std::exception&) { caught = true; }
    assert(caught);
    pass("unknown type throws");
}

static void test_empty_throws() {
    size_t pos = 0;
    bool caught = false;
    try { decode("", pos); }
    catch (const std::exception&) { caught = true; }
    assert(caught);
    pass("empty input throws");
}

/* ────────────────── main ────────────────── */

int main() {
    std::cout << std::endl<< "───> running test_bencode\n\n";

    std::cout << "integers:\n";
    test_integer_positive();
    test_integer_zero();
    test_integer_negative();
    test_integer_large();

    std::cout << "\nstrings:\n";
    test_string_simple();
    test_string_empty();
    test_string_binary();

    std::cout << "\nlists:\n";
    test_list_mixed();
    test_list_empty();
    test_list_nested();

    std::cout << "\ndicts:\n";
    test_dict_simple();
    test_dict_empty();
    test_dict_multiple_keys();
    test_dict_nested();

    std::cout << "\noffset tracking:\n";
    test_offset_after_integer();
    test_offset_after_string();
    test_offset_sequential();
    test_offset_dict_reentry();

    std::cout << "\ntype safety:\n";
    test_wrong_type_throws();

    std::cout << "\nerror handling:\n";
    test_unknown_type_throws();
    test_empty_throws();

    std::cout << "\nall " << 19 << " tests passed.\n";
    return 0;
}