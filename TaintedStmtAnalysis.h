#ifndef  _TAINTED_STMT_ANALYSIS_
#define _TAINTED_STMT_ANALYSIS_
#include <iostream>
#include <vector>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang\Analysis\CFG.h"
#include "Tmap.h"

using namespace clang;
using namespace llvm;


bool checkblock(CFGBlock* cfgb, CTmap &out);
int Stmt_analysis(const Stmt* stmt, CTmap &out);
Tainted_Attr* Expr_analysis(const Expr* expr, CTmap &out);
Tainted_Attr* BinaryOperator_Expr_analysis(const Expr* expr, CTmap &out);

#endif
