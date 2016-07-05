#include"CFGtattr.h"

//对函数的CFG进行迭代分析
Tainted_Attr *checkCFG(clang::CFG &cfg)
{
	clang::CFGBlock* CFGentry = &cfg.getEntry(), *CFGexit = &cfg.getExit();
	
	//扫描函数内部变量定义，创建污染表
	CTmap tm;
	buildmap(cfg, tm);
	
	map<clang::CFGBlock *, CFGInOut> block_io_map;
	
	build_block_io_table(block_io_map, CFGexit, CFGentry, tm);

	while (1)
	{
		bool changed = false;

		for (map<clang::CFGBlock *, CFGInOut>::iterator it = block_io_map.begin(), end = block_io_map.end(); it != end; it++)
		{
			//checkblock
			
		}

		if (changed == true)
			break;
	}
}

//为每个语句块创建INOUT污染表
void build_block_io_table(map<clang::CFGBlock *, CFGInOut> &block_io_map, clang::CFGBlock *CFGexit, clang::CFGBlock *block, CTmap &tm)
{
	map<clang::CFGBlock *, CFGInOut>::iterator t = block_io_map.find(block);
	
	//判断该块是否已添加过
	if (t != block_io_map.end())
		return;
	block_io_map[block].setIO(tm);
	
	//判断该块是否为出口
	if (block == CFGexit)
		return;

	clang::CFGBlock::succ_iterator it = block->succ_begin(), end = block->succ_end();
	
	while (it != end)
	{
		//递归创建表
		build_block_io_table(block_io_map, CFGexit, (*it).getReachableBlock(), tm);
		it++;
	}
}
