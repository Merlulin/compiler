#include "defs.h"
#include "data.h"
#include "decl.h"

// 给定一个AST树，以递归的方式生成汇编代码
int genAST(struct ASTnode *n, int reg) {
    int leftreg, rightreg;

    // 和解释器类似，以后序遍历的方式去扫描AST树,只是现在不是直接返回值，而是返回生成的汇编代码
    if (n->left) leftreg = genAST(n->left, -1);
    if (n->right) rightreg = genAST(n->right, leftreg);

    switch (n->op) {
        case A_ADD: 
            return (cgadd(leftreg, rightreg));
        case A_SUBTRACT: 
            return (cgsub(leftreg, rightreg));
        case A_MULTIPLY: 
            return (cgmul(leftreg, rightreg));
        case A_DIVIDE: 
            return (cgdiv(leftreg, rightreg));
        case A_INTLIT: 
            return (cgload(n->v.intvalue));
        case A_IDENT: 
            return (cgloadglob(Gsym[n->v.id].name));
        case A_LVIDENT: 
            return (cgstorglob(reg, Gsym[n->v.id].name));
        case A_ASSIGN:
            // 工作已经完成返回结果
            return (rightreg);
        default:
            fatald("Unknown AST operator %d.\n", n->op);
            exit(1);
    }
}

void genpreamble() {
    cgpreamble();
}

void genpostamble() {
    cgpostamble();
}

void genfreeregs() {
    freeall_registers();
}

void genprintint(int reg) {
    cgprintint(reg);
}

void genglobsym(char *s) {
    cgglobsym(s);
}