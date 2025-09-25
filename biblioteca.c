/*
 * Sistema de cadastro de livros da biblioteca
 * Linguagem C
 *
 * Versão avançada:
 * - Struct para armazenar dados: nome, autor, editora, edição
 * - Entrada via terminal com validação
 * - Exibição organizada dos livros
 * - Persistência em CSV (salvar e carregar)
 * - Busca por título/autor (case-insensitive)
 * - Ordenação por nome/autor/edição
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <locale.h>
#include <ctype.h>

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
        char *p = entrada;
        while (*p == ' ' || *p == '\t') { p++; }
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

static void imprimir_livros(const Livro *livros, int quantidade) {
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
}

// Converte string para minúsculas (ASCII)
static void tolower_str(const char *src, char *dst, size_t n) {
    if (n == 0) return;
    size_t i;
    for (i = 0; i < n - 1 && src[i] != '\0'; i++) {
        dst[i] = (char)tolower((unsigned char)src[i]);
    }
    dst[i] = '\0';
}

// Verifica se pattern está contido em text (case-insensitive)
static int contains_substring_ci(const char *text, const char *pattern) {
    char t[TAM_TEXTO * 2];
    char p[TAM_TEXTO * 2];
    tolower_str(text, t, sizeof(t));
    tolower_str(pattern, p, sizeof(p));
    return strstr(t, p) != NULL;
}

// Busca por título
static void buscar_por_titulo(const Livro *livros, int quantidade, const char *consulta) {
    int encontrados = 0;
    for (int i = 0; i < quantidade; i++) {
        if (contains_substring_ci(livros[i].nome, consulta)) {
            if (encontrados == 0) {
                printf("\nResultados da busca por título:\n");
            }
            printf("- %s (Autor: %s, Editora: %s, Edição: %d)\n", livros[i].nome, livros[i].autor, livros[i].editora, livros[i].edicao);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum livro encontrado para a busca por título.\n");
    }
}

// Busca por autor
static void buscar_por_autor(const Livro *livros, int quantidade, const char *consulta) {
    int encontrados = 0;
    for (int i = 0; i < quantidade; i++) {
        if (contains_substring_ci(livros[i].autor, consulta)) {
            if (encontrados == 0) {
                printf("\nResultados da busca por autor:\n");
            }
            printf("- %s (Autor: %s, Editora: %s, Edição: %d)\n", livros[i].nome, livros[i].autor, livros[i].editora, livros[i].edicao);
            encontrados++;
        }
    }
    if (encontrados == 0) {
        printf("Nenhum livro encontrado para a busca por autor.\n");
    }
}

// Ordenação
static int cmp_por_nome(const void *a, const void *b) {
    const Livro *la = (const Livro *)a;
    const Livro *lb = (const Livro *)b;
    return strcmp(la->nome, lb->nome);
}
static int cmp_por_autor(const void *a, const void *b) {
    const Livro *la = (const Livro *)a;
    const Livro *lb = (const Livro *)b;
    return strcmp(la->autor, lb->autor);
}
static int cmp_por_edicao(const void *a, const void *b) {
    const Livro *la = (const Livro *)a;
    const Livro *lb = (const Livro *)b;
    if (la->edicao < lb->edicao) return -1;
    if (la->edicao > lb->edicao) return 1;
    return 0;
}

// Persistência CSV simples (separador ';')
static int salvar_csv(const char *caminho, const Livro *livros, int quantidade) {
    FILE *f = fopen(caminho, "w");
    if (!f) {
        perror("Erro ao abrir arquivo para escrita");
        return 0;
    }
    fprintf(f, "nome;autor;editora;edicao\n");
    for (int i = 0; i < quantidade; i++) {
        // Observação: campos não são escapados; evitando vírgula usando ';' como separador
        fprintf(f, "%s;%s;%s;%d\n", livros[i].nome, livros[i].autor, livros[i].editora, livros[i].edicao);
    }
    fclose(f);
    return 1;
}

static int carregar_csv(const char *caminho, Livro **livros_out, int *quantidade_out) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror("Erro ao abrir arquivo para leitura");
        return 0;
    }
    char linha[512];
    // pula cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }
    int capacidade = 16;
    int qtd = 0;
    Livro *vet = (Livro *)malloc(sizeof(Livro) * capacidade);
    if (!vet) { fclose(f); return 0; }
    while (fgets(linha, sizeof(linha), f)) {
        remove_nova_linha(linha);
        char *nome = strtok(linha, ";");
        char *autor = strtok(NULL, ";");
        char *editora = strtok(NULL, ";");
        char *edicao_str = strtok(NULL, ";");
        if (!nome || !autor || !editora || !edicao_str) {
            continue; // linha inválida
        }
        if (qtd == capacidade) {
            capacidade *= 2;
            Livro *tmp = (Livro *)realloc(vet, sizeof(Livro) * capacidade);
            if (!tmp) { free(vet); fclose(f); return 0; }
            vet = tmp;
        }
        strncpy(vet[qtd].nome, nome, sizeof(vet[qtd].nome)); vet[qtd].nome[sizeof(vet[qtd].nome)-1] = '\0';
        strncpy(vet[qtd].autor, autor, sizeof(vet[qtd].autor)); vet[qtd].autor[sizeof(vet[qtd].autor)-1] = '\0';
        strncpy(vet[qtd].editora, editora, sizeof(vet[qtd].editora)); vet[qtd].editora[sizeof(vet[qtd].editora)-1] = '\0';
        vet[qtd].edicao = (int)strtol(edicao_str, NULL, 10);
        qtd++;
    }
    fclose(f);
    *livros_out = vet;
    *quantidade_out = qtd;
    return 1;
}

#ifndef BIBLIOTECA_NO_MAIN
int main(void) {
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

    imprimir_livros(livros, quantidade);

    // Persistência CSV
    char resposta[8];
    ler_linha("\nDeseja salvar os livros em CSV (S/N)? ", resposta, sizeof(resposta));
    if (resposta[0] == 'S' || resposta[0] == 's') {
        if (salvar_csv("livros.csv", livros, quantidade)) {
            printf("Arquivo 'livros.csv' salvo com sucesso.\n");
        } else {
            printf("Falha ao salvar 'livros.csv'.\n");
        }
    }

    // Busca por título
    ler_linha("\nDeseja buscar por título (S/N)? ", resposta, sizeof(resposta));
    if (resposta[0] == 'S' || resposta[0] == 's') {
        char consulta[TAM_TEXTO];
        ler_linha("Digite o título (ou parte): ", consulta, sizeof(consulta));
        buscar_por_titulo(livros, quantidade, consulta);
    }

    // Busca por autor
    ler_linha("\nDeseja buscar por autor (S/N)? ", resposta, sizeof(resposta));
    if (resposta[0] == 'S' || resposta[0] == 's') {
        char consulta[TAM_TEXTO];
        ler_linha("Digite o autor (ou parte): ", consulta, sizeof(consulta));
        buscar_por_autor(livros, quantidade, consulta);
    }

    // Ordenação
    printf("\nOrdenar por:\n  1) Nome\n  2) Autor\n  3) Edição\n  0) Não ordenar\n");
    int opcao_ord = ler_inteiro("Escolha: ");
    if (opcao_ord == 1) {
        qsort(livros, (size_t)quantidade, sizeof(Livro), cmp_por_nome);
        printf("\nLista ordenada por nome:\n");
        imprimir_livros(livros, quantidade);
    } else if (opcao_ord == 2) {
        qsort(livros, (size_t)quantidade, sizeof(Livro), cmp_por_autor);
        printf("\nLista ordenada por autor:\n");
        imprimir_livros(livros, quantidade);
    } else if (opcao_ord == 3) {
        qsort(livros, (size_t)quantidade, sizeof(Livro), cmp_por_edicao);
        printf("\nLista ordenada por edição:\n");
        imprimir_livros(livros, quantidade);
    }

    // Carregar de CSV (opcional)
    ler_linha("\nDeseja carregar livros de um CSV e substituir a lista (S/N)? ", resposta, sizeof(resposta));
    if (resposta[0] == 'S' || resposta[0] == 's') {
        char caminho[256];
        ler_linha("Informe o caminho do CSV (ex.: livros.csv): ", caminho, sizeof(caminho));
        Livro *novos = NULL; int nova_qtd = 0;
        if (carregar_csv(caminho, &novos, &nova_qtd)) {
            free(livros);
            livros = novos;
            quantidade = nova_qtd;
            printf("\nLista carregada de '%s' com %d livro(s).\n", caminho, quantidade);
            imprimir_livros(livros, quantidade);
        } else {
            printf("Falha ao carregar CSV informado.\n");
        }
    }

    printf("\n==========================================\n");
    printf("Fim da execução.\n");

    free(livros);
    return 0;
}
#endif // BIBLIOTECA_NO_MAIN