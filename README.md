# Sistema de Biblioteca em C (Tema 1 – Estruturas de Dados)

Este projeto implementa um sistema de cadastro de livros em C utilizando `struct` para armazenar as informações: nome, autor, editora e edição. Inclui entrada segura de strings, validação de inteiros, listagem dos livros e funcionalidades avançadas de persistência em CSV, busca e ordenação.

## Funcionalidades
- Cadastro de livros usando `struct` (nome, autor, editora, edição)
- Entrada segura de texto e validação da edição como inteiro
- Listagem dos livros cadastrados
- Alocação dinâmica de memória
- Suporte a acentuação com `setlocale`
- Persistência em CSV (salvar e carregar)
- Busca case-insensitive por título e autor
- Ordenação por nome, autor ou edição

## Requisitos
- Compilador C compatível com C11 (GCC/MinGW, MSVC, Clang)

## Compilação e execução

### Windows (GCC/MinGW)
1. Instale o MinGW.
2. Compile:
   gcc -O2 -Wall -Wextra -std=c11 biblioteca.c -o biblioteca
3. Execute:
   .\biblioteca.exe

### Windows (MSVC – Developer Command Prompt)
1. Abra o Developer Command Prompt for VS.
2. Compile:
   cl /W4 /Fe:biblioteca.exe biblioteca.c
3. Execute:
   biblioteca.exe

### Linux (Ubuntu/Debian)
1. Instale o GCC:
   sudo apt-get update && sudo apt-get install -y build-essential
2. Compile:
   gcc -O2 -Wall -Wextra -std=c11 biblioteca.c -o biblioteca
3. Execute:
   ./biblioteca

## Uso
- Informe o número de livros que deseja cadastrar.
- Preencha os campos: nome, autor, editora e edição.
- Utilize as opções do programa para salvar/carregar dados em CSV, realizar buscas e ordenar a lista.
- O arquivo CSV é salvo no diretório atual (nome configurado no código).

## Integração contínua (CI)
Este repositório possui um workflow do GitHub Actions que compila o programa em um ambiente Ubuntu utilizando GCC.
- Arquivo do workflow: .github/workflows/ci.yml
- O job de CI verifica se o código compila com as flags de warnings (-Wall -Wextra) e C11.

## Estrutura do projeto
- biblioteca.c — código-fonte do sistema
- README.md — este documento
- .gitignore — arquivos e pastas ignorados no versionamento
- .github/workflows/ci.yml — pipeline de CI para compilação

## Observações
- O programa foi desenvolvido visando compatibilidade imediata de compilação.
- Para persistência em CSV, garanta permissão de escrita no diretório de execução.
- Caso tenha problemas de acentuação no terminal do Windows, defina: chcp 65001 e use a fonte Consolas.