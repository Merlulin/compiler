#include "defs.h"
#include "data.h"
#include "decl.h"

// 生成一个L跳转语句的id
static int label(void) {
    static int id = 1;
    return (id++);
}

static int genIFAST(struct ASTnode *n) {
    // 分别标识if语句错误，和if语句结束， 如果没有else就用Lfalse代替结束
    int Lfalse, Lend;

    Lfalse = label();
    if (n->right)
        Lend = label();

    // 生成condition内容
    genAST(n->left, Lfalse, n->op);
    genfreeregs();
    
    // 生成true内容
    genAST(n->mid, NOREG, n->op);
    genfreeregs();
    // 如果有else， 先生成else的跳转
    if (n->right) {
        cgjump(Lend);
    }
    // 生成if为真的内容
    cglabel(Lfalse);
    // 如果有else，生成else为真的内容
    if (n->right) {
        genAST(n->right, NOREG, n->op);
        genfreeregs();
        cglabel(Lend);
    }

    return (NOREG);
}

// 给定一个AST树，以递归的方式生成汇编代码
int genAST(struct ASTnode *n, int reg, int parentASTop) {
    int leftreg, rightreg;

    switch (n->op) {
        case A_IF:
            return (genIFAST(n));
        case A_GLUE:
            genAST(n->left, NOREG, n->op);
            genfreeregs();
            genAST(n->right, NOREG, n->op);
            genfreeregs();
            return (NOREG);
    }

    // 和解释器类似，以后序遍历的方式去扫描AST树,只是现在不是直接返回值，而是返回生成的汇编代码
    if (n->left) leftreg = genAST(n->left, NOREG, n->op);
    if (n->right) rightreg = genAST(n->right, leftreg, n->op);

    switch (n->op) {
        case A_ADD: 
            return (cgadd(leftreg, rightreg));
        case A_SUBTRACT: 
            return (cgsub(leftreg, rightreg));
        case A_MULTIPLY: 
            return (cgmul(leftreg, rightreg));
        case A_DIVIDE: 
            return (cgdiv(leftreg, rightreg));
        case A_EQ:
        case A_NE:
        case A_LT:
        case A_GT:
        case A_LE:
        case A_GE:
            if (parentASTop == A_IF) 
                return (cgcompare_and_jump(n->op, leftreg, rightreg, reg));
            else    
                return (cgcompare_and_set(n->op, leftreg, rightreg));
        case A_INTLIT: 
            return (cgloadint(n->v.intvalue));
        case A_IDENT: 
            return (cgloadglob(Gsym[n->v.id].name));
        case A_LVIDENT: 
            return (cgstorglob(reg, Gsym[n->v.id].name));
        case A_ASSIGN:
            // 工作已经完成返回结果
            return (rightreg);
        case A_PRINT:
            genprintint(leftreg);
            genfreeregs();
            return (NOREG);
        default:
            fatald("Unknown AST operator", n->op);
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