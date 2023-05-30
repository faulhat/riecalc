%{
   #include "expr.h"
   #include "parser.h"
%}

%%

[0-9]+|[0-9]*\.[0-9]+ {
   yylval.fval = atof(yytext);
   return NUM;
}

[-+*/[\]()^=] return *yytext;

[A-Z][A-Za-z_]* {
   yylval.sval = strdup(yytext);
   return FUNC;
}

x return VAR;

[ \t] ;

[\n\r] return ENDL;

<<EOF>> return ENDL;

. {
   fprintf(stderr, "Cannot parse expr from input! Unrecognized char: %s\n", yytext);
   exit(EXIT_FAILURE);
}

%%

int yywrap() {
   return 1;
}
 
