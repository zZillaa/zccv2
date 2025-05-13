CC = gcc
CFLAGS = -Wall -g -std=c11 -D_POSIX_C_SOURCE=200809L

SRCDIR = src
OBJDIR = obj
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

target = compiler

all: $(target)

$(target): $(OBJS)
	$(CC) $(OBJS) -o $@


$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(target)
