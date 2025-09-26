# Compilador Lox Simplificado

Compilador básico da linguagem Lox usando **Flex** e **Bison**.

## Requisitos

- Flex 2.6.4
- Bison 3.8.2
- GCC ou outro compilador C
- Make

## Compilação

```bash
make          # gera compilador.exe
make clean    # limpa arquivos gerados
```

## Execução

Testando interativamente:
```bash
./compilador.exe
```
Digite instruções no console e pressione Enter.

Testando arquivo .lox:
```bash
./compilador.exe arquivo.lox
```
Analisa o arquivo e reporta erros sintáticos.