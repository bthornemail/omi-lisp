# OMI Layer: adapter / test
# Minimal build for the OMI-Lisp lowering + downstream handoff stub +
# tiny fixture parser. No evaluation, no validation, no receipt.

CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -Isrc

SRC_DIR := src
TEST_DIR := tests

OBJS := $(SRC_DIR)/omi_lisp.o $(SRC_DIR)/omi_candidate.o $(SRC_DIR)/omi_parse.o $(SRC_DIR)/omi_adapter_contract.o
BUILD_DIR := build
TEST_BIN_SEED := $(BUILD_DIR)/test_seed
TEST_BIN_PAIR := $(BUILD_DIR)/test_pair
TEST_BIN_SYMBOL := $(BUILD_DIR)/test_symbol
TEST_BIN_CANDIDATE := $(BUILD_DIR)/test_candidate
TEST_BIN_PARSE := $(BUILD_DIR)/test_parse
TEST_BIN_FIXTURES := $(BUILD_DIR)/test_fixtures
TEST_BIN_NESTED := $(BUILD_DIR)/test_nested
TEST_BIN_ADAPTER := $(BUILD_DIR)/test_adapter_contract

.PHONY: all test clean

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

all: $(TEST_BIN_SEED) $(TEST_BIN_PAIR) $(TEST_BIN_SYMBOL) $(TEST_BIN_CANDIDATE) $(TEST_BIN_PARSE) $(TEST_BIN_FIXTURES) $(TEST_BIN_NESTED) $(TEST_BIN_ADAPTER)

$(SRC_DIR)/omi_lisp.o: $(SRC_DIR)/omi_lisp.c $(SRC_DIR)/omi_lisp.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/omi_candidate.o: $(SRC_DIR)/omi_candidate.c $(SRC_DIR)/omi_candidate.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/omi_parse.o: $(SRC_DIR)/omi_parse.c $(SRC_DIR)/omi_parse.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/omi_adapter_contract.o: $(SRC_DIR)/omi_adapter_contract.c $(SRC_DIR)/omi_adapter_contract.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN_SEED): $(OBJS) $(TEST_DIR)/test_seed.c $(SRC_DIR)/omi_lisp.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_seed.c -o $@

$(TEST_BIN_PAIR): $(OBJS) $(TEST_DIR)/test_pair.c $(SRC_DIR)/omi_lisp.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_pair.c -o $@

$(TEST_BIN_SYMBOL): $(OBJS) $(TEST_DIR)/test_symbol.c $(SRC_DIR)/omi_lisp.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_symbol.c -o $@

$(TEST_BIN_CANDIDATE): $(OBJS) $(TEST_DIR)/test_candidate.c $(SRC_DIR)/omi_candidate.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_candidate.c -o $@

$(TEST_BIN_PARSE): $(OBJS) $(TEST_DIR)/test_parse.c $(SRC_DIR)/omi_parse.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_parse.c -o $@

$(TEST_BIN_FIXTURES): $(OBJS) $(TEST_DIR)/test_fixtures.c $(SRC_DIR)/omi_parse.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_fixtures.c -o $@

$(TEST_BIN_NESTED): $(OBJS) $(TEST_DIR)/test_nested.c $(SRC_DIR)/omi_parse.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_nested.c -o $@

$(TEST_BIN_ADAPTER): $(OBJS) $(TEST_DIR)/test_adapter_contract.c $(SRC_DIR)/omi_adapter_contract.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_adapter_contract.c -o $@

test: $(TEST_BIN_SEED) $(TEST_BIN_PAIR) $(TEST_BIN_SYMBOL) $(TEST_BIN_CANDIDATE) $(TEST_BIN_PARSE) $(TEST_BIN_FIXTURES) $(TEST_BIN_NESTED) $(TEST_BIN_ADAPTER)
	./$(TEST_BIN_SEED)
	./$(TEST_BIN_PAIR)
	./$(TEST_BIN_SYMBOL)
	./$(TEST_BIN_CANDIDATE)
	./$(TEST_BIN_PARSE)
	./$(TEST_BIN_FIXTURES)
	./$(TEST_BIN_NESTED)
	./$(TEST_BIN_ADAPTER)

clean:
	rm -rf $(BUILD_DIR) $(OBJS)
