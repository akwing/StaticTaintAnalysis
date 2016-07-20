#ifndef TCI_H
#define TCI_H

#include "AST.h"
#include "tmap.h"

using namespace clang;
using namespace std;
using namespace llvm;

//敏感信息类型
#define TTYPE_ARRAY
#define TTYPE_DIV_MOD
#define TTYPE_LOOP_BOUND
#define TTYPE_MEM
#define TTYPE_POINTER_NULL_SET
#define TTYPE_RECURSIVE_CALL

typedef struct
{
	int type;
	VarDecl* vd;
	Stmt* stmt;
	Tainted_Attr* re;
}TCI;

#endif
