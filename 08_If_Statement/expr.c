#include "defs.h"
#include "data.h"
#include "decl.h"


static int arithop(int tokentype) {
  // 只要在范围内就返回对应的token，因为在该范围内token和操作token一一对应了
  if (tokentype > T_EOF && tokentype < T_INTLIT)
    return(tokentype);
  fatald("Syntax error, token", tokentype);
}

// 获取真正的整型自变量
static struct ASTnode* primary(void) {
    struct ASTnode *n;
    int id;

    switch (Token.token) {
        case T_INTLIT:
            n = mkastleaf(A_INTLIT, Token.intvalue);
            break;
        case T_IDENT:
            id = findglob(Text);
            if (id == -1)
              fatals("Unknown variation", Text);
            n = mkastleaf(A_IDENT, id);
            break;
        default:
            fprintf(stderr, "Error: syntax error on line %d, token %d\n", Line, Token.token);
            exit(1);
    }

    scan(&Token);
    return (n);
}

// 使用Pratt Parser重写解释器的表达式优先级

// 为每一个符号生成对应的优先级
static int OpPrec[] = {
  0, 10, 10,                    // T_EOF, T_PLUS, T_MINUS
  20, 20,                       // T_STAR, T_SLASH
  30, 30,                       // T_EQ, T_NE
  40, 40, 40, 40                // T_LT, T_GT, T_LE, T_GE
};

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

        left = mkastnode(arithop(tokentype), left, NULL, right, 0);

        tokentype = Token.token;
        if (tokentype == T_SEMI || tokentype == T_RPAREN) 
            return (left);
    }

    // 如果当前符号的优先级小，那么直接返回当前以及构建好的子树
    return (left);
}