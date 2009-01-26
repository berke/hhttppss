CC=gcc
#CFLAGS=-Wall -g -DDEBUG=1 -D_GNU_SOURCE
CFLAGS=-Wall -O3 -DDEBUG=0 -D_GNU_SOURCE

OBJS= \
    bailout.o \
    memory.o \
    options.o \
    log.o \
    intmap.o \
    stack.o \
    demux.o \
    lines.o \
    scan.o \
    writer.o \
    resource.o \
    http.o \
    server.o \
    main.o

.PHONY: all

all: hhttppss

hhttppss: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o $@

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o hhttppss
