
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

```plaintext
compilador-lox/
├── parser/
│   ├── parser.y            # Gramática Bison
│   └── parser.tab.c        # Gerado pelo Bison
├── lexer/
│   ├── lexer.l             # Regras Flex
│   └── lex.yy.c            # Gerado pelo Flex
├── scr/
│   └── main.c              # Programa principal
├── Makefile                # Build automation
└── README.md               # Este arquivo
````

## ⚙️ Pré-requisitos

  * **GCC** (GNU Compiler Collection)
  * **Flex** (Analisador léxico)
  * **Bison** (Analisador sintático)
  * **make** (Opcional, mas recomendado)

## 📥 Instalação das Dependências

### Ubuntu/Debian:

```sh
sudo apt-get update
sudo apt-get install gcc flex bison make
```

### macOS:

```sh
# Com Homebrew
brew install gcc flex bison make

# Com MacPorts
sudo port install gcc flex bison make
```

### Windows (via WSL2 ou MinGW):

```sh
# No WSL2 (Ubuntu)
sudo apt-get install gcc flex bison make
```

## 🚀 Como Compilar

### Método 1: Usando Makefile (Recomendado)

```sh
# Compilar o projeto
make

# Ou especificamente
make all
```

### Método 2: Compilação Manual

```sh
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
```

## 🧪 Como Executar e Testar

### Modo Interativo

```sh
./compilador
```

**Exemplo de uso:**

```
Interpretador Lox - Digite expressões (Ctrl+D para sair):
> 3 + 4 * 2;
> var x = 10;
> print x + 5;
> Ctrl+D
```

### Executar com Arquivo

```sh
# Teste expressões
echo "3 + 4 * 2;" | ./compilador
echo "var x = 10; print x + 5;" | ./compilador
echo "5 > 3;" | ./compilador
```

## 🧪 Testes Automatizados

O projeto inclui testes automatizados para verificar se o compilador está funcionando corretamente. Para rodar os testes, siga as instruções abaixo.

**Certifique-se de que o compilador está compilado:**
Antes de rodar os testes, o compilador precisa ser compilado. Para isso, execute:

```sh
make
```

**Rodando os Testes Automatizados:**
Para rodar todos os testes, incluindo os de erros de sintaxe e execução, utilize o comando:

```sh
make test
```

Esse comando irá:

1.  Compilar o código do compilador.
2.  Rodar os testes automatizados.
3.  Comparar a saída gerada pelo compilador com a saída esperada.

**Testes Específicos:**
Se desejar rodar apenas um teste específico, como o `teste.lox`, utilize o seguinte comando:

```sh
make test_teste_lox
```

Isso irá rodar apenas o teste do arquivo `teste.lox`.

### 💡 Como os Testes Funcionam

Os testes são escritos em CUnit e verificam se a saída do compilador corresponde à saída esperada. O arquivo `test_compilador.c` automatiza o processo de execução do compilador e comparação da saída com a esperada para os seguintes arquivos de teste:

  * `teste.lox`: Testa operações aritméticas, expressões e funções.
  * `teste2.lox`: Testa declarações de variáveis, funções e estruturas de controle.
  * `teste3.lox`: Testa literais, laços e declarações de classes.
  * `teste_erros.lox`: Testa erros de sintaxe, como caracteres inválidos e operadores inesperados.

### 🧩 Estrutura dos Testes

O arquivo de testes contém funções como:

```c
void test_funcionalidade_lox() {
    // Testes de funcionalidade
    test_compilador_output("testes/teste.lox", "X não é maior que Y\n30\n-10\n11\n12\n2\n10\nfalse\ntrue\ntrue and false\ntrue\nX é maior que Y\nHello, world!\n15\nfalse\nfalse");
    
    // Teste de erro para 'teste_erros.lox'
    test_compilador_output("testes/teste_erros.lox", "Erro: Caractere '#' não reconhecido.\nErro: String não terminada.\nErro: Número com múltiplos pontos decimais...");
}
```

Esse código roda o compilador, captura a saída e a compara com a saída esperada para garantir que os testes passaram ou falharam corretamente.

## 📚 Exemplos de Código Lox

### Expressões Aritméticas

```lox
3 + 4 * 2;
(10 - 5) / 2;
var resultado = 42;
```

### Comparações e Lógicos

```lox
5 > 3;
10 == 10;
true and false;
```

### Strings e Saída

```lox
print "Hello World!";
var nome = "Lox";
print "Olá " + nome;
```

## 🧩 Comandos Úteis

```sh
# Compilar e limpar
make clean && make

# Apenas gerar lexer/parser
make parser/parser.tab.c
make lexer/lex.yy.c

# Verificar estrutura de arquivos
find . -name "*.y" -o -name "*.l" -o -name "*.c" -o -name "*.h"
```

## 🐛 Solução de Problemas

**Erro:** `library not found for -lfl`

**Solução (macOS):**

```sh
# Use -ll em vez de -lfl
gcc -o compilador parser/parser.tab.c lexer/lex.yy.c scr/main.c -ll

# Ou compile sem a flag
gcc -o compilador parser/parser.tab.c lexer/lex.yy.c scr/main.c
```

**Erro:** `parser.tab.h not found`

**Solução:**

```sh
# Certifique-se de gerar o parser primeiro
cd parser && bison -d parser.y && cd ..
```

**Erro de redefinição de tokens**

**Solução:**
Limpe e recompile tudo:

```sh
make clean
make
```

## 📊 Saída Esperada

**Entrada:**

```
3 + 4 * 2;
```

**Saída:**

```
NUMBER(3)
PLUS
NUMBER(4)
STAR
NUMBER(2)
SEMICOLON
```

## 🎯 Funcionalidades Implementadas

### Tokens Reconhecidos

  * **Palavras-chave:** `var`, `fun`, `if`, `else`, `for`, `while`, `print`, `return`
  * **Operadores:** `+`, `-`, `*`, `/`, `=`, `==`, `!=`, `<`, `>`, `<=`, `>=`
  * **Símbolos:** `(`, `)`, `{`, `}`, `,`, `.`, `;`
  * **Literais:** números, strings, `true`, `false`, `nil`
  * **Identificadores:** nomes de variáveis e funções

### Estruturas Suportadas

  * Expressões aritméticas
  * Declaração de variáveis
  * Chamadas de função básicas
  * Estruturas de controle (parcial)

## 👥 Equipe de Desenvolvimento

  * Ana Carolina Madeira Fialho
  * Julia Vitoria Freire Silva
  * Isaac Lucas Souza Bezarra
  * PEDRO RAMOS SOUSA REIS
  * CRISTIANO BORGES DE MORAIS

**Disciplina:**  Compiladores - 2025.2
**Instituição:**  [Universidade de Brasília]
