# Compilador da Linguagem Lox

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/seu-usuario/compilador-lox)

Um compilador completo para a linguagem Lox desenvolvido em C, implementando todas as fases de compilação desde análise léxica até geração de código intermediário TAC (Three-Address Code).

## 📋 Sobre o Projeto

### Visão Geral
Lox é uma linguagem de programação simples com sintaxe similar a JavaScript, criada por Bob Nystrom para o livro "Crafting Interpreters". Este compilador traduz código Lox para um formato executável.

### Características Principais
- ✅ **Análise Léxica Completa** - Scanner com Flex
- ✅ **Análise Sintática** - Parser LALR(1) com Bison  
- ✅ **Árvore Sintática Abstrata (AST)** - Representação hierárquica
- ✅ **Tabela de Símbolos** - Gerenciamento de escopos com hash table
- ✅ **Análise Semântica** - Verificação de tipos e contexto
- ✅ **Geração de Código** - Three-Address Code (TAC) intermediário
- ✅ **Sistema de Tipos** - Dinâmico com verificação estática
- ✅ **Orientação a Objetos** - Classes, métodos, herança
- ✅ **Controle de Fluxo** - If, For, While, Return
- ✅ **Funções** - Parâmetros, escopo, recursão
- ✅ **Tratamento de Erros** - Robustez e mensagens informativas


### Fluxo de Compilação

```
Código Fonte Lox 
     ↓
[Lexer] → Tokens
     ↓  
[Parser] → AST
     ↓
[Tabela de Símbolos] → Análise de Escopos
     ↓
[Análise Semântica] → Verificações
     ↓  
[CodeGen] → Three-Address Code
     ↓
Saída: AST + Tabela + TAC
```

## Componentes Técnicos Detalhados

### 1. Análise Léxica (lexer.l)

**Características:**
- Reconhecimento de 41 tokens diferentes
- Suporte a Unicode e caracteres especiais
- Tratamento de comentários de linha (`//`)
- Números inteiros e de ponto flutuante
- Strings com escape sequences
- Identificadores com verificações

**Tokens Implementados:**
```c
// Palavras-chave
AND, OR, CLASS, ELSE, FALSE, FOR, FUN, IF, NIL, PRINT, 
RETURN, SUPER, THIS, TRUE, VAR, WHILE

// Operadores  
EQUAL_EQUAL, EQUAL, BANG_EQUAL, BANG, LESS_EQUAL, LESS,
GREATER_EQUAL, GREATER, PLUS, MINUS, STAR, SLASH

// Símbolos
LPAREN, RPAREN, LBRACE, RBRACE, COMMA, DOT, SEMICOLON

// Literais
NUM, STRING, IDENTIFIER
```

### 2. Análise Sintática (parser.y)

**Gramática LALR(1):**
- 81 regras de produção
- 41 tokens terminais
- 33 não-terminais
- Precedência de operadores definida

**Construções Suportadas:**
```bnf
program         → statements

statements      → statement*

statement       → var_decl | print_stmt | return_stmt | if_stmt
                | while_stmt | for_stmt | expr_stmt | block
                | fun_decl | class_decl

var_decl        → "var" IDENTIFIER ( "=" expression )? ";"

fun_decl        → "fun" IDENTIFIER "(" parameters? ")" block

class_decl      → "class" IDENTIFIER "{" method* "}"

expression      → assignment

assignment      → ( call "." )? IDENTIFIER "=" assignment
                | logic_or

logic_or        → logic_and ( "or" logic_and )*

logic_and       → comparison ( "and" comparison )*

comparison      → addition ( ( ">" | ">=" | "<" | "<=" | "==" | "!=" ) addition )*

addition        → multiplication ( ( "+" | "-" ) multiplication )*

multiplication  → unary ( ( "*" | "/" ) unary )*

unary           → ( "!" | "-" ) unary | call

call            → primary ( "(" arguments? ")" | "." IDENTIFIER )*

primary         → NUM | STRING | IDENTIFIER | "true" | "false" 
                | "nil" | "this" | "(" expression ")"
```

### 3. Árvore Sintática Abstrata (AST)

**Tipos de Nós Implementados (23 tipos):**

```c
typedef enum {
    // Literais e Identificadores
    NODE_NUM, NODE_STRING, NODE_BOOL, NODE_NIL,
    NODE_IDENTIFIER, NODE_THIS,
    
    // Expressões
    NODE_UNARY_OP, NODE_BINARY_OP, NODE_LOGICAL_OP,
    NODE_ASSIGN, NODE_CALL, NODE_GET_ATTR, NODE_SET_ATTR,
    
    // Declarações e Statements
    NODE_VAR_DECL, NODE_PRINT_STMT, NODE_EXPR_STMT,
    NODE_BLOCK, NODE_IF_STMT, NODE_WHILE_STMT, NODE_FOR_STMT,
    NODE_FUN_DECL, NODE_CLASS_DECL, NODE_RETURN_STMT,
    NODE_PARAM
} NodeType;
```

