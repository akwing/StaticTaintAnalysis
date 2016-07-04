#include"tmap.h"

//CFG的输入输出类
class CFGInOut{
private:
	//CFG的输入输出
	CTmap IN, OUT;
public:
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
