#ifndef TMAP_H
#define TMAP_H

#include <iostream>
#include <map>
#include <vector>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "classTmap.h"

using namespace std;
using namespace clang;
using namespace llvm;

//污染情况
typedef enum{
	TAINTED,
	UNTAINTED,
	RELATED
}e_tattr;

//变量的类型，供tmap使用
typedef enum{
	TYPE_VARIABLE,	//变量
	TYPE_CLASS,		//类
	TYPE_POINTER,	//指针
	TYPE_UNKNOWN	//未知
}eVarDeclType;

//变量的污染属性
class Tainted_Attr
{
private:
	union{
		struct{
			//污染属性
			e_tattr attr;
			//污染与哪些变量相关
			unsigned relation;
		}var;
		Tainted_Attr *attrpt;
		classTmap *classpt;
	}u;
	eVarDeclType type;
public:

	//默认构造函数，污染属性的类型对应为unknown
	Tainted_Attr(){
		u.classpt = NULL;
		type = TYPE_UNKNOWN;
	}
	
	//追加了类型参数的构造函数，如果是类请传入该类的classTmap
	Tainted_Attr(eVarDeclType mytype, classTmap *ct)
	{
		type = mytype;
		if (mytype == TYPE_VARIABLE)
		{
			u.var.attr = UNTAINTED;
			u.var.relation = 0;
		}
		else if (mytype == TYPE_POINTER)
		{
			u.attrpt = NULL;
		}
		else if (mytype == TYPE_CLASS)
		{
			u.classpt = new classTmap(*ct);
		}
		else
		{
			u.classpt = NULL;
			type = TYPE_UNKNOWN;
		}
	}
	//拷贝构造函数
	Tainted_Attr(Tainted_Attr& b)
	{
		type = b.type;
		switch (b.type)
		{
		case TYPE_VARIABLE:
			u.var.attr = b.u.var.attr;
			u.var.relation = b.u.var.relation;
			break;
		case TYPE_CLASS:
			u.classpt = b.u.classpt;
			break;
		case TYPE_POINTER:
			u.attrpt = b.u.attrpt;
			break;
		case TYPE_UNKNOWN:
			break;
		}
	}

	//获取所存储的污染属性的类型
	eVarDeclType getAttrType()
	{
		return type;
	}

	//获取变量类型的污染属性的污染情况
	e_tattr *getVariableAttr()
	{
		return &u.var.attr;
	}

	//获取变量类型的污染属性的关联
	unsigned getVariableRelation()
	{
		return u.var.relation;
	}

	//获取类类型的污染属性的classTmap指针
	classTmap *getClassPt()
	{
		return u.classpt;
	}

	//获得指针类型的污染属性所指向的污染属性
	Tainted_Attr *getPointerPt()
	{
		return u.attrpt;
	}

	//信息输出函数 调试用
	void output()
	{
		if (type == TYPE_VARIABLE)
		{
			if (u.var.attr == TAINTED)
				cout << "TAINTED ";
			else if (u.var.attr == UNTAINTED)
				cout << "UN ";
			else
				cout << "RE ";
			cout << u.var.relation;
		}
		//here to add output
		else
		{
			cout << "unknown type" << endl;
		}
	}

	//信息设置函数，如果当前污染属性的类型不为VARIABLE，不会进行修改，并警告
	void attr_set(e_tattr a, unsigned r)
	{
		if (type != TYPE_VARIABLE)
		{
			cout << "warning: type != TYPE_VARIABLE" << endl;
			return;
		}
		if (type == TYPE_VARIABLE)
		{
			u.var.attr = a;
			u.var.relation = r;
		}
	}

	//信息设置函数，如果当前的污染属性的类型不为CLASS，不会进行修改，并警告
	void attr_set(e_tattr a, unsigned r, Expr *pExp)
	{
		if (type != TYPE_CLASS || type != TYPE_POINTER)
		{
			cout << "warning: type != TYPE_CLASS or TYPE_POINTER" << endl;
			return;
		}
		//here to add
	}

	//信息设置函数，如果当前污染属性的类型不为POINTER，不会进行修改，并警告
	//将当前污染属性指向pt指向的位置
	void attr_set(Tainted_Attr *pt)
	{
		if (type != TYPE_POINTER)
		{
			cout << "warning: type != TYPE_POINTER" << endl;
			return;
		}
		while (1)
		{
			if (pt->type == TYPE_VARIABLE || pt->type == TYPE_CLASS)
			{
				
				return;
			}
			pt = pt->u.attrpt;
		}
	}

