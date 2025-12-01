CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = -lpthread

# Executables
EXECS = zombie_creator zombie_detector zombie_reaper process_daemon test_lib

# Library files
LIB_OBJS = src/zombie.o
LIB_TARGET = libzombie.a

.PHONY: all clean test

all: $(EXECS) $(LIB_TARGET)

# Rule for executables
zombie_creator: src/zombie_creator.c
	$(CC) $(CFLAGS) $< -o $@

zombie_detector: src/zombie_detector.c
	$(CC) $(CFLAGS) $< -o $@

zombie_reaper: src/zombie_reaper.c
	$(CC) $(CFLAGS) $< -o $@

process_daemon: src/process_daemon.c
	$(CC) $(CFLAGS) $< -o $@

# Rule for library object file
src/zombie.o: src/zombie.c src/zombie.h
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for static library
$(LIB_TARGET): $(LIB_OBJS)
	ar rcs $@ $(LIB_OBJS)

# Rule for library test program
test_lib: tests/test_lib.c $(LIB_TARGET)
	$(CC) $(CFLAGS) $< -L. -lzombie $(LDFLAGS) -o $@

# Test execution: Run the test scripts
test: all
	@echo "--- Running Tests ---"
	@./tests/test_creator.sh
	@./tests/test_detector.sh
	@./tests/test_reaper.sh
	@./tests/test_daemon.sh
	@echo "--- Running Library Test ---"
	@./test_lib

clean:
	rm -f $(EXECS) $(LIB_OBJS) $(LIB_TARGET)
	rm -f tests/test_lib
	rm -rf *.dSYM
	@echo "Cleaned."
