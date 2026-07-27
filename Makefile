CXX      := c++
CXXFLAGS := -std=c++17 -Wall -Wextra -I include
LDFLAGS  := -lssl -lcrypto

SRC_DIR  := srcs
TEST_DIR := tests
BIN_DIR  := bin


CRYPTO_SRC := $(SRC_DIR)/crypto/sha1.cpp \
              $(SRC_DIR)/crypto/peer_id.cpp

TORRENT_SRC := $(SRC_DIR)/torrent/metainfo.cpp \
               $(SRC_DIR)/torrent/torrent_file.cpp

BENCODE_SRC := $(SRC_DIR)/bencode/bencode.cpp


all: test_sha1 test_bencode test_metainfo

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

test_sha1: $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_sha1.cpp $(CRYPTO_SRC) -o $(BIN_DIR)/test_sha1 $(LDFLAGS)
	@./$(BIN_DIR)/test_sha1

test_bencode: $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_bencode.cpp $(BENCODE_SRC) -o $(BIN_DIR)/test_bencode
	@./$(BIN_DIR)/test_bencode

test_metainfo: $(BIN_DIR)
	@$(CXX) $(CXXFLAGS) $(TEST_DIR)/test_metainfo.cpp $(BENCODE_SRC) $(CRYPTO_SRC) $(TORRENT_SRC) -o $(BIN_DIR)/test_metainfo $(LDFLAGS)
	@./$(BIN_DIR)/test_metainfo

clean:
	@rm -rf $(BIN_DIR)

.PHONY: all clean test_sha1 test_bencode test_metainfo