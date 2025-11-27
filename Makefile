TARGET = compilador

LEXER = lexer/lexer.l
PARSER = parser/parser.y
MAIN = scr/main.c

AST_C = ast/ast.c
TABELA_C = tabela/tabela.c
SEMANTICA_C = semantica/semantica.c
CODEGEN_C = codegen/tac.c codegen/codegen.c
BACKEND_C = backend/backend_c.c

LEXER_C = lexer/lex.yy.c
PARSER_C = parser/parser.tab.c
PARSER_H = parser/parser.tab.h
SAIDA = saida.c

CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -lm

all: $(TARGET)

$(PARSER_C) $(PARSER_H): $(PARSER)
	bison -d -v -o $(PARSER_C) $(PARSER)

$(LEXER_C): $(LEXER) $(PARSER_H)
	flex -o $(LEXER_C) $(LEXER)

# Adicionado $(BACKEND_C) na linha de compilação
$(TARGET): $(MAIN) $(PARSER_C) $(LEXER_C) $(AST_C) $(TABELA_C) $(SEMANTICA_C) $(CODEGEN_C) $(BACKEND_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(PARSER_C) $(LEXER_C) $(AST_C) $(TABELA_C) $(SEMANTICA_C) $(CODEGEN_C) $(BACKEND_C) $(LDFLAGS)

clean:
	rm -f $(TARGET) $(LEXER_C) $(PARSER_C) $(PARSER_H) $(SAIDA) parser/parser.output
	rm -f *.o *.out saida.c saida