	//将两个污染属性取并
	void AndAttr(Tainted_Attr &b)
	{
		if (type != b.type)
		{
			cout << "Error in AndAttr()" << endl;
			return;
		}
		if (type == TYPE_VARIABLE)
		{
			if (u.var.attr == TAINTED)
				return;
			if (b.u.var.attr == TAINTED)
			{
				u.var.attr = TAINTED;
				return;
			}
			if (u.var.attr == UNTAINTED)
			{
				u.var.attr = b.u.var.attr;
				u.var.relation = b.u.var.relation;
				return;
			}
			if (b.u.var.attr == UNTAINTED)
				return;
			u.var.attr = b.u.var.attr;
			u.var.relation |= b.u.var.relation;
		}
	}
};

//封装了C++ map模板的污染表类 
class CTmap
{
	friend class CFGInOut;
public:
	//构造函数
	CTmap(){}
	//拷贝构造函数
	CTmap(CTmap& b)
	{
		Tainted_Attr *t = NULL, *newattr;
		VarDecl *pdec = NULL;
		map<VarDecl *, Tainted_Attr *>::iterator it = b.tmap.begin(), it_end = b.tmap.end();

		while (it != it_end)
		{
			pdec = (*it).first;
			t = (*it).second;
			//pdec==class to add how to copy


			newattr = new Tainted_Attr;

			newattr->attr_set(t->getAttr, t->getRelation);

			tmap[pdec] = newattr;
			it++;
		}
	}
	//析构函数
	~CTmap()
	{
		Tainted_Attr *t;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();

		while (iter != iter_end)
		{
			//pdec==class to add how to delete


			t = (*iter).second;
			delete t;	//释放临时变量的空间
			iter++;
		}
		tmap.clear();	//清空所有元素
	}

	//map中的元素及对应的污染情况输出
	void output()
	{
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		while (iter != iter_end)
		{
			std::cout << iter->first->getQualifiedNameAsString() << " ";
			iter->second->output();
			cout << endl;
			iter++;
		}
	}

	//将当前map清空，并将b中的元素及污染属性整个拷贝到map中
	void CopyMap(CTmap& b)
	{
	/*	tmap.clear();
		Tainted_Attr *t = NULL, *newattr;
		VarDecl *pdec = NULL;
		map<VarDecl *, Tainted_Attr *>::iterator it = b.tmap.begin(), it_end = b.tmap.end();

		while (it != it_end)
		{
			pdec = (*it).first;
			t = (*it).second;
			//pdec==class here to add how to copy
			
			newattr = new Tainted_Attr;
		
			newattr->attr_set(t->getVariableAttr(), t->getVariableRelation());

			tmap[pdec] = newattr;
			it++;
		}*/
	}

	//若p不在表中，插入一个变量定义节点，并创建一个污染属性变量
	void insert(VarDecl *p)
	{
		Tainted_Attr *t = new Tainted_Attr();

		int count;
		count = tmap.count(p);
		if (count == 0)
		{
			tmap[p] = t;
			//==class here to add how to insert
		}
		else
			delete t;
	}

	//从map中删除p
	void del(VarDecl *p)
	{
		Tainted_Attr *t = tmap[p];
		//==class here to add how to delete
		delete t;
		tmap.erase(p);
	}

	//取得变量定义节点p对应的污染属性
	Tainted_Attr *getmap(VarDecl *p)
	{
		int count;
		count = tmap.count(p);
		if (count == 0)
			return NULL;
		else
			return tmap[p];
	}

	//获取类的变量的自身的map
	classTmap *getClassTmap(VarDecl *p)
	{
		int count;
		count = tmap.count(p);
		if (count == 0)
		{
			return NULL;
		}
		else
		{
			return tmap[p]->getClassPt();
		}
	}

	//对某个变量的污染属性值进行设置
	void setAttr(VarDecl *p, e_tattr a, unsigned r)
	{
		int count;
		count = tmap.count(p);
		if (count == 0)
		{
			cout << "No such variable in the function" << endl;
			return;
		}
		else
		{
			tmap[p]->attr_set(a, r);
		}
	}

	//将两个map中的污染属性合并
	void AndMap(CTmap &b)
	{
		VarDecl *p;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		while (iter != iter_end)
		{
			p = (*iter).first;
			if (b.getmap(p) != NULL)
				(*iter).second->AndAttr(*b.getmap(p));
			iter++;
		}
	}

	//清空map中的元素
	void clear()
	{
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		while (iter != iter_end)
		{
			//==class here to add delete
			delete iter->second;
			iter++;
		}
		tmap.clear();
	}

	//获取第n个变量定义
	VarDecl *get_VarDecl(int n)
	{
		int i = 0;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		for (; iter != iter_end; iter++)
		{
			if (i == n)
				return iter->first;
			i++;
		}
		return NULL;
	}
private:
	map<VarDecl *, Tainted_Attr *> tmap;
};

#endif
