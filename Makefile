TARGET = compilador

LEXER = lexer/lexer.l
PARSER = parser/parser.y
MAIN = scr/main.c

LEXER_C = lexer/lex.yy.c
PARSER_C = parser/parser.tab.c
PARSER_H = parser/parser.tab.h

CC = gcc
CFLAGS = -Wall -g

all: $(TARGET)

$(PARSER_C) $(PARSER_H): $(PARSER)
	bison -d -o $(PARSER_C) $(PARSER)

$(LEXER_C): $(LEXER) $(PARSER_H)
	flex -o $(LEXER_C) $(LEXER)

$(TARGET): $(MAIN) $(PARSER_C) $(LEXER_C)
	$(CC) $(CFLAGS) -o $(TARGET) $(MAIN) $(PARSER_C) $(LEXER_C)

clean:
# 	del $(TARGET).exe $(LEXER_C) $(PARSER_C) $(PARSER_H)
	rm -f $(TARGET).exe $(LEXER_C) $(PARSER_C) $(PARSER_H)
# 	powershell -Command "Remove-Item -Force $(TARGET).exe, $(LEXER_C), $(PARSER_C), $(PARSER_H)"
