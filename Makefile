all: scanner parser
	gcc -o exprtest expr.c lex.yy.c parser.tab.c -lfl -lm

scanner:
	flex lexer.l

parser:
	bison -d parser.y
