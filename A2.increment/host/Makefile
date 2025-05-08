# No Need to modify this file.

CFLAGS += -Wall -I../ta -I$(OPTEE_CLIENT_EXPORT)/include -fstack-protector-strong
LDADD += -lteec -L$(OPTEE_CLIENT_EXPORT)/lib

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c,$(O)/%.o,$(SRCS))
BINARY := $(patsubst %.c,$(O)/%, $(SRCS))

.PHONY: all clean

all: $(OBJS) $(BINARY)

$(O)/%.o: %.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

$(O)/%: $(O)/%.o
	mkdir -p $(dir $@)
	gcc -o $@ $< $(LDADD)

install: $(BINARY)
	@echo "[installing binaries...]"
	sudo cp $(foreach bin, $(BINARY), $(bin)) $(CA_EXE_DIR)

clean:
	rm -f $(OBJS) $(BINARY)