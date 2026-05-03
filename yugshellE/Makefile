CC = clang
CFLAGS = -Wall -Wextra -pedantic -g -fsanitize=address

# -Wall: enable most common warning messages
# -Wextra: enable additional warnings not included in -Wall
# -pedantic: warn about code that is not strict ISO C standard
# -g: include debug symbols (so debuggers like gdb/lldb show source lines and variables)
# -fsanitize=address: turn on AddressSanitizer to catch memory bugs at runtime (buffer overflows, use-after-free, etc.)


TARGET = yugshell
BUILD_DIR = build
SRCS = main.c input.c parser.c builtins.c executor.c history.c
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
