CC_FLAGS = -I./libpuush -Wall
LD_FLAGS = -L./libpuush -lpuush
CC = clang

.PHONY: clean all run

all: libpuush-test

run: libpuush-test
	LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:./libpuush ./libpuush-test

libpuush-test: libpuush-test.c libpuush/libpuush.so
	$(CC) $(CC_FLAGS) libpuush-test.c -o libpuush-test $(LD_FLAGS)

libpuush/libpuush.so:
	cd libpuush && $(MAKE) libpuush.so

clean:
	rm -f ./libpuush-test
	cd libpuush && $(MAKE) clean
