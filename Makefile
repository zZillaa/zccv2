CC = gcc
CFLAGS = -Wall -g -std=c11 -D_POSIX_C_SOURCE=200809L

SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = ${patsubst $(SRCDIR)/%.c, %.o, $(SRCS)}
target = compiler

all: $(target)

$(target) : $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(target)
