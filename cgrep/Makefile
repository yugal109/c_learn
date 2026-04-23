CC = clang
CFLAGS = -Wall -Wextra -pedantic -g -fsanitize=address

TARGET = cgrep
BUILD_DIR = build
SRCS = main.c search.c
OBJS = $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(BUILD_DIR) $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
