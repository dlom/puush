CC_FLAGS = -Wall -g
LD_FLAGS = -lcurl
CC = clang

.PHONY: clean all

all: test

libpuush.o: libpuush.c libpuush.h libpuush_internal.h
	$(CC) -c libpuush.c $(CC_FLAGS)

libpuush_internal.o: libpuush_internal.c libpuush_internal.h
	$(CC) -c libpuush_internal.c $(CC_FLAGS)

test: libpuush.o libpuush_internal.o
	$(CC) libpuush.o libpuush_internal.o -o test $(LD_FLAGS)

clean:
	rm -f ./test ./libpuush.o ./libpuush_internal.o
