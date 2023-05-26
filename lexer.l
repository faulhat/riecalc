%{
   #include "expr.h"
   #include "parser.tab.h"
%}

%%

[0-9]+|[0-9]*\.[0-9]+ {
   yylval.fval = atof(yytext);
   return NUM;
}

[-+*/[\]()^] {
   return *yytext;
}

. ;

%%

int yywrap() {
   return 1;
}
 
