%{
   #include "expr.h"
   #include "parser.h"
%}

%%

[0-9]+|[0-9]*\.[0-9]+ {
   yylval.fval = atof(yytext);
   return NUM;
}

[-+*/[\]()^] {
   return *yytext;
}

x|X {
   return VAR;
}

[ \t\n] ;

. {
   fprintf(stderr, "Cannot parse expr from input! Unrecognized char: %s\n", yytext);
   exit(EXIT_FAILURE);
}

%%

int yywrap() {
   return 1;
}
 
