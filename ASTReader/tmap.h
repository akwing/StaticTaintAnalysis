#include <iostream>
#include <map>
#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace std;
using namespace clang;
using namespace llvm;

enum e_tattr{
		TAINTED,
		UNTAINTED,
		RELATED
};

class Tainted_Attr
{
public:
	friend class Tmap;
	//构造函数
	Tainted_Attr(){
		attr = UNTAINTED;
		relation = 0;
	}
	//拷贝构造函数
	Tainted_Attr(const Tainted_Attr& b)
	{
		attr = b.attr;
		relation = b.relation();
	}
	//信息输出函数 调试用
	void output()
	{
		if (attr == TAINTED)
			cout << "TAINTED ";
		else if (attr == UNTAINTED)
			cout << "UN ";
		else
			cout << "RE ";
		cout << relation << endl;
	}
	//信息设置函数
	void attr_set(e_tattr a, unsigned long long r)
	{
		attr = a;
		relation |= r;
	}
private:
	e_tattr attr;	//污染属性
	unsigned long long relation;	//污染与哪些变量相关
};

//封装了C++ map模板的污染表类 
class Tmap
{
public:
	//构造函数
	Tmap(){}
	//拷贝构造函数
	Tmap(const Tmap& b)
	{
		Tainted_Attr *t = NULL, *newattr;
		VarDecl *pdec = NULL;
		map<VarDecl *, Tainted_Attr *>::iterator it = b.tmap.begin(), it_end = b.tmap.end();

		while (it != it_end)
		{
			pdec = (*it).first;
			t = (*it).second;
			newattr = new Tainted_Attr;
			
			newattr->attr = t->attr;
			newattr->relation = t->relation;

			tmap[pdec] = newattr;
		}
	}
	//析构函数
	~Tmap()
	{
		Tainted_Attr *t;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		
		while (iter != iter_end)
		{
			t = (*iter).second;
			delete t;	//释放临时变量的空间
		}
		tmap.clear();	//清空所有元素
	}
	
	void output()
	{
		Tainted_Attr *t;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		
		while (iter != iter_end)
		{
			t = (*iter).second;
			t->output();
		}
	}
	void insert(VarDecl *p)
	{
		Tainted_Attr *t = new Tainted_Attr();
		
		int count;
		count = tmap.count(p);
		if (count == 0)
			tmap[p] = t;
		else
			delete t;
	}
	void del(VarDecl *p)
	{
		Tainted_Attr *t = tmap[p];
		delete t;
		tmap.erase(p);
	}
	Tainted_Attr *getmap(VarDecl *p)
	{
		Tainted_Attr *t;
		int count;
		count = tmap.count(p);
		if(count == 0)
			return NULL;
		else
			return tmap[p];
	} 

private:
	map<VarDecl *, Tainted_Attr *> tmap;
};