**Exemplo de Estrutura de Dados:**
```c
typedef struct NoAST {
    NodeType type;
    int lineno;
    struct NoAST *next;  // Para listas encadeadas
    
    union {
        // Literais
        double number;
        char* string;
        int boolean;
        char* identifier;
        
        // Expressões
        struct { int op; NoAST *operand; } unary_op;
        struct { int op; NoAST *left, *right; } binary_op;
        
        // Declarações
        struct { char *name; NoAST *initializer; } var_decl;
        struct { NoAST *expression; } print_stmt;
        
        // Estruturas de controle
        struct { NoAST *condition, *then_branch, *else_branch; } if_stmt;
        struct { NoAST *initializer, *condition, *increment, *body; } for_stmt;
        
        // Funções e Classes
        struct { char *name; NoAST *params, *body; } fun_decl;
        struct { char *name; NoAST *methods; } class_decl;
        
        // Chamadas e Acessos
        struct { NoAST *target, *arguments; } call;
        struct { NoAST *object; char *name; } get_attr;
    } data;
} NoAST;
```

### 4. Tabela de Símbolos

**Implementação:**
- Tabela hash com encadeamento externo
- 101 buckets (TAM_HASH)
- Suporte a múltiplos escopos
- Busca hierárquica (escopo local → global)

**Estrutura do Símbolo:**
```c
typedef struct simbolo {
    char nome[32];      // Nome do símbolo
    char tipo[16];      // Tipo inferido
    int escopo;         // Nível do escopo
    int numParams;      // Para funções (-1 se não for função)
    struct simbolo *prox;  // Encadeamento
} Simbolo;
```

**Operações:**
```c
void tab_inicializar();                    // Inicializa tabela
void tab_entrarEscopo();                   // Incrementa escopo
void tab_sairEscopo();                     // Decrementa e limpa símbolos
void tab_inserirSimbolo(char *nome, char *tipo);  // Insere símbolo
Simbolo *tab_buscarSimboloLocal(char *nome);     // Busca no escopo atual
Simbolo *tab_buscarSimbolo(char *nome);          // Busca hierárquica
```

### 5. Análise Semântica

**Verificações Implementadas:**

1. **Declaração de Variáveis:**
   - Variáveis não declaradas
   - Redefinição no mesmo escopo
   - Inferência de tipos

2. **Verificação de Tipos:**
   - Operações aritméticas (números)
   - Concatenação de strings
   - Operações lógicas (booleanos)
   - Compatibilidade em atribuições

3. **Contexto de Funções:**
   - Número de parâmetros vs argumentos
   - Return em contextos válidos
   - Construtor `init` com validações

4. **Contexto de Classes:**
   - Uso de `this` apenas em métodos
   - Validação de retorno em construtores
   - Acesso a atributos

5. **Controle de Fluxo:**
   - Break/continue em loops
   - Condições booleanas

### 6. Geração de Código (TAC)

**Operações TAC Implementadas (24 operações):**

```c
typedef enum {
    // Operações Aritméticas
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV,
    
    // Operações de Cópia e Atribuição
    TAC_COPY,
    
    // Operações de Comparação
    TAC_EQ, TAC_NEQ, TAC_GT, TAC_GE, TAC_LT, TAC_LE,
    
    // Operações Lógicas
    TAC_AND, TAC_OR, TAC_NOT,
    
    // Operações Unárias
    TAC_NEG,
    
    // Controle de Fluxo
    TAC_LABEL, TAC_JUMP, TAC_JUMP_TRUE, TAC_JUMP_FALSE,
    
    // Funções e Chamadas
    TAC_CALL, TAC_RETURN, TAC_PARAM,
    
    // Orientação a Objetos
    TAC_GET_ATTR, TAC_SET_ATTR
} TacOp;
```

**Estrutura TAC:**
```c
typedef struct TacNode {
    TacOp op;           // Operação
    char *res;          // Resultado
    char *arg1;         // Operando 1
    char *arg2;         // Operando 2
    struct TacNode *next, *prev;  // Lista duplamente encadeada
} TacNode;
```

## Exemplos Detalhados

### Exemplo 1: Expressões Aritméticas

**Código Lox:**
```lox
var x = 10;
var y = 20;
var resultado = (x + y) * 2 - 5;
print resultado;
```

**AST Gerada:**
```
VarDecl: x
  Num: 10.000000
VarDecl: y  
  Num: 20.000000
VarDecl: resultado
  OpBinario: -
    OpBinario: *
      OpBinario: +
        Ident: x
        Ident: y
      Num: 2.000000
    Num: 5.000000
PrintStmt
  Ident: resultado
```

