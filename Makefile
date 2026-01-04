# ============================================
# GYM Management System - Build Configuration
# ============================================

CC = gcc
CFLAGS = -Wall -g `pkg-config --cflags gtk+-3.0 sqlite3`
LDFLAGS = `pkg-config --libs gtk+-3.0 sqlite3`

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
TARGET = $(BIN_DIR)/gym_system

# Default target: build the application
all: directories $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -Iinclude -c -o $@ $<

# Create necessary directories
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR) database

# Build and run the application
run: all
	./$(TARGET)

# Remove build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Show available commands
help:
	@echo "GYM Management System - Available Commands:"
	@echo "  make        - Build the application"
	@echo "  make run    - Build and run the application"
	@echo "  make clean  - Remove build artifacts"
	@echo "  make help   - Show this help message"

.PHONY: all clean directories run help
