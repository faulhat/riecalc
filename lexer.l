%{
   #include "expr.h"
   #include "parser.h"
   #include <math.h>
%}

%%

[0-9]+|[0-9]*\.[0-9]+ {
   yylval.fval = atof(yytext);
   return NUM;
}

[-+*/[\]()^=] return *yytext;

x return ARG;

pi {
   yylval.fval = M_PI;
   return NUM;
}

e {
   yylval.fval = M_E;
   return NUM;
}

[a-z][0-9_]* {
   yylval.sval = strdup(yytext);
   return VAR;
}

[A-Z][a-z0-9_]* {
   yylval.sval = strdup(yytext);
   return FUNC;
}

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
 
