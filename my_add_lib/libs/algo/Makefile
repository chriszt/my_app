CROSS_COMPILE_GCC := aarch64-linux-gnu-gcc
CROSS_COMPILE_AR := aarch64-linux-gnu-ar

SRCS := add.c sub.c
TAT_A := libalgo.a
TAT_SO := ../../../../out-br/target/usr/lib/libalgo.so

.PHONY: all
all:
	$(CROSS_COMPILE_GCC) -Wall -Iinclude -c $(SRCS) -g
	$(CROSS_COMPILE_AR) rcs $(TAT_A) $(SRCS:.c=.o)
	-rm -rf $(SRCS:.c=.o)
	$(CROSS_COMPILE_GCC) -Wall -Iinclude -fPIC -c $(SRCS) -g
	$(CROSS_COMPILE_GCC) -shared $(SRCS:.c=.o) -o $(TAT_SO)
	-rm -rf $(SRCS:.c=.o)

.PHONY: clean
clean:
	-rm -rf $(TAT_SO) $(TAT_A)
