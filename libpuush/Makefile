CC_FLAGS = -Wall -g
LD_FLAGS = -lcurl
CC = clang

.PHONY: clean all

all: libpuush-test

libpuush.o: libpuush.c libpuush.h libpuush_internal.h
	$(CC) -c libpuush.c $(CC_FLAGS)

libpuush_internal.o: libpuush_internal.c libpuush_internal.h
	$(CC) -c libpuush_internal.c $(CC_FLAGS)

libpuush-test: libpuush.o libpuush_internal.o
	$(CC) libpuush.o libpuush_internal.o -o libpuush-test $(LD_FLAGS)

clean:
	rm -f ./libpuush-test ./libpuush.o ./libpuush_internal.o
