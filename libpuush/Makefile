CC_FLAGS = -Wall -fPIC
LD_FLAGS = -lcurl -lssl -lcrypto -lm -shared
CC = clang

.PHONY: clean all

all: libpuush.so

libpuush.o: libpuush.c libpuush.h libpuush_internal.h
	$(CC) $(CC_FLAGS) -c libpuush.c -o libpuush.o

libpuush_internal.o: libpuush_internal.c libpuush_internal.h
	$(CC) $(CC_FLAGS) -c libpuush_internal.c -o libpuush_internal.o

libpuush.so: libpuush.o libpuush_internal.o
	$(CC) libpuush.o libpuush_internal.o -o libpuush.so $(LD_FLAGS)

clean:
	rm -f ./libpuush.o ./libpuush_internal.o ./libpuush.so
