#include "defs.h"
#include "data.h"
#include "decl.h"

// 给定一个AST树，以递归的方式生成汇编代码
static int genAST(struct ASTnode *n) {
    int leftreg, rightreg;

    // 和解释器类似，以后序遍历的方式去扫描AST树,只是现在不是直接返回值，而是返回生成的汇编代码
    if (n->left) leftreg = genAST(n->left);
    if (n->right) rightreg = genAST(n->right);

    switch (n->op) {
        case A_ADD: return (cgadd(leftreg, rightreg));
        case A_SUBTRACT: return (cgsub(leftreg, rightreg));
        case A_MULTIPLY: return (cgmul(leftreg, rightreg));
        case A_DIVIDE: return (cgdiv(leftreg, rightreg));
        case A_INTLIT: return (cgload(n->intvalue));

        default:
            fprintf(stderr, "Error: Unknown AST operator %d.\n", n->op);
            exit(1);
    }
}

// 调用生成汇编的函数
void generatecode(struct ASTnode *n) {
    int reg;

    cgpreamble(); // 汇编前处理
    reg = genAST(n);  // 执行编译
    cgprintint(reg); // 打印最后的输出结果，int类型
    cgpostamble(); // 汇编后处理
}