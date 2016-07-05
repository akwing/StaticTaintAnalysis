#include"CFGtattr.h"

//对函数的CFG进行迭代分析
bool checkCFG(clang::CFG &cfg)
{
	const clang::CFGBlock* CFGentry = &cfg.getEntry(), *CFGexit = &cfg.getExit();
	CTmap tm();
	map<clang::CFGBlock, CFGInOut> block_io_table;
}
