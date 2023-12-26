#include "defs.h"
#include "data.h"
#include "decl.h"

static struct ASTnode *print_statement(void) {
    struct ASTnode *tree;
    int reg;
    
    match(T_PRINT, "print"); // 匹配首个print

    tree = binexpr(0);
    // 直接将print作为一个AST树的做左子树，等待返回后与其他语句拼接
    tree = mkastunary(A_PRINT, tree, 0);
    semi();
    return (tree);
}


static struct ASTnode *assignment_statement(void) {
    struct ASTnode *left, *right, *tree;
    int id;

    ident();    // 确认是否是赋值标识符

    // 检查符号的名字是否存在在符号表中
    if ((id = findglob(Text)) == -1) {
        fatals("Undeclared variable", Text);
    }
    // 为何左变成右了呢，因为我们是将右侧的表达式结果赋值给左侧的变量地址当中，ASTnode构建需要反过来，以在编译时满足X86的指令要求
    right = mkastleaf(A_LVIDENT, id);

    match(T_ASSIGN, "=");   // 确保是否有一个等号
    
    // 解析后面的表达式
    left = binexpr(0);

    tree = mkastnode(A_ASSIGN, left, NULL, right, 0);

    semi();
    return (tree);
}


struct ASTnode *if_statement(void) {
    // 创建条件AST树、情况为真的AST树和为假的AST树
    struct ASTnode *condAST, *trueAST, *falseAST = NULL;

    // 匹配首个if关键字
    match(T_IF, "if");
    // 匹配左侧的(
    lparen(); 

    // 递归构造if语句条件表达式的AST树
    condAST = binexpr(0);

    if (condAST->op < A_EQ || condAST->op > A_GE)
        fatal("Bad comparison operator");
    rparen();

    // if为真的ast树
    trueAST = compound_statement();

    if (Token.token == T_ELSE) {
        scan(&Token);
        falseAST = compound_statement();
    }

    return (mkastnode(A_IF, condAST, trueAST, falseAST, 0));
}

// 解析复合语句
struct ASTnode *compound_statement(void) {
    struct ASTnode *left = NULL;
    struct ASTnode *tree;

    lbrace(); // 识别左括号

    while (1) {
        switch (Token.token) {
            case T_PRINT:   // 如果是打印语句
                tree = print_statement();
                break;
            case T_INT: // 如果是变量定义
                var_declaration();
                tree = NULL;
                break;
            case T_IDENT:   // 如果是赋值语句
                tree = assignment_statement();
                break;
            case T_IF:
                tree = if_statement();
                break;
            case T_RBRACE:  // 识别右括号
                rbrace(); 
                return (left);
            default:
                fatald("Error: Syntax error, token", Token.token);
        }

        if (tree) {
            if (left == NULL)
                left = tree;
            else 
                left = mkastnode(A_GLUE, left, NULL, tree, 0);  // 如果之前已经有了语句则直接做语句的拼接
        }
    }
}