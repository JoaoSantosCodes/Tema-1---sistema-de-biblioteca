CC := gcc
CFLAGS := -O2 -Wall -Wextra -std=c11

.PHONY: all clean test

all: biblioteca

biblioteca: biblioteca.c
	$(CC) $(CFLAGS) biblioteca.c -o biblioteca

# Compila e roda testes unitários simples
# Inclui o fonte principal com BIBLIOTECA_NO_MAIN para evitar múltiplos mains
TEST_BIN := tests/bin/test

test: $(TEST_BIN)
	$(TEST_BIN)

$(TEST_BIN): tests/test.c biblioteca.c | tests/bin
	$(CC) $(CFLAGS) -DBIBLIOTECA_NO_MAIN tests/test.c -o $(TEST_BIN)

# Cria pasta de binários de testes
tests/bin:
	mkdir -p tests/bin

clean:
	rm -rf biblioteca tests/bin test_livros.csv