#ifndef TCI_H
#define TCI_H

#include "AST.h"
#include "tmap.h"

using namespace clang;
using namespace std;
using namespace llvm;

typedef struct
{
	VarDecl* vd;
	Stmt* stmt;
	Tainted_Attr* re;
}TCI;

#endif
