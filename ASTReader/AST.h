#include <iostream>
#include <vector>
#include <stack>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Analysis/CFG.h"

using namespace clang;
using namespace llvm;
//using namespace std;

//获取Funtion相关语句
class ASTFunctionLoad : public ASTConsumer, public RecursiveASTVisitor < ASTFunctionLoad > {	//get functions

public:
	void HandleTranslationUnit(ASTContext &Context) override {
		TranslationUnitDecl *D = Context.getTranslationUnitDecl();
		TraverseDecl(D);
	}

	bool VisitFunctionDecl(FunctionDecl *FD) {
		if (FD && FD->isThisDeclarationADefinition()) {
			// Add C non-inline function 
			if (!FD->isInlined()){
				functions.push_back(FD);
			}
		}
		return true;
	}

	const std::vector<FunctionDecl *> &getFunctions() const{
		return functions;
	}

private:
	std::vector<FunctionDecl *> functions;
};

//被调用函数
class ASTCalledFunctionLoad : public RecursiveASTVisitor<ASTCalledFunctionLoad> {
public:
	bool VisitCallExpr(CallExpr *E) {
		if (FunctionDecl *FD = E->getDirectCallee()) {
			functions.insert(FD);
		}
		return true;
	}

	const std::vector<FunctionDecl *> getFunctions() {
		return std::vector<FunctionDecl *>(functions.begin(), functions.end());
	}

private:
	std::set<FunctionDecl *> functions;
};

//调用函数语句
class ASTCallExprLoad : public RecursiveASTVisitor<ASTCallExprLoad> {

public:
	bool VisitCallExpr(CallExpr *E) {
		call_exprs.push_back(E);
		return true;
	}

	const std::vector<CallExpr *> getCallExprs() {
		return call_exprs;
	}

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
class ASTVariableLoad : public RecursiveASTVisitor<ASTVariableLoad> {

public:
	bool VisitDeclStmt(DeclStmt *S) {
		for (auto D : S->decls()) {
			if (VarDecl *VD = dyn_cast<VarDecl>(D)) {
				variables.push_back(VD);
			}
		}
		return true;
	}

	const std::vector<VarDecl *> getVariables() {
		return variables;
	}

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
class ASTCXXMethodDeclLoad : public ASTConsumer, public RecursiveASTVisitor<ASTCXXMethodDeclLoad> {
public:
	void HandleTranslationUnit(ASTContext &Context) override {
		TranslationUnitDecl *D = Context.getTranslationUnitDecl();
		TraverseDecl(D);
	}

	bool VisitClassDecl(CXXMethodDecl *rd) {
		cxxmds.insert(rd);
		return true;
	}

	const std::vector<CXXMethodDecl *> getClassDecl() {
		return std::vector<CXXMethodDecl *>(cxxmds.begin(), cxxmds.end());
	}

private:
	std::set<CXXMethodDecl *> cxxmds;
};

typedef enum
{
	common,inclass
}methodType;

//函数调用关系图
class callgraph{
public:
	callgraph(FunctionDecl* f1)
	{
		cur = f1;
		callerNum = 0;
		calleeNum = 0;
		ifCheck = 0;
		type = common;
		classDecl = NULL;
	}
	callgraph(FunctionDecl* f1, FunctionDecl* f2)
	{
		cur = f1;
		callee[0] = f2;
		callerNum = 0;
		calleeNum = 1;
		ifCheck = 0;
		type = common;
		classDecl = NULL;
	}
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
	//获取当前函数语句块信息指针
	std::unique_ptr<CFG> get_cfg()	
	{
		return CFG::buildCFG(cur, cur->getBody(), &cur->getASTContext(), CFG::BuildOptions());
	}

	//打印cfg信息，使用dump()
	void print_cfg()
	{
		get_cfg()->dump(LangOptions(), true);
	}

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
};

callgraph* findById(std::vector<callgraph*> Callgraph, std::string id);		
void ifcheck(std::vector<callgraph*> cg, callgraph* t);		
void resetIfCheck(std::vector<callgraph*>Callgraph);		
void getRing(std::vector<callgraph*>& Callgraph, int n, std::vector<FunctionDecl*>& ringVector);	
void printCallGraph(std::vector<callgraph*> Callgraph);	

