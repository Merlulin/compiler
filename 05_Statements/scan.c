#include "defs.h"
#include "data.h"
#include "decl.h"

/*
scan.c 文件包含我们词法扫描器的函数。我们将逐个从输入文件中读取一个字符。
然而，如果我们在输入流中读取得太远，就需要“放回”一个字符。
我们还想追踪我们当前所在的行，以便在调试消息中打印行号。所有这些都由 next() 函数完成：
*/
static int next(void) {
    int c;

    if (Putback) {
        c = Putback;  // 将一个字符提前返回
        Putback = 0;
        return c;
    }

    c = fgetc(Infile);  // 从输入文件读入一个字符
    if ('\n' == c) {
        Line ++;  // 如果读到换行符则说明行号+1
    }

    return c;
}


// 返回一个不想要的字符
static void putback(int c) {
    Putback = c;
}

// 跳过空白字符, 直到非空白，返回非空白的字符
static int skip(void) {
    int c;
    c = next();
    while (' ' == c || '\t' == c || '\n' == c || '\r' == c || '\f' == c) {
        c = next();
    }
    return (c);
}

// 返回字符c在字符串s中的位置
static int chrpos(char *s, int c) {
    char *p;
    p = strchr(s, c);  // strchr函数用于查找字符串中的一个字符，并返回该字符在字符串中出第一次出现的位置, 返回值是指向这个位置的指针，否则即为null
    return (p ? p - s : -1);
}


static int scanint(int c) {
    int k, val = 0;
    // 将每个字符转成对应的整数值
    while ((k = chrpos("0123456789", c)) >= 0) {
        val = val * 10 + k;
        c = next();
    }
    
    // 碰到非整数字符就将其返回
    putback(c);
    return val;
}

// 从输入文件中扫描一个标识符，并存储到buf缓存种，返回标识符的长度，直到扫描到非字母数字的字符
static int scanident(int c, char *buf, int lim) {
    int i = 0;

    // 当输入的c是一个字母、数字、或者下划线，继续取读取
    while (isalpha(c) || isdigit(c) || '_' == c) {
        if (lim - 1 == i) {
            printf("Error: identifier too long on long %d\n", Line);
            exit(1);
        } else if (i < lim - 1) {
            buf[i ++] = c;
        }
        // 读入下一个字符
        c = next();
    }
    // 退出时说明遇到非字母数字下划线，所以将读入的c推出
    putback(c);
    // 加入结束标识符
    buf[i] = '\0';
    return (i);
}


// 关键词匹配，匹配buf中读入的字符,返回关键字标记号，如果不是关键字则返回0
static int keyword(char *s) {
    switch (*s) {
        case 'p':
            if (!strcmp(s, "print"))
                return (T_PRINT);
            break;
    }
    return (0);
}

// 扫描字符并将token加入到对应的结构中
int scan(struct token *t) {
    
    int c, tokentype;
    c = skip();

    switch (c) {
        case EOF:  // 文件终止符（实际就是-1）
            t->token = T_EOF;
            return (0);
        case '+':
            t->token = T_PLUS;
            break;
        case '-':
            t->token = T_MINUS;
            break;
        case '*':
            t->token = T_STAR;
            break;
        case '/':
            t->token = T_SLASH;
            break;
        case ';':
            t->token = T_SEMI;
            break;
        default:
            // 如果是整数数字，需要扫描所有的字面值
            if (isdigit(c)) {
                t->intvalue = scanint(c);
                t->token = T_INTLIT;
                break;
            } else if (isalpha(c) || '_' == c) {
                scanident(c, Text, TEXTLEN);

                // 如果是keyword中已知的关键字，返回标记
                if (tokentype = keyword(Text)) {
                    t->token = tokentype;
                    break;
                }

                printf("Error: Unrecognised character %c on line %d\n", c, Line);
                exit(1);
            }
            printf("Error: exits no recognize character %c at %d Line.\n", c, Line);
            exit(1);
    }
    return (1);
}