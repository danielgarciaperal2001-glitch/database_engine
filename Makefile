CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -O2 -g -D_POSIX_C_SOURCE=200809L
BIN=bin
SRC=src
TESTS=tests

all: $(BIN)/database_engine

$(BIN)/tinydb: $(BIN) $(SRC)/*.c
	$(CC) $(CFLAGS) -o $@ $(SRC)/*.c

$(BIN):
	mkdir -p $@

test: $(BIN)/test_storage
	$<

$(BIN)/test_storage: $(BIN) $(TESTS)/test_storage.c $(SRC)/storage.c $(SRC)/catalog.c
	$(CC) $(CFLAGS) -o $@ $(TESTS)/test_storage.c $(SRC)/storage.c $(SRC)/catalog.c -lcunit

clean:
	rm -rf $(BIN)

.PHONY: all test clean

