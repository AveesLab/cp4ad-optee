CC ?= $(CROSS_COMPILE)gcc

CFLAGS += -I../ta -I${OPTEE_CLIENT_EXPORT}/include -fstack-protector-strong
LDADD += -lteec -L$(OPTEE_CLIENT_EXPORT)/lib

SRCS = main.c save_key.c
OBJS = $(patsubst %.c,$(O)/%.o,$(SRCS))

## 1. Modify the BINARY ##
BINARY = optee_crypto save_key

.PHONY: all install
all: $(BINARY) install

save_key: $(O)/save_key.o
	$(CC) -o $(O)/$@ $^ $(LDADD)

optee_crypto: $(O)/main.o
	$(CC) -o $(O)/$@ $^ $(LDADD)

$(O)/%.o: ./%.c
	mkdir -p $(O)
	$(CC) $(CFLAGS) -c $< -o $@

install: $(BINARY)
	mkdir -p $(OPTEE_CLIENT_EXPORT)/sbin
	for bin in $(BINARY); do cp $(O)/$$bin $(OPTEE_CLIENT_EXPORT)/sbin/; done


.PHONY: clean
clean:
	rm -f $(OBJS)
	for bin in $(BINARY); do rm -f $(O)/$$bin $(OPTEE_CLIENT_EXPORT)/sbin/$$bin; done

