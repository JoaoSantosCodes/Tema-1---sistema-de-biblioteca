/*
 * Sistema de cadastro de livros da biblioteca
 * Linguagem C
 *
 * Este programa utiliza struct para armazenar dados de livros (nome, autor, editora e edição),
 * realiza a entrada via terminal e exibe os dados cadastrados.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <locale.h>

#define TAM_TEXTO 120

typedef struct {
    char nome[TAM_TEXTO];
    char autor[TAM_TEXTO];
    char editora[TAM_TEXTO];
    int edicao;
} Livro;

// Remove o caractere de nova linha ao final (se existir)
static void remove_nova_linha(char *s) {
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n') {
        s[len - 1] = '\0';
    }
}

// Lê uma linha de texto com prompt e garante que não haverá overflow
static void ler_linha(const char *prompt, char *buffer, size_t tamanho) {
    printf("%s", prompt);
    if (fgets(buffer, (int)tamanho, stdin) == NULL) {
        // Em caso de erro de leitura, definir string vazia
        buffer[0] = '\0';
        return;
    }
    remove_nova_linha(buffer);
}

// Lê um inteiro com validação
static int ler_inteiro(const char *prompt) {
    char entrada[64];
    while (1) {
        ler_linha(prompt, entrada, sizeof(entrada));
        // Ignora espaços iniciais
        char *p = entrada;
        while (*p == ' ' || *p == '\t') {
            p++;
        }
        // Converte para inteiro
        char *fim = NULL;
        long valor = strtol(p, &fim, 10);
        if (fim != p && *fim == '\0') {
            if (valor >= INT_MIN && valor <= INT_MAX) {
                return (int)valor;
            }
        }
        printf("Entrada inválida. Digite um número inteiro válido.\n");
    }
}

int main(void) {
    // Ajusta locale para melhor exibição em consoles com acentuação
    setlocale(LC_ALL, "");

    printf("==========================================\n");
    printf("  Sistema de Cadastro de Livros (C)\n");
    printf("==========================================\n\n");

    int quantidade = ler_inteiro("Quantos livros deseja cadastrar? ");
    if (quantidade <= 0) {
        printf("Nenhum livro será cadastrado. Encerrando.\n");
        return 0;
    }

    Livro *livros = (Livro *)malloc(sizeof(Livro) * (size_t)quantidade);
    if (!livros) {
        fprintf(stderr, "Erro: memória insuficiente.\n");
        return 1;
    }

    for (int i = 0; i < quantidade; i++) {
        printf("\n== Cadastro do Livro %d ==\n", i + 1);
        ler_linha("Nome da obra: ", livros[i].nome, sizeof(livros[i].nome));
        ler_linha("Autor: ", livros[i].autor, sizeof(livros[i].autor));
        ler_linha("Editora: ", livros[i].editora, sizeof(livros[i].editora));
        livros[i].edicao = ler_inteiro("Edição (número inteiro): ");
    }

    printf("\n------------------------------------------\n");
    printf("         Livros cadastrados\n");
    printf("------------------------------------------\n");

    for (int i = 0; i < quantidade; i++) {
        printf("\nLivro %d\n", i + 1);
        printf("  Nome:    %s\n", livros[i].nome);
        printf("  Autor:   %s\n", livros[i].autor);
        printf("  Editora: %s\n", livros[i].editora);
        printf("  Edição:  %d\n", livros[i].edicao);
    }

    printf("\n==========================================\n");
    printf("Fim da listagem.\n");

    free(livros);
    return 0;
}