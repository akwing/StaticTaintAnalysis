#include"CFGtattr.h"
#include"AST.h"

//对函数的CFG进行迭代分析
void checkCFG(clang::CFG &cfg, CTmap &tm, callgraph *cg)
{
	clang::CFGBlock* CFGentry = &(cfg.getEntry()), *CFGexit = &(cfg.getExit());
	clang::CFGBlock::pred_iterator pred_it, pred_end;

	CTmap *outm = NULL, *inm = NULL, preout;
	map<clang::CFGBlock *, CFGInOut> block_io_map;

	build_block_io_table(block_io_map, CFGexit, CFGentry, tm);
	cg->print_cfg();
	//printiotable(block_io_map);

	//主循环，当无OUT发生改变时跳出循环
	int i = 1;
	while (1)
	{
		bool changed = false;

		//为每个block计算其新的out
		cout << "****************************" << endl;
		cout << "CFG check " << i << " start........." << endl;
		for (map<clang::CFGBlock *, CFGInOut>::reverse_iterator r_iter = block_io_map.rbegin(), r_end = block_io_map.rend(); r_iter != r_end; r_iter++)
		{
			//计算新的in，即对block的前驱的out求并，作为该block的in
			pred_it = r_iter->first->pred_begin(), pred_end = r_iter->first->pred_end();
			clang::CFGBlock* temp = NULL;
			if (r_iter->first != &cfg.getEntry())
			{
				inm = r_iter->second.GetIN();
				inm->clear();
				inm->CopyMap(tm);
				while (pred_it != pred_end)
				{
					temp = pred_it->getReachableBlock();
					cout << "BBB" << temp->getBlockID() << endl;;
					outm = block_io_map[temp].GetOUT();
					inm->unionMap(*outm);
					pred_it++;
				}
				cout << endl;
			}
			else
			{
				continue;
			}
			outm = r_iter->second.GetOUT();
			preout.CopyMap(*outm);
			outm->CopyMap(*inm);
			
			//checkblock, modify changed
			checkblock(r_iter->first, *outm,cg);
			if (outm->compareMap(preout) == false)
			{
				changed = true;
				printBlockMsg(block_io_map,r_iter->first);
			}
		}

		//迭代至所有block的OUT都不发生改变，跳出循环
		cout << "CFG check " << i << " end." << endl;
		cout << "****************************" << endl;
		if (changed == false)
			break;
		i++;
	}
	//here to add output
	printiotable(block_io_map);
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

//打印一个block的污染信息
void printBlockMsg(map<clang::CFGBlock *, CFGInOut> &block_io_map, clang::CFGBlock *block)
{
	cout << endl << "============================" << endl;
	cout << "B" << block->getBlockID() << ":" << endl << "IN" << endl;
	block_io_map[block].GetIN()->output();
	cout << endl << "OUT"<<endl;
	block_io_map[block].GetOUT()->output();
	cout << "============================" << endl<<endl;
}

//打印当前函数中每个block的污染信息
void printiotable(map<clang::CFGBlock *, CFGInOut> &block_io_map)
{
	map<clang::CFGBlock *, CFGInOut>::iterator it = block_io_map.begin(), it_end = block_io_map.end();
	while (it != it_end)
	{
		printBlockMsg(block_io_map, it->first);
		it++;
	}
}
