TARGET = compilador

LEXER = lexer/lexer.l
PARSER = parser/parser.y
MAIN = scr/main.c

LEXER_C = lexer/lex.yy.c
PARSER_C = parser/parser.tab.c
PARSER_H = parser/parser.tab.h

CC = gcc
CFLAGS = -Wall -g

# Alvo principal do compilador
all: $(TARGET)

# Gerar os arquivos do parser
$(PARSER_C) $(PARSER_H): $(PARSER)
	bison -d -o $(PARSER_C) $(PARSER)

# Gerar o código do lexer
$(LEXER_C): $(LEXER) $(PARSER_H)
	flex -o $(LEXER_C) $(LEXER)

# Compilação do compilador
$(TARGET): $(MAIN) $(PARSER_C) $(LEXER_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(PARSER_C) $(LEXER_C)

# Alvo para rodar os testes
test: $(TARGET)
	$(CC) $(CFLAGS) -o bin/test_compilador testes/test_compilador.c -lcunit
	./bin/test_compilador

# Limpeza de arquivos gerados
clean:
	rm -f $(TARGET).exe $(LEXER_C) $(PARSER_C) $(PARSER_H)
	rm -f bin/test_compilador

# Criar diretórios de binários se necessário
$(shell mkdir -p bin)
