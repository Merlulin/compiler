#include "defs.h"
#include "data.h"
#include "decl.h"

struct ASTnode *mkastnode(int op, struct ASTnode *left, struct ASTnode *mid, struct ASTnode *right, int intvalue) {
    struct ASTnode *n;
    // 动态创建一个空节点
    n = (struct ASTnode *) malloc (sizeof(struct ASTnode));
    if (n == NULL) {
        fprintf(stderr, "Error: Unable to malloc in mkastnode().\n");
        exit(1);
    }

    n->op = op;
    n->left = left;
    n->mid = mid;
    n->right = right;
    n->v.intvalue = intvalue;
    return (n);
}

// 创建AST的叶子节点
struct ASTnode *mkastleaf(int op, int intvalue) {
    return (mkastnode(op, NULL, NULL, NULL, intvalue));
}

// 创捷AST的只有一个子节点的node
struct ASTnode *mkastunary(int op, struct ASTnode *left, int intvalue) {
    return (mkastnode(op, left, NULL, NULL, intvalue));
}
