CC = clang
CFLAGS = -Wall -Wextra -pedantic -g -fsanitize=address
INCLUDES = -Iinclude

BUILD_DIR = build
SRC_DIR = src
TEST_DIR = tests

SRCS = $(SRC_DIR)/allocator.c $(SRC_DIR)/zones.c $(SRC_DIR)/calloc.c $(SRC_DIR)/debug.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

LIB = $(BUILD_DIR)/libyugalloc.a
TEST_BIN = $(TEST_DIR)/test

all: $(BUILD_DIR) $(LIB)

$(LIB): $(OBJS)
	ar rcs $@ $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(BUILD_DIR) $(LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TEST_BIN) $(TEST_DIR)/main.c $(LIB)
	./$(TEST_BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
