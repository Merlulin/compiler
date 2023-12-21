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

// 创建加法表达式
struct ASTnode *additive_expr(void) {
    struct ASTnode *left, *right;
    int tokentype;

    // 获得左侧的表达式值，应为要避免一开始就是乘法的情况，所以先调用一次乘法运算作为左侧值
    left = multiplicative_expr();                        
    // 获取当前Token的类型
    tokentype = Token.token;
    if (tokentype == T_EOF)
        // 如果当前读到的字符是EOF，则说明以及读到结尾了，直接返回左表达式就行了
        return left;
    // 循环构建AST树
    while (1) {
        scan(&Token);
        // 避免中间（右侧）有比当前操作更高级的优先级， 如果他不是更高优先级，就会返回。
        right = multiplicative_expr();
        // 继续从左开始迭代向上， 
        left = mkastnode(arithop(tokentype), left, right, 0);

        tokentype = Token.token;
        if (tokentype == T_EOF) 
            break;
    }
    // left存了最后的顶点
    return (left);
}

// 乘法表达式，只有在遇到加法操作采回返回
struct ASTnode *multiplicative_expr(void) {
    struct ASTnode *left, *right;
    int tokentype;

    // 获取左侧的字面值
    left = primary();

    tokentype = Token.token;
    if (tokentype == T_EOF)
        return (left);

    while ((tokentype == T_STAR) || (tokentype == T_SLASH)) {
        scan(&Token);
        right = primary();

        left = mkastnode(arithop(tokentype), left, right, 0);

        tokentype = Token.token;
        if (tokentype == T_EOF) 
            break;
    }
    return (left);
}