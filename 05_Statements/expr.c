#include "defs.h"
#include "data.h"
#include "decl.h"


static int arithop(int tok) {
  switch (tok) {
  case T_PLUS:
    return (A_ADD);
  case T_MINUS:
    return (A_SUBTRACT);
  case T_STAR:
    return (A_MULTIPLY);
  case T_SLASH:
    return (A_DIVIDE);
  default:
    fprintf(stderr, "syntax error on line %d, token %d\n", Line, tok);
    exit(1);
  }
}

// 获取真正的整型自变量
struct ASTnode* primary(void) {
    struct ASTnode *n;

    switch (Token.token) {
        case T_INTLIT:
            n = mkastleaf(A_INTLIT, Token.intvalue);
            scan(&Token);
            return (n);
        default:
            fprintf(stderr, "Error: syntax error on line %d, token %d\n", Line, Token.token);
            exit(1);
    }
}

// 使用Pratt Parser重写解释器的表达式优先级

// 为每一个符号生成对应的优先级(EOF, +, -, *, /, INTLIT)
static int OpPrec[] = { 0, 10, 10, 20, 20, 0}; 

// 为什么再加一个函数，为了错误提示
static int op_precedence(int tokentype) {
    int prec = OpPrec[tokentype];
    if (prec == 0) {
        fprintf(stderr, "Error: syntax error on line %d, token %d\n", Line, tokentype);
        exit(1);
    }
    return (prec);
}

// 返回一个AST树，ptp是上一个运算符的优先级
struct ASTnode *binexpr(int ptp) {
    struct ASTnode *left, *right;
    int tokentype;

    left = primary();
    tokentype = Token.token;
    if (tokentype == T_SEMI) 
        return (left);
    
    // 如果当前符号的优先级大于上一个符号的优先级，那么就先构建当前子树
    while (op_precedence(tokentype) > ptp) {
        scan(&Token);

        // 递归构建右侧子树
        right = binexpr(OpPrec[tokentype]);

        left = mkastnode(arithop(tokentype), left, right, 0);

        tokentype = Token.token;
        if (tokentype == T_SEMI) 
            return (left);
    }

    // 如果当前符号的优先级小，那么直接返回当前以及构建好的子树
    return (left);
}