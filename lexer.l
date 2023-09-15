/*
 * This file is part of the Riemann Project.
 * Developed by Tom Faulhaber for personal use.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>. 
 */

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

[A-Z][A-Za-z0-9_]* {
   yylval.sval = strdup(yytext);
   return FUNC;
}

[ \t] ;

[\n\r] return ENDL;

<<EOF>> return ENDL;

. {
   printf("WARNING: Unrecognized char ignored: %s\n", yytext);
}

%%

int yywrap() {
   return 1;
}

