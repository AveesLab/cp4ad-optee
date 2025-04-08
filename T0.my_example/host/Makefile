## HOST (Client Application) Makefile (Upgraded Version)

CC ?= gcc
O ?= ../build/host

CFLAGS += -Wall -I../ta -I$(OPTEE_CLIENT_EXPORT)/include -fstack-protector-strong
LDADD += -lteec -L$(OPTEE_CLIENT_EXPORT)/lib

SRCS := main.c
OBJS := $(patsubst %.c,$(O)/%.o,$(SRCS))

BINARY := hello_world

.PHONY: all clean

all: $(O)/$(BINARY)

$(O)/$(BINARY): $(OBJS)
	mkdir -p $(dir $@)
	$(CC) -o $@ $(OBJS) $(LDADD)

$(O)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(O)/$(BINARY)
