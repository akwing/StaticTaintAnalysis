#ifndef _TMAP_H_
#define _TMAP_H_

#include <iostream>
#include <map>
#include <vector>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"

#define USECLASS

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
#endif
	TYPE_POINTER,	//指针
	TYPE_UNKNOWN	//未知
}VarDeclType;

typedef enum{
	U2T,
	T2U,
	OTHER
}MessageType;

//变量的污染属性
class Tainted_Attr
{
private:
	VarDeclType type;

	//污染属性
	e_tattr attr;
	//污染与哪些变量相关
	set<const VarDecl *> relation;

	//指向该指针指向的位置的污染状态
	Tainted_Attr *ptrAttr;
	bool is_temp;

#ifdef USECLASS
	//指向该类的实例的map
	classTmap *ptrClassDecl;
#endif

public:
	Tainted_Attr();
	Tainted_Attr(VarDeclType mytype
#ifdef USECLASS
		, classTmap *ct
#endif
		);
	Tainted_Attr(Tainted_Attr& b);
	~Tainted_Attr();

	/*获取相关的函数*/

	VarDeclType getType();
	e_tattr getVariableAttr();
	set<const VarDecl *> *getVariableRelation();
#ifdef USECLASS
	classTmap *getClassDecl();
#endif
	Tainted_Attr *getPointerAttr();
	bool getistemp();

	/*调试相关的函数*/

	void output();
	void copy(Tainted_Attr *p);

	/*属性设置相关的函数*/

	void settemp(bool b);
	void var_attr_set(e_tattr a, const VarDecl *r);
	void var_attr_set(e_tattr a, set<const VarDecl *> r);
#ifdef USECLASS
	void class_attr_set(e_tattr a, const VarDecl *r, Expr *ptrExp);
	void classmember_set(classTmap *ct);
#endif
	void pointer_attr_set(e_tattr a, const VarDecl *r);
	void setPointer(Tainted_Attr *pt);
	void setType(VarDeclType tp);
	void unionAttr(Tainted_Attr &a, Tainted_Attr &b);
	void unionAttr(Tainted_Attr &a);
	bool compareAttr(Tainted_Attr &ta);
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
	Tainted_Attr *getAttr(const VarDecl *vd);
	Tainted_Attr *getPointerAttr(const VarDecl *vd);

#ifdef USECLASS
	classTmap *getClassTmap(const VarDecl *p);
#endif
	void setType(const VarDecl *p, VarDeclType tp);
	void var_attr_set(const VarDecl *p, e_tattr e, const VarDecl *r);
	void var_attr_set(const VarDecl *vd, Tainted_Attr *ta);
	void ptr_set(const VarDecl *p, Tainted_Attr *tp);
	void ptr_attr_set(const VarDecl *p, e_tattr e, const VarDecl *r);
#ifdef USECLASS
	void classmember_attr_set(const VarDecl *p, classTmap *ct);
	void classmember_attr_set(const VarDecl *p, e_tattr e, const VarDecl *r, Expr *ptrExpr);
#endif
	void unionMap(CTmap &b);
	void clear();
	const VarDecl *get_VarDecl(int n);
	bool compareMap(CTmap &tm);
};

#endif
