CC_FLAGS = -Wall -g -I./libpuush
LD_FLAGS = -L./libpuush -lpuush
CC = gcc

.PHONY: clean all run

all: libpuush-test

run: libpuush-test
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./libpuush && ./libpuush-test

libpuush-test: libpuush-test.c libpuush/libpuush.so
	$(CC) $(CC_FLAGS) libpuush-test.c -o libpuush-test $(LD_FLAGS)

libpuush/libpuush.so:
	cd libpuush && $(MAKE)

clean:
	rm -f ./libpuush-test
	cd libpuush && $(MAKE) clean
