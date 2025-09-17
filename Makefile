TARGET = compilador

LEXER = lexer/lexer.l
PARSER = parser/parser.y
MAIN = scr/main.c

LEXER_C = lex.yy.c
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h

CC = gcc
CFLAGS = -Wall -g

all: $(TARGET)

$(PARSER_C) $(PARSER_H): $(PARSER)
	bison -d $(PARSER)

$(LEXER_C): $(LEXER)
	flex $(LEXER)

$(TARGET): $(MAIN) $(PARSER_C) $(LEXER_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(PARSER_C) $(LEXER_C)

clean:
	del $(TARGET).exe $(LEXER_C) $(PARSER_C) $(PARSER_H)
