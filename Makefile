CC      ?= gcc
CFLAGS  = -std=gnu11 -Wall -Wextra -g

# libsodium: Ubuntu cài vào /usr nên không cần thêm đường dẫn.
# macOS cài bằng Homebrew vào /opt/homebrew, phải chỉ đường cho gcc.
SODIUM_PREFIX := $(shell brew --prefix libsodium 2>/dev/null)
ifneq ($(SODIUM_PREFIX),)
CFLAGS  += -I$(SODIUM_PREFIX)/include
LDFLAGS += -L$(SODIUM_PREFIX)/lib
endif

SERVER = src/server
CLIENT = src/client
TESTS  = tests/test_protocol tests/test_client tests/test_accounts tests/test_server

all: $(SERVER) $(CLIENT)

$(SERVER): src/server.c src/accounts.h src/protocol.h
	$(CC) $(CFLAGS) -o $@ src/server.c $(LDFLAGS) -lsodium

$(CLIENT): src/client.c src/protocol.h
	$(CC) $(CFLAGS) -o $@ src/client.c

tests/test_protocol: tests/test_protocol.c src/protocol.h
	$(CC) $(CFLAGS) -o $@ $<

tests/test_client: tests/test_client.c src/client.c src/protocol.h
	$(CC) $(CFLAGS) -o $@ $<

tests/test_accounts: tests/test_accounts.c src/accounts.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lsodium

tests/test_server: tests/test_server.c src/server.c src/accounts.h src/protocol.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lsodium

test: $(TESTS)
	@for t in $(TESTS); do echo "===== $$t"; ./$$t || exit 1; done

clean:
	rm -f $(SERVER) $(CLIENT) $(TESTS)
	rm -rf src/*.dSYM tests/*.dSYM

.PHONY: all test clean
