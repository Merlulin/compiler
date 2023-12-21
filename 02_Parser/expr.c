#include "defs.h"
#include "data.h"
#include "decl.h"

// 该函数实现将scanner的token与AST的节点op进行映射
int arithop(int tok) {
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
        fprintf(stderr, "Error: unknown token in arithop() on line %d.\n", Line);
        exit(1);
    }
}

// 检查下一个标记是否是整数文字，并构建一个对应的ASTnode来保存文字值
static struct ASTnode *primary(void) {
    struct ASTnode *n;
    // Token是一个全局的token，用于保存读取到的字符
    switch (Token.token) {
        // 只有是整数文字才创建AST叶子节点，否则都是不合法的语义
        case T_INTLIT:
            n = mkastleaf(A_INTLIT, Token.intvalue);
            scan(&Token);
            return (n);
        default:
            fprintf(stderr, "Error: syntax error on line %d.\n", Line);
            exit(1);
    }
}

// 返回根为二进制运算符的AST树, 就是构建一个AST树，先左后递归右最后中。（后序遍历的思想）
struct ASTnode *binexpr(void) {
    struct ASTnode *n, *left, *right;
    int nodetype;

    // 获取节点左侧的文字
    left = primary();
    // 如果没有下一个token了，就直接返回左节点
    if (Token.token == T_EOF)
        return (left);

    // nodetype变成token映射成ASTnode的标签
    nodetype = arithop(Token.token);
    // 继续获取下一个token
    scan(&Token);

    // 递归获取右子树
    right = binexpr();

    n = mkastnode(nodetype, left, right ,0);
    return (n);
}