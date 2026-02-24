CC      ?= gcc
CXX     ?= g++
CFLAGS  ?= -O2 -Wall -Wextra -std=c11
CXXFLAGS?= -O2 -Wall -Wextra -std=c++17
LDFLAGS ?=

BUILD_DIR := build
BIN_DIR   := $(BUILD_DIR)/bin

SRCS_C   := src/protocol_stack.c src/routing.c src/platform_linux.c
SRCS_CPP := src/router_metrics.cpp

OBJS_C   := $(SRCS_C:src/%.c=$(BUILD_DIR)/%.o)
OBJS_CPP := $(SRCS_CPP:src/%.cpp=$(BUILD_DIR)/%.o)

TARGET   := $(BIN_DIR)/sentryflow_firmware

.PHONY: all clean run test

all: $(TARGET)

$(BUILD_DIR)/%.o: src/%.c include/%.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Iinclude -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)/bin

$(TARGET): $(OBJS_C) $(OBJS_CPP) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJS_C) $(OBJS_CPP) -o $@ $(LDFLAGS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

run: $(TARGET)
	$(TARGET)

test: all
	@echo "Running basic firmware self-test..."
	$(TARGET) --self-test

clean:
	rm -rf $(BUILD_DIR)

