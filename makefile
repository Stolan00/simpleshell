# Compiler to use
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -std=c11

# Name of the executable
TARGET = simpleshell

# Source files
SRCS = simpleshell.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Rule to compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Clean target for removing compiled files
clean:
	rm -f $(OBJS) $(TARGET)

# Run target to execute the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run