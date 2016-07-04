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

class ASTCalledFunctionLoad : public RecursiveASTVisitor<ASTCalledFunctionLoad> {	//get call functions

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
//test STMT 

class ASTStmtLoad : public RecursiveASTVisitor<ASTStmtLoad> {	//get stmt

public:
	bool VisitStmt(Stmt *E) {//////////////////////
		stmts.insert(E);
		return true;
	}

	const std::vector<Stmt *> getStmts() {
		return std::vector<Stmt *>(stmts.begin(), stmts.end());
	}

private:
	std::set<Stmt *> stmts;
};
//end test STMT

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


class callgraph{
public:
	callgraph(FunctionDecl* f1)
	{
		cur = f1;
		callerNum = 0;
		calleeNum = 0;
		ifCheck = 0;
	}
	callgraph(FunctionDecl* f1, FunctionDecl* f2)
	{
		cur = f1;
		callee[0] = f2;
		callerNum = 0;
		calleeNum = 1;
		ifCheck = 0;
	}
	FunctionDecl* getCaller(int i);
	FunctionDecl* getCallee(int i);
	FunctionDecl* getCur();
	int getCallerNum();
	int getCalleeNum();
	void addCaller(FunctionDecl* otherFD);
	void addCallee(FunctionDecl* otherFD);
	void delCallee(FunctionDecl* otherFD);
	int ifCheck;
	std::unique_ptr<CFG> get_cfg()
	{
		return CFG::buildCFG(cur, cur->getBody(), &cur->getASTContext(), CFG::BuildOptions());
	}
	void print_cfg()
	{
		get_cfg()->dump(LangOptions(), true);
	}
private:
	FunctionDecl* caller[10];
	FunctionDecl* cur;
	FunctionDecl* callee[10];
	//std::unique_ptr<CFG> cfg;
	int callerNum, calleeNum;
};

//callgraph* findById(std::vector<callgraph*> Callgraph, std::string id);

callgraph* findById(std::vector<callgraph*> Callgraph, std::string id);
void ifcheck(std::vector<callgraph*> cg, callgraph* t);
void resetIfCheck(std::vector<callgraph*>Callgraph);
void getRing(std::vector<callgraph*>& Callgraph, int n, std::vector<FunctionDecl*>& ringVector);
void printCallGraph(std::vector<callgraph*> Callgraph);
