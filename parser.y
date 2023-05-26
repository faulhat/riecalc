%{
   #include "expr.h"
%}

%union {
   float fval;
   Expr *expr;
}

%parse-param {Expr **root}

%token NUM

%nonassoc UMINUS
%left '+' '-'
%left '*' '/'
%left '^'

%type <fval> NUM
%type <expr> expr

%%

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
   ;

%%

void yyerror(char *s) {
   fprintf(stderr, "Error: %s\n", s);
}

int main() {
   Expr *expr = NULL;
   yyparse(&expr);
   if (expr != NULL) {
      print_expr(expr, stdout);
      printf(" = %.2f\n", eval_const_expr(expr));
   }
   else {
      fprintf(stderr, "Error: could not parse an expr from input.\n");
   }

   return 0;
}

