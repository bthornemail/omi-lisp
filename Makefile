# OMI Layer: adapter / test
# Minimal build for the OMI-Lisp seed lowering stub.
# No parser, no evaluation, no validation, no receipt.

CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -O2 -Isrc

SRC_DIR := src
TEST_DIR := tests

OBJS := $(SRC_DIR)/omi_lisp.o
TEST_BIN := test_seed

.PHONY: all test clean

all: $(TEST_BIN)

$(SRC_DIR)/omi_lisp.o: $(SRC_DIR)/omi_lisp.c $(SRC_DIR)/omi_lisp.h
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_BIN): $(OBJS) $(TEST_DIR)/test_seed.c $(SRC_DIR)/omi_lisp.h
	$(CC) $(CFLAGS) $(OBJS) $(TEST_DIR)/test_seed.c -o $@

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -f $(OBJS) $(TEST_BIN)
