CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Werror -O3 -pedantic -I.
TARGET = rv32i_core

SRCS = main.c \
       engine/memory.c \
       engine/elf_loader.c \
       engine/cpu.c \
       disassemble/disasm.c

OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
