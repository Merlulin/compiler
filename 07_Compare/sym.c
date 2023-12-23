#include "defs.h"
#include "data.h"
#include "decl.h"

// 管理符号表的文件

static int Globs = 0;


// 创建一个新的全局变量
static int newglob(void) {
    int p;

    if ((p = Globs++) >= NSYMBOLS) {
        fatal("Too many global symbols");
    }
    return (p);
}

// 如果查询的关键词在符号表里，返回对应的token
int findglob(char *s) {
    int i;
    for(i = 0; i < Globs; i ++) {
        if (*s == *Gsym[i].name && !strcmp(s, Gsym[i].name))
            return (i);
    }
    return (-1);
}

// 将关键词添加到符号表中
int addglob(char *name) {
    int y;

    // 如果要添加的符号已经在符号表中，则直接返回对应的token
    if ((y = findglob(name)) != -1)
        return (y);

    y = newglob();
    // 深度拷贝
    Gsym[y].name = strdup(name);
    return (y);
}