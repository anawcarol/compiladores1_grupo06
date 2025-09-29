# Compilador da Linguagem Lox

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/seu-usuario/compilador-lox)

Um compilador para a linguagem Lox desenvolvido em C usando Flex e Bison. Este projeto implementa as fases de análise léxica e sintática de um compilador completo.

## 📋 Sobre o Projeto

Lox é uma linguagem de programação simples com sintaxe similar a JavaScript, criada por Bob Nystrom para o livro "Crafting Interpreters". Este compilador traduz código Lox para um formato executável.

**Características implementadas:**
- ✅ Analisador léxico completo (Scanner)
- ✅ Analisador sintático (Parser) 
- ✅ Suporte a expressões matemáticas
- ✅ Declaração de variáveis
- ✅ Estruturas de controle básicas
- ✅ Sistema de tipos simples

## 🏗️ Estrutura do Projeto
compilador-lox/
├── parser/
│ ├── parser.y # Gramática Bison
│ └── parser.tab.c # Gerado pelo Bison
├── lexer/
│ ├── lexer.l # Regras Flex
│ └── lex.yy.c # Gerado pelo Flex
├── scr/
│ └── main.c # Programa principal
├── Makefile # Build automation
└── README.md # Este arquivo

text

## ⚙️ Pré-requisitos

- **GCC** (GNU Compiler Collection)
- **Flex** (Analisador léxico)
- **Bison** (Analisador sintático)
- **make** (Opcional, mas recomendado)

### 📥 Instalação das Dependências

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install gcc flex bison make
macOS:

bash
# Com Homebrew
brew install gcc flex bison make

# Com MacPorts
sudo port install gcc flex bison make
Windows (via WSL2 ou MinGW):

bash
# No WSL2 (Ubuntu)
sudo apt-get install gcc flex bison make
🚀 Como Compilar
Método 1: Usando Makefile (Recomendado)
bash
# Compilar o projeto
make

# Ou especificamente
make all
Método 2: Compilação Manual
bash
# 1. Gerar o parser
cd parser
bison -d parser.y
cd ..

# 2. Gerar o lexer
cd lexer
flex lexer.l
cd ..

# 3. Compilar tudo
gcc -o compilador parser/parser.tab.c lexer/lex.yy.c scr/main.c -ll
🧪 Como Executar e Testar
Modo Interativo
bash
./compilador
Exemplo de uso:

text
Interpretador Lox - Digite expressões (Ctrl+D para sair):
> 3 + 4 * 2;
> var x = 10;
> print x + 5;
> Ctrl+D
Executar com Arquivo
bash

# Teste expressões
echo "3 + 4 * 2;" | ./compilador
echo "var x = 10; print x + 5;" | ./compilador
echo "5 > 3;" | ./compilador
📚 Exemplos de Código Lox
Expressões Aritméticas
lox
3 + 4 * 2;
(10 - 5) / 2;
var resultado = 42;
Comparações e Lógicos
lox
5 > 3;
10 == 10;
true and false;
Strings e Saída
lox
print "Hello World!";
var nome = "Lox";
print "Olá " + nome;
🧩 Comandos Úteis
bash
# Compilar e limpar
make clean && make


# Apenas gerar lexer/parser
make parser/parser.tab.c
make lexer/lex.yy.c

# Verificar estrutura de arquivos
find . -name "*.y" -o -name "*.l" -o -name "*.c" -o -name "*.h"
🐛 Solução de Problemas
Erro: library not found for -lfl

Solução (macOS):

bash
# Use -ll em vez de -lfl
gcc -o compilador parser/parser.tab.c lexer/lex.yy.c scr/main.c -ll

# Ou compile sem a flag
gcc -o compilador parser/parser.tab.c lexer/lex.yy.c scr/main.c
Erro: parser.tab.h not found
Solução:

bash
# Certifique-se de gerar o parser primeiro
cd parser && bison -d parser.y && cd ..
Erro de redefinição de tokens
Solução: Limpe e recompile tudo:

bash
make clean
make
📊 Saída Esperada
Entrada:

lox
3 + 4 * 2;
Saída:

text
NUMBER(3)
PLUS
NUMBER(4)
STAR
NUMBER(2)
SEMICOLON
🎯 Funcionalidades Implementadas
Tokens Reconhecidos
Palavras-chave: var, fun, if, else, for, while, print, return

Operadores: +, -, *, /, =, ==, !=, <, >, <=, >=

Símbolos: (, ), {, }, ,, ., ;

Literais: números, strings, true, false, nil

Identificadores: nomes de variáveis e funções

Estruturas Suportadas
Expressões aritméticas

Declaração de variáveis

Chamadas de função básicas

Estruturas de controle (parcial)

👥 Equipe de Desenvolvimento
- [Ana Carolina Madeira Fialho](https://github.com/anawcarol)  
- [Julia Vitoria Freire Silva](https://github.com/Juhvitoria4)  
- [Isaac Lucas Souza Bezarra](https://github.com/IsaacLusca)  
- [PEDRO RAMOS SOUSA REIS](https://github.com/PedroRSR)  
- [CRISTIANO BORGES DE MORAIS](https://github.com/CristianoMoraiss)  

Disciplina: Compiladores - 2025.2
Instituição: [Universidade de Brasília]
