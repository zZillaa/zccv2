CC = gcc
CFLAGS = -Wall -g -std=c11 -D_POSIX_C_SOURCE=200809L

SRCS = main.c preprocessor.c lexer.c parser.c semantics_other.c IR.c
OBJS = ${SRCS:.c=.o}
target = compiler

all: $(target)

$(target) : $(OBJS)
	$(CC) $(OBJS) -o $@

%.o: %c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(target)

.PHONY: all clean