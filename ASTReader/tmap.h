#ifndef _TMAP_H_
#define _TMAP_H_

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
		}var;
		//指向该指针指向的变量的污染属性
		Tainted_Attr *ptrAttr;

#ifdef USECLASS
		//指向该类的实例的map
		classTmap *ptrClassDecl;
#endif
	}u;
	eVarDeclType type;
	set<const VarDecl *> relation;
public:
	Tainted_Attr();
	Tainted_Attr(eVarDeclType mytype
#ifdef USECLASS
		, classTmap *ct
#endif
		);
	Tainted_Attr(Tainted_Attr& b);
	~Tainted_Attr();

	/*获取相关的函数*/

	eVarDeclType getType();
	e_tattr getVariableAttr();
	set<const VarDecl *> *getVariableRelation();
#ifdef USECLASS
	classTmap *getClassDecl();
#endif
	Tainted_Attr *getPointerAttr();

	/*调试相关的函数*/

	void output();
	void copy(Tainted_Attr *p);

	/*属性设置相关的函数*/

	void var_attr_set(e_tattr a, const VarDecl *r);
	void var_attr_set(e_tattr a, set<const VarDecl *> r);
#ifdef USECLASS
	void class_attr_set(e_tattr a, const VarDecl *r, Expr *ptrExp);
	void classmember_set(classTmap *ct);
	void classpointer_attr_set(e_tattr a, const VarDecl *r, Expr *ptrExp);
#endif
	void pointer_attr_set(e_tattr a, const VarDecl *r);
	void setPointer(Tainted_Attr *pt);
	void setType(eVarDeclType tp);
	void AndAttr(Tainted_Attr &a, Tainted_Attr &b);
};

//封装了C++ map模板的污染表类 
class CTmap
{
private:
	map<const VarDecl *, Tainted_Attr *> tmap;
public:
	CTmap();
	CTmap(CTmap& b);
	~CTmap();
	void output();
	void CopyMap(CTmap& b);
	void insert(const VarDecl *p);
	void del(const VarDecl *p);
	Tainted_Attr *getAttr(const VarDecl *p);

#ifdef USECLASS
	classTmap *getClassTmap(const VarDecl *p);
#endif
	void setType(const VarDecl *p, eVarDeclType tp);
	void var_attr_set(const VarDecl *p, e_tattr e, const VarDecl *r);
	void var_attr_set(const VarDecl *vd, Tainted_Attr *ta);
	void ptr_set(const VarDecl *p, Tainted_Attr *tp);
	void ptr_attr_set(const VarDecl *p, e_tattr e, const VarDecl *r);
#ifdef USECLASS
	void classmember_attr_set(const VarDecl *p, classTmap *ct);
	void classmember_attr_set(const VarDecl *p, e_tattr e, const VarDecl *r, Expr *ptrExpr);
#endif
	void AndMap(CTmap &b);
	void clear();
	const VarDecl *get_VarDecl(int n);
};

#endif
