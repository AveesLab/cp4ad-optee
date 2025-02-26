CC ?= $(CROSS_COMPILE)gcc
CADD = $(CFLAGS) -Wall -I../ta -I$(OPTEE_CLIENT_EXPORT)/include -fstack-protector-strong
LDADD = $(LDFLAGS)  -lteec -L$(OPTEE_CLIENT_EXPORT)/lib

SRCS = main.c generate_key.c
OBJS = $(patsubst %.c,$(O)/%.o,$(SRCS))

## 1. Modify the BINARY ##
BINARY = rsa generate_key

.PHONY: all install
all: $(BINARY) install

generate_key: $(O)/generate_key.o
	$(CC) -o $(O)/$@ $^ $(LDADD)

rsa: $(O)/main.o
	$(CC) -o $(O)/$@ $^ $(LDADD)

$(O)/%.o: %.c
	@echo here
	@echo $(CADD)
	@echo $(LDADD)
	@echo here
	mkdir -p $(O)
	$(CC) $(CADD) -c $< -o $@

install: $(BINARY)
	mkdir -p $(OPTEE_CLIENT_EXPORT)/sbin
	for bin in $(BINARY); do cp $(O)/$$bin $(OPTEE_CLIENT_EXPORT)/sbin/; done

.PHONY: clean
clean:
	rm -f $(OBJS)
	for bin in $(BINARY); do rm -f $(O)/$$bin $(OPTEE_CLIENT_EXPORT)/sbin/$$bin; done
