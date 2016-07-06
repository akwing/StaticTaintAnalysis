#ifndef CFGTATTR_H
#define CFGTATTR_H

#include"tmap.h"
#include"clang\Analysis\CFG.h"

using namespace std;
using namespace clang;
using namespace llvm;

//CFG的输入输出类
class CFGInOut{
private:
	//CFG的输入输出
	CTmap IN, OUT;
public:
	//拷贝构造函数
	CFGInOut(CTmap& b)
	{
		IN.CopyMap(b);
		OUT.CopyMap(b);
	}
	CTmap* GetIN()
	{
		return &IN;
	}
	CTmap* GetOUT()
	{
		return &OUT;
	}
	void setIO(CTmap &tm)
	{
		IN.CopyMap(tm);
		OUT.CopyMap(tm);
	}
};

Tainted_Attr *checkCFG(clang::CFG &cfg, CTmap &tm, clang::FunctionDecl *fd);
bool buildmap(clang::CFG &cfg, CTmap &tm);
void build_block_io_table(map<clang::CFGBlock *, CFGInOut> &block_io_map, clang::CFGBlock *CFGexit, clang::CFGBlock *block, CTmap &tm);

#endif
