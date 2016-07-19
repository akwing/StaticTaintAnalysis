#ifndef TCI_H
#define TCI_H

#include "AST.h"
#include "tmap.h"

using namespace clang;
using namespace std;
using namespace llvm;

class TCI
{
public:
	VarDecl* vd;
	Stmt* stmt;
	Tainted_Attr* re;
};

#endif