**TAC Gerado:**
```
t0 = 10
x = t0
t1 = 20
y = t1
t2 = x + y
t3 = t2 * 2
t4 = t3 - 5
resultado = t4
param resultado
call print
```

### Exemplo 2: Estruturas de Controle

**Código Lox:**
```lox
for (var i = 0; i < 10; i = i + 1) {
    if (i % 2 == 0) {
        print i;
    }
}
```

**TAC Gerado:**
```
t0 = 0
i = t0
L0:
t1 = i < 10
ifnot t1 jump L2
t2 = i % 2
t3 = t2 == 0
ifnot t3 jump L1
param i
call print
L1:
t4 = i + 1
i = t4
jump L0
L2:
```

### Exemplo 3: Funções e Recursão

**Código Lox:**
```lox
fun fatorial(n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

print fatorial(5);
```

**TAC Gerado:**
```
label fatorial
t0 = n <= 1
ifnot t0 jump L0
return 1
L0:
t1 = n - 1
param t1
t2 = call fatorial
t3 = n * t2
return t3

param 5
t4 = call fatorial
param t4
call print
```

### Exemplo 4: Classes e Objetos

**Código Lox:**
```lox
class Pessoa {
    init(nome, idade) {
        this.nome = nome;
        this.idade = idade;
    }
    
    fun apresentar() {
        print "Olá, meu nome é " + this.nome;
    }
}

var p = Pessoa("João", 25);
p.apresentar();
```

**TAC Gerado:**
```
label Pessoa_init
this.nome = nome
this.idade = idade
return

label Pessoa_apresentar
t0 = "Olá, meu nome é " + this.nome
param t0
call print
return

param "João"
param 25
t1 = call Pessoa
p = t1
param p
call Pessoa_apresentar
```

## Sistema de Build

### Makefile Completo

```makefile
# Compilador e Flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
FLEX = flex
BISON = bison

# Arquivos Fonte
SRC_AST = ast/ast.c
SRC_TABELA = tabela/tabela.c  
SRC_SEMANTICA = semantica/semantica.c
SRC_CODEGEN = codegen/codegen.c codegen/tac.c
SRC_PARSER = parser/parser.tab.c
SRC_LEXER = lexer/lex.yy.c
SRC_MAIN = scr/main.c

# Alvo Principal
all: compilador

# Geração do Parser
parser/parser.tab.c parser/parser.tab.h: parser/parser.y
	cd parser && $(BISON) -d parser.y

# Geração do Lexer
lexer/lex.yy.c: lexer/lexer.l
	cd lexer && $(FLEX) lexer.l

# Compilação Principal
compilador: $(SRC_PARSER) $(SRC_LEXER) $(SRC_AST) $(SRC_TABELA) $(SRC_SEMANTICA) $(SRC_CODEGEN) $(SRC_MAIN)
	$(CC) $(CFLAGS) -o $@ $^ -ll

# Limpeza
clean:
	rm -f parser/parser.tab.* lexer/lex.yy.c compilador

# Testes
test: compilador
	./compilador exemplos/teste.lox

.PHONY: all clean test
```

###  Makefile para macOS

**⚠️ Nota Importante para usuários macOS:** 
O macOS possui uma versão muito antiga do Bison (2.3) pré-instalada. Para compilar corretamente, é necessário usar o Bison do Homebrew.

#### Instalação das Dependências no macOS:
```bash
# Instalar Bison e Flex atualizados
brew install bison flex

# Configurar o PATH para usar o Bison do Homebrew
echo 'export PATH="/opt/homebrew/opt/bison/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc

# Verificar a versão (deve ser 3.8.2 ou superior)
bison --version
```


#### Uso no macOS:
```bash
# Compilar com Makefile específico para macOS
make -f Makefile.mac clean
make -f Makefile.mac

# Criar alias para facilitar
alias macmake='make -f Makefile.mac'
macmake clean && macmake

# Executar testes
macmake test
```

#### Problemas Comuns no macOS e Soluções:

1. **Erro "bison: command not found" ou versão antiga:**
   ```bash
   brew install bison
   export PATH="/opt/homebrew/opt/bison/bin:$PATH"
   ```

2. **Erro de linker com libl:**
   - Use `LDFLAGS = -lm -ll` no Makefile.mac

3. **Warnings de versão do macOS SDK:**
   - São inofensivos e não afetam a funcionalidade

## Detalhes de Implementação

### 1. Gerenciamento de Memória

**Alocação:**
- Uso consistente de `malloc`/`free`
- Liberação recursiva da AST
- Cleanup de listas encadeadas
- Strings duplicadas com `strdup`

