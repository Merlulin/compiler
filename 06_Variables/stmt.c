#include "defs.h"
#include "data.h"
#include "decl.h"

void print_statement(void) {
    struct ASTnode *tree;
    int reg;
    
    match(T_PRINT, "print"); // 匹配首个print

    tree = binexpr(0);
    reg = genAST(tree, -1);
    genprintint(reg);
    genfreeregs();

    semi();
}


void assignment_statement(void) {
    struct ASTnode *left, *right, *tree;
    int id;

    ident();    // 确认是否是赋值标识符

    // 检查符号的名字是否存在在符号表中
    if ((id = findglob(Text)) == -1) {
        fatals("Undeclared variable", Text);
    }
    // 为何左变成右了呢，因为我们是将右侧的表达式结果赋值给左侧的变量地址当中，ASTnode构建需要反过来，以在编译时满足X86的指令要求
    right = mkastleaf(A_LVIDENT, id);

    match(T_EQUALS, "=");   // 确保是否有一个等号
    
    // 解析后面的表达式
    left = binexpr(0);

    tree = mkastnode(A_ASSIGN, left, right, 0);

    genAST(tree, -1);
    genfreeregs();

    semi();
}

// 解析语句
void statements(void) {

    while (1) {
        switch (Token.token) {
            case T_PRINT:   // 如果是打印语句
                print_statement();
                break;
            case T_INT: // 如果是变量定义
                var_declaration();
                break;
            case T_IDENT:   // 如果是赋值语句
                assignment_statement();
                break;
            case T_EOF:
                return;
            default:
                fatald("Error: Syntax error, token", Token.token);
        }
    }
}