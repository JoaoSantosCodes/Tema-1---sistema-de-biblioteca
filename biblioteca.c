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

#define BIBLIOTECA_VERSION "1.0.0"
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
// Helpers de CSV com separador ';' e suporte a aspas
static int campo_precisa_aspas(const char *s) {
    for (const char *p = s; *p; ++p) {
        if (*p == ';' || *p == '"' || *p == '\n' || *p == '\r') return 1;
    }
    // preservar espaços iniciais/finais
    if (s[0] == ' ' || (strlen(s) > 0 && s[strlen(s)-1] == ' ')) return 1;
    return 0;
}

static void escrever_campo_csv(FILE *f, const char *s) {
    if (!s) { fputs("", f); return; }
    if (!campo_precisa_aspas(s)) {
        fputs(s, f);
        return;
    }
    fputc('"', f);
    for (const char *p = s; *p; ++p) {
        if (*p == '"') { fputs("\"\"", f); }
        else { fputc(*p, f); }
    }
    fputc('"', f);
}

static int salvar_csv(const char *caminho, const Livro *livros, int quantidade) {
    FILE *f = fopen(caminho, "w");
    if (!f) {
        perror("Erro ao abrir arquivo para escrita");
        return 0;
    }
    fprintf(f, "nome;autor;editora;edicao\n");
    for (int i = 0; i < quantidade; i++) {
        escrever_campo_csv(f, livros[i].nome); fputc(';', f);
        escrever_campo_csv(f, livros[i].autor); fputc(';', f);
        escrever_campo_csv(f, livros[i].editora); fputc(';', f);
        fprintf(f, "%d\n", livros[i].edicao);
    }
    fclose(f);
    return 1;
}

// Parser simples de linha CSV com separador ';' e aspas
// Retorna 1 em sucesso e preenche campos out_nome/out_autor/out_editora/out_edicao
static int parse_csv_linha(const char *linha, char *out_nome, size_t n1, char *out_autor, size_t n2, char *out_editora, size_t n3, int *out_edicao) {
    const char *p = linha;
    char buf[3][TAM_TEXTO];
    for (int campo = 0; campo < 3; campo++) {
        size_t bi = 0; int aspas = 0;
        if (*p == '"') { aspas = 1; p++; }
        while (*p) {
            if (aspas) {
                if (*p == '"') {
                    if (*(p+1) == '"') { if (bi < sizeof(buf[campo]) - 1) buf[campo][bi++] = '"'; p += 2; continue; }
                    else { p++; aspas = 0; break; }
                } else { if (bi < sizeof(buf[campo]) - 1) buf[campo][bi++] = *p; p++; }
            } else {
                if (*p == ';' || *p == '\n' || *p == '\r' || *p == '\0') { break; }
                if (bi < sizeof(buf[campo]) - 1) buf[campo][bi++] = *p;
                p++;
            }
        }
        buf[campo][bi] = '\0';
        if (*p == ';') p++; // pular separador
    }
    // Campo edição (inteiro)
    while (*p == ' ') p++;
    char ed[32]; size_t ei = 0;
    int aspas = 0;
    if (*p == '"') { aspas = 1; p++; }
    while (*p) {
        if (aspas) {
            if (*p == '"') { if (*(p+1) == '"') { if (ei < sizeof(ed)-1) ed[ei++] = '"'; p += 2; continue; } else { p++; aspas = 0; break; } }
            else { if (ei < sizeof(ed)-1) ed[ei++] = *p; p++; }
        } else {
            if (*p == ';' || *p == '\n' || *p == '\r' || *p == '\0') { break; }
            if (ei < sizeof(ed)-1) ed[ei++] = *p; p++;
        }
    }
    ed[ei] = '\0';

    // Copiar para saída
    strncpy(out_nome, buf[0], n1); out_nome[n1-1] = '\0';
    strncpy(out_autor, buf[1], n2); out_autor[n2-1] = '\0';
    strncpy(out_editora, buf[2], n3); out_editora[n3-1] = '\0';
    char *endptr = NULL; long v = strtol(ed, &endptr, 10);
    if (endptr == ed) return 0;
    *out_edicao = (int)v;
    return 1;
}

static int carregar_csv(const char *caminho, Livro **livros_out, int *quantidade_out) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        perror("Erro ao abrir arquivo para leitura");
        return 0;
    }
    char linha[1024];
    // pula cabeçalho
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }
    int capacidade = 16;
    int qtd = 0;
    Livro *vet = (Livro *)malloc(sizeof(Livro) * capacidade);
    if (!vet) { fclose(f); return 0; }
    while (fgets(linha, sizeof(linha), f)) {
        remove_nova_linha(linha);
        if (qtd == capacidade) {
            capacidade *= 2;
            Livro *tmp = (Livro *)realloc(vet, sizeof(Livro) * capacidade);
            if (!tmp) { free(vet); fclose(f); return 0; }
            vet = tmp;
        }
        int ed = 0;
        if (!parse_csv_linha(linha, vet[qtd].nome, sizeof(vet[qtd].nome), vet[qtd].autor, sizeof(vet[qtd].autor), vet[qtd].editora, sizeof(vet[qtd].editora), &ed)) {
            continue; // linha inválida
        }
        vet[qtd].edicao = ed;
        qtd++;
    }
    fclose(f);
    *livros_out = vet;
    *quantidade_out = qtd;
    return 1;
}

