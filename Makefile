# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -I./h

# Directories
SRC_DIR = c
HEADER_DIR = h

# Source files
SOURCES = $(SRC_DIR)/ArrayManager.c \
          $(SRC_DIR)/fileReader.c \
          $(SRC_DIR)/loopHandler.c \
          main.c

# Object files (replacing .c with .o)
OBJECTS = $(SOURCES:.c=.o)

# Executable name
TARGET = asciilang

# Default target
all: $(TARGET)

# Linking the program
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compiling source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean built files
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean