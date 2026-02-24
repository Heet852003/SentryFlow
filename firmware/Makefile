CC      ?= gcc
CXX     ?= g++
CFLAGS  ?= -O2 -Wall -Wextra -std=c11
CXXFLAGS?= -O2 -Wall -Wextra -std=c++17
LDFLAGS ?=

BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin

SRCS_C   := \
	src/main.c \
	src/protocol_stack.c \
	src/platform_linux.c \
	src/sf_crc32.c \
	src/sf_protocol.c \
	src/sf_commands.c \
	src/routing_table.c \
	src/routing.c \
	src/hal_linux.c

SRCS_CPP := src/router_metrics.cpp

OBJS_C   := $(SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
OBJS_CPP := $(SRCS_CPP:src/%.cpp=$(BUILD_DIR)/%.o)

TARGET   := $(BIN_DIR)/sentryflow_firmware
TEST_BIN := $(BIN_DIR)/sentryflow_tests

.PHONY: all clean run test

all: $(TARGET)

$(BUILD_DIR)/%.o: src/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/bin

$(TARGET): $(OBJS_C) $(OBJS_CPP) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS_C) $(OBJS_CPP) -o $@ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(TARGET)
	$(TARGET)

$(TEST_BIN): $(BUILD_DIR)/sf_crc32.o $(BUILD_DIR)/sf_protocol.o $(BUILD_DIR)/routing_table.o $(BUILD_DIR)/test_main.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BUILD_DIR)/test_main.o: tests/test_main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

test: $(TEST_BIN) $(TARGET)
	@echo "Running unit tests..."
	$(TEST_BIN)
	@echo "Running firmware self-test..."
	$(TARGET) --self-test

clean:
	rm -rf $(BUILD_DIR)

