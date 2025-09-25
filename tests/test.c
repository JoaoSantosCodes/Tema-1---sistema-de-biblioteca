#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define BIBLIOTECA_NO_MAIN
#include "../biblioteca.c"

static int tests_run = 0;
static int tests_failed = 0;

static void expect_int(const char *label, int got, int expected) {
    tests_run++;
    if (got != expected) {
        tests_failed++;
        fprintf(stderr, "[FAIL] %s: got=%d expected=%d\n", label, got, expected);
    } else {
        printf("[OK]   %s\n", label);
    }
}

static void expect_str(const char *label, const char *got, const char *expected) {
    tests_run++;
    if (strcmp(got, expected) != 0) {
        tests_failed++;
        fprintf(stderr, "[FAIL] %s: got=\"%s\" expected=\"%s\"\n", label, got, expected);
    } else {
        printf("[OK]   %s\n", label);
    }
}

static void test_contains_substring_ci(void) {
    expect_int("contains_substring_ci - basic", contains_substring_ci("Biblioteca Central", "central"), 1);
    expect_int("contains_substring_ci - not found", contains_substring_ci("Estruturas de Dados", "xpto"), 0);
    expect_int("contains_substring_ci - accents treated as bytes", contains_substring_ci("João", "jo"), 1);
}

static void test_ordenação(void) {
    Livro v[3] = {
        {"Cálculo", "Ana", "EditA", 2},
        {"Algoritmos", "Bruno", "EditB", 3},
        {"Banco de Dados", "Carlos", "EditC", 1}
    };
    qsort(v, 3, sizeof(Livro), cmp_por_nome);
    expect_str("ordenação por nome [0]", v[0].nome, "Algoritmos");
    expect_str("ordenação por nome [1]", v[1].nome, "Banco de Dados");
    expect_str("ordenação por nome [2]", v[2].nome, "Cálculo");

    qsort(v, 3, sizeof(Livro), cmp_por_autor);
    expect_str("ordenação por autor [0]", v[0].autor, "Ana");
    expect_str("ordenação por autor [1]", v[1].autor, "Bruno");
    expect_str("ordenação por autor [2]", v[2].autor, "Carlos");

    qsort(v, 3, sizeof(Livro), cmp_por_edicao);
    expect_int("ordenação por edição [0]", v[0].edicao, 1);
    expect_int("ordenação por edição [2]", v[2].edicao, 3);
}

static void test_csv_roundtrip(void) {
    const char *path = "test_livros.csv";
    Livro in[2] = {
        {"Introdução ao C", "Kernighan & Ritchie", "Prentice Hall", 2},
        {"Estruturas de Dados", "Weiss", "Pearson", 3}
    };
    int ok = salvar_csv(path, in, 2);
    expect_int("salvar_csv", ok, 1);

    Livro *out = NULL; int qtd = 0;
    ok = carregar_csv(path, &out, &qtd);
    expect_int("carregar_csv ok", ok, 1);
    expect_int("carregar_csv qtd", qtd, 2);
    if (ok && qtd == 2 && out) {
        expect_str("csv[0] nome", out[0].nome, in[0].nome);
        expect_str("csv[0] autor", out[0].autor, in[0].autor);
        expect_str("csv[0] editora", out[0].editora, in[0].editora);
        expect_int("csv[0] edicao", out[0].edicao, in[0].edicao);
        expect_str("csv[1] nome", out[1].nome, in[1].nome);
        expect_str("csv[1] autor", out[1].autor, in[1].autor);
        expect_str("csv[1] editora", out[1].editora, in[1].editora);
        expect_int("csv[1] edicao", out[1].edicao, in[1].edicao);
        free(out);
    }
    remove(path);
}

static void test_csv_campos_especiais(void) {
    const char *path = "test_livros_escapes.csv";
    Livro in[3] = {
        {"Nome;Com;PontoEVirgula", "Autor;X", "Edit;ora", 10},
        {"Nome com \"aspas\" internas", "Autor \"Y\"", "Editora \"Z\"", 20},
        {" Espaços nas pontas ", "  Autor  ", " Editora ", 30}
    };
    int ok = salvar_csv(path, in, 3);
    expect_int("salvar_csv (escapes)", ok, 1);

    Livro *out = NULL; int qtd = 0;
    ok = carregar_csv(path, &out, &qtd);
    expect_int("carregar_csv ok (escapes)", ok, 1);
    expect_int("carregar_csv qtd (escapes)", qtd, 3);
    if (ok && qtd == 3 && out) {
        for (int i = 0; i < 3; ++i) {
            expect_str("escapes nome", out[i].nome, in[i].nome);
            expect_str("escapes autor", out[i].autor, in[i].autor);
            expect_str("escapes editora", out[i].editora, in[i].editora);
            expect_int("escapes edicao", out[i].edicao, in[i].edicao);
        }
        free(out);
    }
    remove(path);
}

int main(void) {
    printf("==== Testes biblioteca ====/\n");

    test_contains_substring_ci();
    test_ordenação();
    test_csv_roundtrip();
    test_csv_campos_especiais();

    printf("\nTotal: %d, Falhas: %d\n", tests_run, tests_failed);
    return tests_failed ? 1 : 0;
}