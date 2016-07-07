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
	const std::vector<FunctionDecl *>& getFunctions();
private:
	std::vector<FunctionDecl *> functions;
};

//调用函数语句
class ASTCallExprLoad : public RecursiveASTVisitor<ASTCallExprLoad> {

public:
	bool VisitCallExpr(CallExpr *E);
	const std::vector<CallExpr *>& getCallExprs();
private:
	std::vector<CallExpr *> call_exprs;
};

//变量定义
class ASTVarDeclLoad : public RecursiveASTVisitor<ASTVarDeclLoad> {

public:
	bool VisitDeclStmt(DeclStmt *S);
	const std::vector<VarDecl *>& getVariables();
private:
	std::vector<VarDecl *> variables;
};

//获取类decl
class ASTCXXRecordLoad : public ASTConsumer, public RecursiveASTVisitor<ASTCXXRecordLoad> {
public:
	void HandleTranslationUnit(ASTContext &Context) override;
	bool VisitCXXRecordDecl(CXXRecordDecl *rd);
	const std::vector<CXXRecordDecl *>& getClassDecl();

private:
	std::vector<CXXRecordDecl *> cxxrds;
};

//获取类方法decl
class ASTCXXMethodDeclLoad :public RecursiveASTVisitor<ASTCXXMethodDeclLoad> {
public:
	bool VisitCXXMethodDecl(CXXMethodDecl *rd);
	const std::vector<CXXMethodDecl *>& getCXXMethodDecl();

private:
	std::vector<CXXMethodDecl *> cxxmds;
};

//获取类变量decl
class ASTFieldDeclLoad :public RecursiveASTVisitor<ASTFieldDeclLoad> {
public:
	bool VisitFieldDecl(FieldDecl *fd);
	const std::vector<FieldDecl *>& getFieldDecl();

private:
	std::vector<FieldDecl *> fds;
};


#endif
