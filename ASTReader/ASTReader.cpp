#include <iostream>
#include <vector>

#include "clang/Frontend/ASTUnit.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Basic/FileSystemOptions.h"
#include "clang/AST/RecursiveASTVisitor.h"

using namespace clang;
using namespace llvm;

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

class callgraph{
public:
	callgraph(FunctionDecl* f1, FunctionDecl* f2)
	{
		callee = f1;
		caller = f2;
	}
	FunctionDecl* getCaller()
	{
		return caller;
	}
	FunctionDecl* getCallee()
	{
		return callee;
	}
private:
	FunctionDecl* caller;
	FunctionDecl* callee;
};

int main(int argc, char *argv[]) {
	
	FileSystemOptions opts;
	IntrusiveRefCntPtr<DiagnosticsEngine> Diags = CompilerInstance::createDiagnostics(new DiagnosticOptions());
	CompilerInstance compiler;
	
	std::unique_ptr<ASTUnit> AU = ASTUnit::LoadFromASTFile(argv[1], compiler.getPCHContainerReader(), Diags, opts);
	
	std::cout << AU->getASTFileName().str() << std::endl;
	
	ASTContext &context = AU->getASTContext();
	ASTFunctionLoad load;
	load.HandleTranslationUnit(context);
	std::vector<FunctionDecl*>  func = load.getFunctions();


	std::vector<FunctionDecl*>::iterator it;
	//for (it = func.begin(); it != func.end(); it++)
	//	std::cout << (*it)->getQualifiedNameAsString() << std::endl;

	
	std::vector<CallExpr*>  func2;
	std::vector<CallExpr*>::iterator it2;
	std::vector<callgraph*> Callgraph;
	for (it = func.begin(); it != func.end(); it++)
	{
		ASTCallExprLoad load2;
		load2.TraverseStmt((*it)->getBody());
		func2 = load2.getCallExprs();
		for (it2 = func2.begin(); it2 != func2.end(); it2++)
		{
			callgraph* temp1 =new callgraph(*it, (*it2)->getDirectCallee());
			Callgraph.insert(Callgraph.begin(),temp1);
			//std::cout << (*it)->getQualifiedNameAsString() << " -> " << (*it2)->getDirectCallee()->getQualifiedNameAsString() << std::endl;
		}
	}
	std::vector<callgraph*>::iterator it3;
	for (it3 = Callgraph.begin(); it3 != Callgraph.end(); it3++)
		std::cout << (*it3)->getCallee()->getQualifiedNameAsString() << " -> " << (*it3)->getCaller()->getQualifiedNameAsString() << "\n";
	return 0;
	
	return 0;
}
