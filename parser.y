%{
   #include "expr.h"
   #include "lexer.h"
%}

%union {
   float fval;
   Expr *expr;
   char *sval;
}

%parse-param {Expr **root}
%parse-param {char **funcname}
%parse-param {int *errc}
%parse-param {const char ***errv}

%define parse.error detailed

%token NUM VAR FUNC ENDL

%nonassoc '='
%nonassoc UMINUS
%left '+' '-'
%left '*' '/'
%left '^'

%type <fval> NUM
%type <sval> FUNC
%type <expr> expr equation

%%

equation:
     FUNC '=' expr ENDL 
     {
         *funcname = $1;
         *root = $$ = $3;
         return *errc;
     }
   | expr ENDL
     {
         *root = $$ = $1;
         return *errc;
     }

expr:
     expr '+' expr
      {
         *root = $$ = new_binary(ADD, $1, $3);
      }
   | expr '-' expr
      {
         *root = $$ = new_binary(SUB, $1, $3);
      }
   | expr '*' expr
      {
         *root = $$ = new_binary(MUL, $1, $3);
      }
   | expr expr %prec '*'
      {
         *root = $$ = new_binary(MUL, $1, $2);
      }
   | expr '/' expr
      {
         *root = $$ = new_binary(DIV, $1, $3);
      }
   | expr '^' expr
      {
         *root = $$ = new_binary(POW, $1, $3);
      }
   | '-' expr %prec UMINUS
      {
         *root = $$ = new_unary(NEG, $2);
      }
   | FUNC '(' expr ')'
      {
         *root = $$ = new_apply($1, $3);
      }
   | '[' expr ']'
      {
         *root = $$ = new_unary(ABS, $2);
      }
   | '(' expr ')'
      {
         *root = $$ = $2;
      }
   | NUM
      {
         *root = $$ = new_num_expr($1);
      }
   | VAR
      {
         *root = $$ = new_var_expr();
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

