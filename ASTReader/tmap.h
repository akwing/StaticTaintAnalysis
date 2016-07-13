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
	TYPE_CLASSPOINTER,//指向类的指针
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
		//指向该指针指向的变量的污染属性
		Tainted_Attr *ptrAttr;
		//指向该类的实例的map
		classTmap *ptrClassDecl;
	}u;
	eVarDeclType type;
public:

	//默认构造函数，污染属性的类型对应为unknown
	Tainted_Attr(){
		u.ptrClassDecl = NULL;
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
		else if (mytype == TYPE_POINTER || mytype == TYPE_CLASSPOINTER)
		{
			u.ptrAttr = NULL;
		}
		else if (mytype == TYPE_CLASS)
		{
			u.ptrClassDecl = new classTmap(*ct);
		}
		else
		{
			u.ptrClassDecl = NULL;
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
			u.ptrClassDecl = b.u.ptrClassDecl;	//
			break;
		case TYPE_POINTER:
			u.ptrAttr = b.u.ptrAttr;
			break;
		case TYPE_UNKNOWN:
			break;
		}
	}

	//获取所存储的污染属性的类型
	eVarDeclType getType()
	{
		return type;
	}

	//获取变量类型的污染属性的污染情况
	e_tattr getVariableAttr()
	{
		return u.var.attr;
	}

	//获取变量类型的污染属性的关联
	unsigned getVariableRelation()
	{
		return u.var.relation;
	}

	//获取类类型的污染属性的classTmap指针
	classTmap *getClassDecl()
	{
		return u.ptrClassDecl;
	}

	//获得指针类型的污染属性所指向的污染属性
	Tainted_Attr *getPointerAttr()
	{
		return u.ptrAttr;
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

	//复制p中的污染属性
	void copy(Tainted_Attr *p)
	{
		if (type == TYPE_CLASS)
		{
			u.ptrClassDecl->~classTmap();
			delete u.ptrClassDecl;
		}
		type = p->type;
		if (type == TYPE_VARIABLE)
		{
			u.var.attr = p->u.var.attr;
			u.var.relation = p->u.var.relation;
		}
		else if (type == TYPE_POINTER || type == TYPE_CLASSPOINTER)
		{
			u.ptrAttr = p->u.ptrAttr;
		}
		else if (type == TYPE_CLASS)
		{
			u.ptrClassDecl = new classTmap(*p->u.ptrClassDecl);
		}
	}

	//信息设置函数，如果当前污染属性的类型不为VARIABLE，不会进行修改，并警告
	void var_attr_set(e_tattr a, unsigned r)
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
	void class_attr_set(e_tattr a, unsigned r, Expr *ptrExp)
	{
		if (type != TYPE_CLASS)
		{
			cout << "warning: type != TYPE_CLASS" << endl;
			return;
		}
		//here to add
	}

	void setclass(classTmap *ct)
	{
		u.ptrClassDecl = ct;
	}

	//信息设置函数，如果当前的污染属性的类型不为POINTER，不会进行修改，并警告
	void pointer_attr_set(e_tattr a, unsigned r)
	{
		if (type != TYPE_CLASS)
		{
			cout << "warning: type != TYPE_POINTER" << endl;
			return;
		}
		u.ptrAttr->var_attr_set(a, r);
	}

	void classpointer_attr_set(e_tattr a, unsigned r, Expr *ptrExp)
	{
		if (type != TYPE_CLASSPOINTER || ptrExp == NULL)
		{
			cout << "Warning: classpointer" << endl;
			return;
		}
		//here to add
	}

	//将当前污染属性指向pt指向的位置，如果当前污染属性的类型不为POINTER，不会进行修改，并警告
	void setPointer(Tainted_Attr *pt)
	{
		if (type != TYPE_POINTER || type != TYPE_CLASSPOINTER)
		{
			cout << "Warning: type != POINTER" << endl;
			return;
		}
		while (1)
		{
			if (pt->type == TYPE_VARIABLE || pt->type == TYPE_CLASS)
			{
				u.ptrAttr = pt;
				return;
			}
			pt = pt->u.ptrAttr;
		}
	}

	//设置污染属性的类型
	void setType(eVarDeclType tp)
	{
		type = tp;
		if (tp == TYPE_VARIABLE)
		{
			u.var.attr = UNTAINTED;
			u.var.relation = 0;
		}
		else if (tp == TYPE_POINTER)
		{
			u.ptrAttr = NULL;
		}
		else if (tp == TYPE_CLASS)
		{
			u.ptrClassDecl = NULL;
		}
		else if (tp == TYPE_CLASSPOINTER)
		{
			u.ptrAttr = NULL;
		}
		else
		{
			u.ptrClassDecl = NULL;
			type = TYPE_UNKNOWN;
		}
	}

	//将两个污染属性取并，有待修改
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
private:
	map<VarDecl *, Tainted_Attr *> tmap;
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

			newattr->var_attr_set(t->getVariableAttr(), t->getVariableRelation());

			tmap[pdec] = newattr;
			it++;
		}
	}
	//析构函数
	~CTmap()
	{
		Tainted_Attr *t;
		classTmap *ct;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		while (iter != iter_end)
		{
			t = iter->second;
			if (t->getType() == TYPE_CLASS)
			{
				ct = t->getClassDecl();
				ct->clearTmap();
			}
			delete iter->second;
			iter->second = NULL;
			iter++;
		}
		tmap.clear();
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
		clear();
		Tainted_Attr *t = NULL, *newattr;
		VarDecl *pdec = NULL;
		map<VarDecl *, Tainted_Attr *>::iterator it = b.tmap.begin(), it_end = b.tmap.end();

		while (it != it_end)
		{
			pdec = it->first;
			t = it->second;
			//pdec==class here to add how to copy
			
			newattr = new Tainted_Attr;
		
			newattr->copy(t);

			tmap[pdec] = newattr;
			it++;
		}
	}

	//若p不在表中，插入一个以p为索引的空条目
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
	Tainted_Attr *getAttr(VarDecl *p)
	{
		int count;
		count = tmap.count(p);
		if (count == 0)
			return NULL;
		else
		{
			return tmap[p];
		}
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
			if (tmap[p]->getType() != TYPE_CLASS)
			{
				return NULL;
			}
			return tmap[p]->getClassDecl();
		}
	}

	//设置某个变量的属性（变量、指针、类）
	void setType(VarDecl *p, eVarDeclType tp)
	{
		int count;
		count = tmap.count(p);
		if (count == 0)
		{
			cout << "Error: No such variable in the function" << endl;
			return;
		}
		else
		{
			tmap[p]->setType(tp);
		}
	}

	//设置p的污染属性，p为普通变量类型
	void var_attr_set(VarDecl *p, e_tattr e, unsigned r)
	{
		int count;
		Tainted_Attr *tp;
		count = tmap.count(p);
		if (count == 0)
		{
			cout << "Error: No such variable in the function" << endl;
			return;
		}
		else
		{
			tp = tmap[p];
			if (tp->getType() != TYPE_VARIABLE)
			{
				cout << "Warning: type != TYPE_VARIABLE" << endl;
				return;
			}
			tp->var_attr_set(e, r);
		}
	}

	//设置pt指向的变量
	void ptr_set(VarDecl *p, Tainted_Attr *tp)
	{
		int count;
		count = tmap.count(p);
		if (count == 0)
		{
			cout << "Error: No such variable in the function" << endl;
			return;
		}
		else
		{
			if (tmap[p]->getType() != TYPE_POINTER || tmap[p]->getType() != TYPE_CLASSPOINTER)
			{
				cout << "Warning: type != POINTER" << endl;
				return;
			}
			tmap[p]->setPointer(tp);
		}
	}

	//设置pt指向的变量的污染属性
	void ptr_attr_set(VarDecl *p, e_tattr e, unsigned r)
	{
		int count;
		Tainted_Attr *tp;
		count = tmap.count(p);
		if (count == 0)
		{
			cout << "Error: No such variable in the function" << endl;
			return;
		}
		else
		{
			tp = tmap[p];
			if (tp->getType() != TYPE_POINTER)
			{
				cout << "Warning: type != TYPE_POINTER" << endl;
				return;
			}
			tp->pointer_attr_set(e, r);
		}
	}

	void classmember_attr_set(VarDecl *p,classTmap *ct)
	{
		int count;
		Tainted_Attr *tp;
		count = tmap.count(p);
		if (count == 0)
		{
			cout << "Error: No such variable in the function" << endl;
			return;
		}
		else
		{
			tp = tmap[p];
			if (tp->getType() != TYPE_CLASS)
			{
				cout << "Warning: type != TYPE_POINTER" << endl;
				return;
			}
			else
			{
				tp->setclass(ct);
			}
		}
	}

	void classmember_attr_set(VarDecl *p, e_tattr e, unsigned r, Expr *ptrExpr)
	{}

	//将两个map中的污染属性合并
	void AndMap(CTmap &b)
	{
		VarDecl *p;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		while (iter != iter_end)
		{
			p = (*iter).first;
			if (b.getAttr(p) != NULL)
				(*iter).second->AndAttr(*b.getAttr(p));
			iter++;
		}
	}

	//清空map中的元素
	void clear()
	{
		Tainted_Attr *t;
		classTmap *ct;
		map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
		while (iter != iter_end)
		{
			t = iter->second;
			if (t->getType() == TYPE_CLASS)
			{
				ct = t->getClassDecl();
				ct->clearTmap();
			}
			delete iter->second;
			iter->second = NULL;
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
};

#endif
