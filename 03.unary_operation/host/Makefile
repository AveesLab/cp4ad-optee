# No Need to modify this file.

CFLAGS += -Wall -I../ta -I$(OPTEE_CLIENT_EXPORT)/include -fstack-protector-strong
LDADD += -lteec -L$(OPTEE_CLIENT_EXPORT)/lib

SRCS := unary_operation.c
OBJS := $(patsubst %.c,$(O)/%.o,$(SRCS))

BINARY := unary_operation

.PHONY: all clean

all: $(O)/$(BINARY)

$(O)/$(BINARY): $(OBJS)
	mkdir -p $(dir $@)
	gcc -o $@ $(OBJS) $(LDADD)

$(O)/%.o: %.c
	mkdir -p $(dir $@)
	gcc $(CFLAGS) -c $< -o $@

install:
	@echo "[installing $(BINARY)...]"
	sudo cp $(O)/$(BINARY) $(CA_EXE_DIR)
	
clean:
	rm -f $(OBJS) $(O)/$(BINARY)
