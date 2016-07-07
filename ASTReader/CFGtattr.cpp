#include"CFGtattr.h"

//对函数的CFG进行迭代分析
Tainted_Attr *checkCFG(clang::CFG &cfg, CTmap &tm, clang::FunctionDecl *fd)
{
	
	clang::CFGBlock* CFGentry = &cfg.getEntry(), *CFGexit = &cfg.getExit();
	
	//扫描函数内部变量定义，创建污染表
	CTmap *outm = NULL, *inm = NULL;
	
	map<clang::CFGBlock *, CFGInOut> block_io_map;
	
	build_block_io_table(block_io_map, CFGexit, CFGentry, tm);

	//主循环，当无OUT发生改变时跳出循环
	while (1)
	{
		bool changed = false;

		//为每个block计算其新的out
		for (map<clang::CFGBlock *, CFGInOut>::iterator it = block_io_map.begin(), end = block_io_map.end(); it != end; it++)
		{
			//block-in = Upred it->pred
			//即对block的前驱的out求并，作为该block的in
			clang::CFGBlock::succ_iterator succ_it = it->first->succ_begin(), succ_end = it->first->succ_end();
			clang::CFGBlock* temp = NULL;

			inm = it->second.GetIN();
			inm->clear();
			inm->CopyMap(tm);

			while (succ_it != succ_end)
			{
				temp = succ_it->getReachableBlock();
				outm = block_io_map[temp].GetOUT();
				inm->AndMap(*outm);
				succ_it++;
			}
			
			outm = it->second.GetOUT();
			outm->CopyMap(*inm);

			//checkblock, modify changed
			
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
