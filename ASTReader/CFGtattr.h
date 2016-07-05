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
	CTmap& GetIN()
	{
		return IN;
	}
	CTmap& GetOUT()
	{
		return OUT;
	}
};

bool checkCFG(clang::CFG &cfg);
bool buildmap(clang::CFG &cfg, CTmap &tm);