// Helper: adiciona um livro ao vetor dinâmico
static void adicionar_livro(Livro **plivros, int *pqt, int *pcap) {
    if (*pcap == 0) {
        *pcap = 4;
        *plivros = (Livro *)malloc(sizeof(Livro) * (size_t)(*pcap));
        if (!*plivros) { fprintf(stderr, "Erro: memória insuficiente.\n"); *pcap = 0; return; }
    }
    if (*pqt >= *pcap) {
        int novo = (*pcap) * 2;
        Livro *tmp = (Livro *)realloc(*plivros, sizeof(Livro) * (size_t)novo);
        if (!tmp) { fprintf(stderr, "Erro: memória insuficiente.\n"); return; }
        *plivros = tmp;
        *pcap = novo;
    }
    Livro *lv = &(*plivros)[*pqt];
    printf("\n== Cadastro do Livro %d ==\n", *pqt + 1);
    ler_linha("Nome da obra: ", lv->nome, sizeof(lv->nome));
    ler_linha("Autor: ", lv->autor, sizeof(lv->autor));
    ler_linha("Editora: ", lv->editora, sizeof(lv->editora));
    lv->edicao = ler_inteiro("Edição (número inteiro): ");
    (*pqt)++;
}

#ifndef BIBLIOTECA_NO_MAIN
int main(void) {
    setlocale(LC_ALL, "");

    printf("==========================================\n");
    printf("  Sistema de Cadastro de Livros (C) v%s\n", BIBLIOTECA_VERSION);
    printf("==========================================\n\n");

    Livro *livros = NULL; int quantidade = 0; int capacidade = 0;

    while (1) {
        printf("\nMenu:\n");
        printf("  1) Adicionar livro\n");
        printf("  2) Listar livros\n");
        printf("  3) Salvar em CSV\n");
        printf("  4) Carregar de CSV (substitui lista)\n");
        printf("  5) Buscar por título\n");
        printf("  6) Buscar por autor\n");
        printf("  7) Ordenar (1-Nome, 2-Autor, 3-Edição)\n");
        printf("  0) Sair\n");
        char optbuf[8];
        ler_linha("Escolha: ", optbuf, sizeof(optbuf));
        char op = optbuf[0];

        if (op == '0' || op == 'q' || op == 'Q') {
            break;
        } else if (op == '1') {
            adicionar_livro(&livros, &quantidade, &capacidade);
        } else if (op == '2') {
            if (quantidade == 0) printf("\nNenhum livro cadastrado.\n");
            else imprimir_livros(livros, quantidade);
        } else if (op == '3') {
            if (quantidade == 0) { printf("\nNada para salvar.\n"); continue; }
            char caminho[256];
            ler_linha("Caminho do CSV para salvar (ex.: livros.csv): ", caminho, sizeof(caminho));
            if (caminho[0] == '\0') strcpy(caminho, "livros.csv");
            if (salvar_csv(caminho, livros, quantidade)) printf("Arquivo '%s' salvo com sucesso.\n", caminho);
            else printf("Falha ao salvar '%s'.\n", caminho);
        } else if (op == '4') {
            char caminho[256];
            ler_linha("Caminho do CSV para carregar: ", caminho, sizeof(caminho));
            if (caminho[0] == '\0') { printf("Caminho inválido.\n"); continue; }
            Livro *novos = NULL; int nova_qtd = 0;
            if (carregar_csv(caminho, &novos, &nova_qtd)) {
                free(livros);
                livros = novos; quantidade = nova_qtd; capacidade = nova_qtd;
                printf("\nLista carregada de '%s' com %d livro(s).\n", caminho, quantidade);
            } else {
                printf("Falha ao carregar o CSV informado.\n");
            }
        } else if (op == '5') {
            char consulta[TAM_TEXTO];
            ler_linha("Digite o título (ou parte): ", consulta, sizeof(consulta));
            buscar_por_titulo(livros, quantidade, consulta);
        } else if (op == '6') {
            char consulta[TAM_TEXTO];
            ler_linha("Digite o autor (ou parte): ", consulta, sizeof(consulta));
            buscar_por_autor(livros, quantidade, consulta);
        } else if (op == '7') {
            printf("\nOrdenar por:\n  1) Nome\n  2) Autor\n  3) Edição\n");
            int opc = ler_inteiro("Escolha: ");
            if (opc == 1) { qsort(livros, (size_t)quantidade, sizeof(Livro), cmp_por_nome); printf("\nLista ordenada por nome:\n"); }
            else if (opc == 2) { qsort(livros, (size_t)quantidade, sizeof(Livro), cmp_por_autor); printf("\nLista ordenada por autor:\n"); }
            else if (opc == 3) { qsort(livros, (size_t)quantidade, sizeof(Livro), cmp_por_edicao); printf("\nLista ordenada por edição:\n"); }
            if (quantidade > 0) imprimir_livros(livros, quantidade);
        } else {
            printf("Opção inválida.\n");
        }
    }

    printf("\n==========================================\n");
    printf("Fim da execução.\n");

    free(livros);
    return 0;
}
#endif // BIBLIOTECA_NO_MAIN