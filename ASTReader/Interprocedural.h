#include <iostream>
#include "tmap.h"
#include "AST.h"
#include "CFGtattr.h"

//调用函数的返回值的污染属性
Tainted_Attr* callFuncTainted(std::vector<callgraph*> Callgraph,CallExpr* ce)
{
	FunctionDecl* fd = ce->getDirectCallee();
	callgraph* cg = findById(Callgraph, fd->getQualifiedNameAsString());
	clang::CFG* cfg = dynamic_cast<clang::CFG*>(cg->get_cfg().get());
	return checkCFG(*cfg);//等待checkCFG完善，可以获得返回的Tainted_Attr* 类型数据
}
