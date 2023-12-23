#include <stdio.h>

#ifndef extern_
 #define extern_ extern
#endif


extern_ int Line;
extern_ int Putback;
extern_ FILE *Infile;
extern_ FILE *Outfile;
extern_ struct token Token;
extern_ char Text[TEXTLEN + 1];  // 标识符缓冲区
extern_ struct symtable Gsym[NSYMBOLS]; // 定义全局的符号表


