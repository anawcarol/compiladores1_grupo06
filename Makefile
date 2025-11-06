TARGET = compilador

LEXER = lexer/lexer.l
PARSER = parser/parser.y
MAIN = scr/main.c

# --- NOVO ---
# Adicionar os arquivos .c da AST e da Tabela
AST_C = ast/ast.c
TABELA_C = tabela/tabela.c
# --- FIM NOVO ---

LEXER_C = lexer/lex.yy.c
PARSER_C = parser/parser.tab.c
PARSER_H = parser/parser.tab.h

CC = gcc
CFLAGS = -Wall -g
# --- NOVO ---
# Adicionar a biblioteca matemática (-lm) para o 'atof' no lexer
LDFLAGS = -lm
# --- FIM NOVO ---

# Alvo principal do compilador
all: $(TARGET)

# Gerar os arquivos do parser
$(PARSER_C) $(PARSER_H): $(PARSER)
	# Adicionado -v para gerar o parser.output (bom para debug)
	bison -d -v -o $(PARSER_C) $(PARSER)

# Gerar o código do lexer
$(LEXER_C): $(LEXER) $(PARSER_H)
	flex -o $(LEXER_C) $(LEXER)

# Compilação do compilador
# --- MODIFICADO ---
# Adicionado $(AST_C), $(TABELA_C) e $(LDFLAGS)
$(TARGET): $(MAIN) $(PARSER_C) $(LEXER_C) $(AST_C) $(TABELA_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(PARSER_C) $(LEXER_C) $(AST_C) $(TABELA_C) $(LDFLAGS)
# --- FIM MODIFICADO ---

# Alvo para rodar os testes
test: $(TARGET)
	$(CC) $(CFLAGS) -o bin/test_compilador testes/test_compilador.c -lcunit
	./bin/test_compilador

# Limpeza de arquivos gerados
clean:
	# --- MODIFICADO ---
	# Limpa o alvo $(TARGET) (sem .exe) e o parser.output
	rm -f $(TARGET) $(LEXER_C) $(PARSER_C) $(PARSER_H) parser.output
	rm -f bin/test_compilador
# --- FIM MODIFICADO ---

# Criar diretórios de binários se necessário
$(shell mkdir -p bin)