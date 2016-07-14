#ifndef TMAP_H
#define TMAP_H

#include <iostream>
#include <map>
#include <vector>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"

//#define USECLASS

#ifdef USECLASS
#include "classTmap.h"
#endif

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
#ifdef USECLASS
	TYPE_CLASS,		//类
	TYPE_CLASSPOINTER,//指向类的指针
#endif
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
		//指向该指针指向的变量的污染属性
		Tainted_Attr *ptrAttr;

#ifdef USECLASS
		//指向该类的实例的map
		classTmap *ptrClassDecl;
#endif
	}u;
	eVarDeclType type;
public:
	Tainted_Attr();
	Tainted_Attr(eVarDeclType mytype
#ifdef USECLASS
		, classTmap *ct
#endif
		);
	Tainted_Attr(Tainted_Attr& b);

	/*获取相关的函数*/

	eVarDeclType getType();
	e_tattr getVariableAttr();
	unsigned getVariableRelation();
#ifdef USECLASS
	classTmap *getClassDecl();
#endif
	Tainted_Attr *getPointerAttr();

	/*调试相关的函数*/

	void output();
	void copy(Tainted_Attr *p);

	/*属性设置相关的函数*/

	void var_attr_set(e_tattr a, unsigned r);
#ifdef USECLASS
	void class_attr_set(e_tattr a, unsigned r, Expr *ptrExp);
	void classmember_set(classTmap *ct);
	void classpointer_attr_set(e_tattr a, unsigned r, Expr *ptrExp);
#endif
	void pointer_attr_set(e_tattr a, unsigned r);
	void setPointer(Tainted_Attr *pt);
	void setType(eVarDeclType tp);
	void AndAttr(Tainted_Attr &b);
};

//封装了C++ map模板的污染表类 
class CTmap
{
private:
	map<VarDecl *, Tainted_Attr *> tmap;
public:
	CTmap();
	CTmap(CTmap& b);
	~CTmap();
	void output();
	void CopyMap(CTmap& b);
	void insert(VarDecl *p);
	void del(VarDecl *p);
	Tainted_Attr *getAttr(VarDecl *p);

#ifdef USECLASS
	classTmap *getClassTmap(VarDecl *p);
#endif
	void setType(VarDecl *p, eVarDeclType tp);
	void var_attr_set(VarDecl *p, e_tattr e, unsigned r);
	void ptr_set(VarDecl *p, Tainted_Attr *tp);
	void ptr_attr_set(VarDecl *p, e_tattr e, unsigned r);
#ifdef USECLASS
	void classmember_attr_set(VarDecl *p, classTmap *ct);
	void classmember_attr_set(VarDecl *p, e_tattr e, unsigned r, Expr *ptrExpr);
#endif
	void AndMap(CTmap &b);
	void clear();
	VarDecl *get_VarDecl(int n);
};

#endif
