@echo off
setlocal

set CC=gcc
set CFLAGS=-O2 -Wall -Wextra -std=c11

if not exist biblioteca.c (
  echo [ERRO] arquivo biblioteca.c nao encontrado.
  exit /b 1
)

%CC% %CFLAGS% biblioteca.c -o biblioteca.exe
if errorlevel 1 (
  echo [ERRO] Falha na compilacao.
  exit /b 1
)

echo [OK] Compilacao concluida. Execute: biblioteca.exe