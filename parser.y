%{
   #include "expr.h"
   #include "lexer.h"

   void yyerror(Expr **, char **, int *errc, const char ***errv, const char *s);
%}

%union {
   double fval;
   Expr *expr;
   char *sval;
}

%parse-param {Expr **root}
%parse-param {char **funcname}
%parse-param {int *errc}
%parse-param {const char ***errv}

%define parse.error detailed

%token NUM VAR FUNC ENDL

%nonassoc '=' '+' '-' '*' '/' '^'

%type <fval> NUM
%type <sval> FUNC
%type <expr> isolate pow cmul neg mul expr stmt

%%

stmt:
     FUNC '=' expr ENDL 
     {
         *funcname = $1;
         *root = $$ = $3;
         YYABORT;
     }
   | expr ENDL
     {
         *root = $$ = $1;
         YYABORT;
     }
   ;

isolate:
     VAR
      {
         $$ = new_var_expr();
      }
   | NUM
      {
         $$ = new_num_expr($1);
      }
   | '(' expr ')'
      {
         $$ = $2;
      }
   | '[' expr ']'
      {
         $$ = new_unary(ABS, $2);
      }
   | FUNC '(' expr ')'
      {
         $$ = new_apply($1, $3);
      }
   | FUNC '(' ')'
      {
         $$ = new_apply($1, new_num_expr(0));
      }
   ;

pow:
     isolate
      {
         $$ = $1;
      }
   | isolate '^' pow
      {
         $$ = new_binary(POW, $1, $3);
      }
   ;

cmul:
     pow
      {
         $$ = $1;
      }
   | cmul pow
      {
         $$ = new_binary(MUL, $1, $2);
      }
   ;
   
neg:
     cmul
      {
         $$ = $1;
      }
   | '-' cmul
      {
         $$ = new_unary(NEG, $2);
      }
   ;

mul:
     neg
      {
         $$ = $1;
      }
   | mul '*' neg
      {
         $$ = new_binary(MUL, $1, $3);
      }
   | mul '/' neg
      {
         $$ = new_binary(DIV, $1, $3);
      }
   ;

expr:
     mul
      {
         $$ = $1;
      }
   | expr '+' mul
      {
         $$ = new_binary(ADD, $1, $3);
      }
   | expr '-' mul
      {
         $$ = new_binary(SUB, $1, $3);
      }
   ;

%%

void yyerror(Expr **, char **, int *errc, const char ***errv, const char *s) {
   if (*errc == 0) {
      *errv = malloc(sizeof(char *));
   }
   else {
      *errv = realloc(*errv, (*errc + 1) * sizeof(char *));
   }

   *errv[*errc] = s;
   ++*errc;
}