**Exemplo de Cleanup:**
```c
void liberarAST(NoAST *node) {
    if (!node) return;
    
    switch (node->type) {
        case NODE_STRING: 
            free(node->data.string); 
            break;
        case NODE_IDENTIFIER: 
            free(node->data.identifier); 
            break;
        case NODE_BINARY_OP:
            liberarAST(node->data.binary_op.left);
            liberarAST(node->data.binary_op.right);
            break;
        // ... outros casos
    }
    free(node);
}
```

### 2. Tratamento de Erros

**Sistema de Erros:**
- Erros léxicos: caracteres inválidos
- Erros sintáticos: construções malformadas  
- Erros semânticos: tipos, escopo, contexto
- Mensagens informativas com linha

**Exemplo:**
```c
void yyerror(const char *s) {
    fprintf(stderr, "Erro sintatico na linha %d: %s\n", yylineno, s);
}
```

### 3. Sistema de Tipos

**Inferência de Tipos:**
```c
char* obterNomeTipo(NoAST *no) {
    switch (no->type) {
        case NODE_NUM:
            return (no->data.number == floor(no->data.number)) ? "int" : "float";
        case NODE_STRING: return "string";
        case NODE_BOOL: return "bool";
        case NODE_NIL: return "nil";
        case NODE_IDENTIFIER: 
            // Busca na tabela de símbolos
            Simbolo *s = tab_buscarSimbolo(no->data.identifier);
            return s ? s->tipo : "dynamic";
        default: return "dynamic";
    }
}
```

# Sistema de Testes Automatizados

## Visão Geral dos Testes

O compilador inclui um **sistema completo de testes automatizados** que verifica todas as funcionalidades implementadas através de testes de integração.


### Ferramentas de Automação

#### 1. `run_tests.py` - Executor de Testes

**Funcionalidades:**
- ✅ **Execução automatizada** de todos os testes
- ✅ **Comparação inteligente** entre saída atual e esperada
- ✅ **Relatório detalhado** com sucessos e falhas
- ✅ **Execução seletiva** de testes específicos
- ✅ **Múltiplos modos** de verificação (sintaxe, erros, etc.)
- ✅ **Detecção de regressões** automática

**Uso:**
```bash
# Executar todos os testes
python tests/run_tests.py

# Executar testes específicos
ONLY="teste1,teste2" python tests/run_tests.py
```

#### 2. `update_expected.py` - Gerenciador de Expectativas

**Funcionalidades:**
- ✅ **Geração automática** de arquivos de expectativa
- ✅ **Atualização em lote** de todos os testes
- ✅ **Sincronização** entre código e resultados esperados
- ✅ **Facilita manutenção** dos testes

**Uso:**
```bash
# Atualizar todas as expectativas
python tests/update_expected.py
```

### Fluxo de Testes Automatizado

1. **Descoberta Automática** - Localiza todos os arquivos `teste*.lox`
2. **Execução em Lote** - Compila cada teste com o compilador
3. **Comparação de Resultados** - Diferença entre saída atual e esperada
4. **Relatório Consolidado** - Sumário com estatísticas de sucesso/falha

### Integração com Desenvolvimento

Os testes estão **totalmente integrados** ao processo de desenvolvimento:

```bash
# Desenvolvimento normal
make && make test

# Quando comportamento muda legitimamente
python tests/update_expected.py

# Debug de teste específico
ONLY="teste_problema" python tests/run_tests.py
```

### Cobertura dos Testes

Os testes automatizados verificam:

- ✅ **Análise Léxica e Sintática**
- ✅ **Árvore Sintática Abstrata (AST)**
- ✅ **Tabela de Símbolos e Escopos**
- ✅ **Análise Semântica e Tipos**
- ✅ **Geração de Código TAC**
- ✅ **Casos de Erro e Recuperação**
- ✅ **Funcionalidades de Linguagem Completa**

### Vantagens do Sistema

- **Feedback Imediato** - Problemas são detectados rapidamente
- **Prevenção de Regressões** - Mudanças não quebram funcionalidades existentes
- **Manutenção Simplificada** - Atualização automática de expectativas
- **Cobertura Abrangente** - Todas as fases do compilador são testadas
- **Integração Contínua** - Pronto para CI/CD

O sistema de testes garante que o compilador **funcione corretamente** após cada modificação e fornece **confiança** para evolução do código.

## Equipe de Desenvolvimento

  * Ana Carolina Madeira Fialho
  * Julia Vitoria Freire Silva
  * Isaac Lucas Souza Bezarra
  * Pedro Ramos Sousa Reis
  * Cristiano Broges De Morais

**Disciplina:**  Compiladores - 2025.2
**Instituição:**  [Universidade de Brasília]
