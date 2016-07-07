#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <stack>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Analysis/CFG.h"

#include"tmap.h"
using namespace clang;
using namespace llvm;
//using namespace std;

//获取Funtion相关语句
class ASTFunctionLoad : public ASTConsumer, public RecursiveASTVisitor < ASTFunctionLoad > {	//get functions

public:
	void HandleTranslationUnit(ASTContext &Context) override;
	bool VisitFunctionDecl(FunctionDecl *FD);
	const std::vector<FunctionDecl *> &getFunctions() const;
private:
	std::vector<FunctionDecl *> functions;
};

//被调用函数
class ASTCalledFunctionLoad : public RecursiveASTVisitor<ASTCalledFunctionLoad> {
public:
	bool VisitCallExpr(CallExpr *E);
	const std::vector<FunctionDecl *> getFunctions();
private:
	std::set<FunctionDecl *> functions;
};

//调用函数语句
class ASTCallExprLoad : public RecursiveASTVisitor<ASTCallExprLoad> {

public:
	bool VisitCallExpr(CallExpr *E);
	const std::vector<CallExpr *> getCallExprs();
private:
	std::vector<CallExpr *> call_exprs;
};
/*
class ASTDeclStmtLoad : public RecursiveASTVisitor<ASTDeclStmtLoad> {	//get call functions

public:
	bool VisitDeclStmt(DeclStmt *E) {//////////////////////
		decl_stmts.insert(E);
		return true;
	}

	const std::vector<FunctionDecl *> getFunctions() {
		return std::vector<FunctionDecl *>(decl_stmts.begin(), decl_stmts.end());
	}

private:
	std::set<DeclStmt *> decl_stmts;
};
*/

//变量定义
class ASTVarDeclLoad : public RecursiveASTVisitor<ASTVarDeclLoad> {

public:
	bool VisitDeclStmt(DeclStmt *S);
	const std::vector<VarDecl *> getVariables();
private:
	std::vector<VarDecl *> variables;
};

//获取类decl
class ASTCXXRecorderLoad : public ASTConsumer, public RecursiveASTVisitor<ASTCXXRecorderLoad> {
public:
	void HandleTranslationUnit(ASTContext &Context) override {
		TranslationUnitDecl *D = Context.getTranslationUnitDecl();
		TraverseDecl(D);
	}

	bool VisitClassDecl(CXXRecordDecl *rd) {
		cxxrds.insert(rd);
		return true;
	}

	const std::vector<CXXRecordDecl *> getClassDecl() {
		return std::vector<CXXRecordDecl *>(cxxrds.begin(), cxxrds.end());
	}

private:
	std::set<CXXRecordDecl *> cxxrds;
};

//获取类方法decl
/*class ASTCXXMethodDeclLoad : public ASTConsumer, public RecursiveASTVisitor<ASTCXXMethodDeclLoad> {
public:
	void HandleTranslationUnit(ASTContext &Context) override {
		TranslationUnitDecl *D = Context.getTranslationUnitDecl();
		TraverseDecl(D);
	}

	bool VisitClassDecl(CXXMethodDecl *rd) {
		cxxmds.insert(rd);
		return true;
	}

	const std::vector<CXXMethodDecl *> getClassMethodDecl() {
		return std::vector<CXXMethodDecl *>(cxxmds.begin(), cxxmds.end());
	}

private:
	std::set<CXXMethodDecl *> cxxmds;
};*/


//获取类构造函数decl
/*class ASTCXXConstructorDeclLoad : public ASTConsumer, public RecursiveASTVisitor<ASTCXXConstructorDeclLoad> {
public:
	void HandleTranslationUnit(ASTContext &Context) override {
		TranslationUnitDecl *D = Context.getTranslationUnitDecl();
		TraverseDecl(D);
	}

	bool VisitClassDecl(CXXConstructorDecl *rd) {
		cxxcds.insert(rd);
		return true;
	}

	const std::vector<CXXConstructorDecl *> getClassConstructorDecl() {
		return std::vector<CXXConstructorDecl *>(cxxcds.begin(), cxxcds.end());
	}

private:
	std::set<CXXConstructorDecl *> cxxcds;
};*/

typedef enum
{
	common,inclass
}methodType;




//函数调用关系图
class callgraph{
public:
	callgraph(FunctionDecl* f1);
	callgraph(FunctionDecl* f1, FunctionDecl* f2);
	FunctionDecl* getCaller(int i);		//调用cur
	FunctionDecl* getCallee(int i);		//被cur调用
	FunctionDecl* getCur();
	int getCallerNum();
	int getCalleeNum();
	void addCaller(FunctionDecl* otherFD);
	void addCallee(FunctionDecl* otherFD);
	void delCallee(FunctionDecl* otherFD);
	void changeMethodType();
	void setRoot(VarDecl* r);
	void setClass(CXXRecordDecl* rd);
	CXXRecordDecl* getClass();
	VarDecl* getRoot();
	methodType getMethodType();
	int ifCheck;

public:
	std::unique_ptr<CFG> get_cfg();
	void print_cfg();
	CTmap& getCTmap();
	void addParam(VarDecl* vd);
	void addVar(VarDecl* vd);
	int getParamNum();
	int getVarNum();

private:
	//方法的类型
	methodType type;
	//类方法所属的实例
	VarDecl* root;
	CXXRecordDecl* classDecl;
	FunctionDecl* caller[10];
	FunctionDecl* cur;
	FunctionDecl* callee[10];
	//std::unique_ptr<CFG> cfg;
	int callerNum, calleeNum;
	CTmap* map;
	int paramNum;
	int varNum;
	Tainted_Attr* returnVar;
};

callgraph* findById(std::vector<callgraph*> Callgraph, std::string id);		
void ifcheck(std::vector<callgraph*> cg, callgraph* t);		
void resetIfCheck(std::vector<callgraph*>Callgraph);		
void getRing(std::vector<callgraph*>& Callgraph, int n, std::vector<FunctionDecl*>& ringVector);	
void printCallGraph(std::vector<callgraph*> Callgraph);	

#endif
