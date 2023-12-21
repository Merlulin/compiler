#include "defs.h"
#include "data.h"
#include "decl.h"

static char *ASTop[] = { "+", "-", "*", "/" };

// 给定AST树，解释其中的运算符并返回最终的结果。
int interpretAST(struct ASTnode *n) {
    int leftval, rightval;

    if (n->left) 
        leftval = interpretAST(n->left);
    if (n->right) 
        rightval = interpretAST(n->right);

    // 展示一下情况。
    if (n->op == A_INTLIT) {
        printf("int %d\n", n->intvalue);
    } else {
        printf("%d %s %d\n", leftval, ASTop[n->op], rightval);
    }
    
    switch (n->op) {
        case A_ADD:
            return (leftval + rightval);
        case A_SUBTRACT:
            return (leftval - rightval);
        case A_MULTIPLY:
            return (leftval * rightval);
        case A_DIVIDE:
            return (leftval / rightval);
        case A_INTLIT:
            return (n->intvalue);
        default:
            fprintf(stderr, "Error: Unknown AST operator %d.\n", n->op);
            exit(1);
    }
}