#include "ASTReader.h"
int main(int argc, char *argv[]) {

	FileSystemOptions opts;
	IntrusiveRefCntPtr<DiagnosticsEngine> Diags = CompilerInstance::createDiagnostics(new DiagnosticOptions());
	CompilerInstance compiler;

	std::unique_ptr<ASTUnit> AU = ASTUnit::LoadFromASTFile(argv[1], compiler.getPCHContainerReader(), Diags, opts);

	//std::cout << AU->getASTFileName().str() << std::endl;
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
	callgraph* temp;
	//add cur->callee
	for (it = func.begin(); it != func.end(); it++)
	{
		callgraph* tempCallNode = new callgraph(*it);
		Callgraph.insert(Callgraph.begin(), tempCallNode); 

		ASTCallExprLoad load2;
		load2.TraverseStmt((*it)->getBody());
		func2 = load2.getCallExprs();
		for (it2 = func2.begin(); it2 != func2.end(); it2++)
		{
			temp = findById(Callgraph, (*it)->getQualifiedNameAsString());
			temp->addCallee((*it2)->getDirectCallee());
			//	std::cout << (*it)->getQualifiedNameAsString() << " -> " << (*it2)->getDirectCallee()->getQualifiedNameAsString() << std::endl;
		}
	}

	//add caller->cur
	std::vector<callgraph*>::iterator it3;
	for (it = func.begin(); it != func.end(); it++)
	{
		ASTCallExprLoad load2;
		load2.TraverseStmt((*it)->getBody());
		func2 = load2.getCallExprs();
		for (it2 = func2.begin(); it2 != func2.end(); it2++)
		{
			for (it3 = Callgraph.begin(); it3 != Callgraph.end(); it3++)
			{
				if ((*it3)->getCur() == (*it2)->getDirectCallee())
					(*it3)->addCaller(*it);
			}
		}
	}

	//std::vector<callgraph*>::iterator it3;
	for (it3 = Callgraph.begin(); it3 != Callgraph.end(); it3++)
	{
		std::cout << (*it3)->getCur()->getQualifiedNameAsString() << ":\n";
		int j = (*it3)->getCallerNum();
		std::cout << "\tcaller:" << j << "\n";
		for (int i = 0; i < j; i++)
			std::cout << "\t\t" << (*it3)->getCaller(i)->getQualifiedNameAsString() << "\n";
		j = (*it3)->getCalleeNum();
		std::cout << "\tcallee:" << j << "\n";
		for (int i = 0; i < j; i++)
			std::cout << "\t\t" << (*it3)->getCallee(i)->getQualifiedNameAsString() << "\n";
	}
	
	ifcheck(Callgraph, *Callgraph.begin());
	
	for (it3 = Callgraph.begin(); it3 != Callgraph.end(); it3++)
	{
		std::cout << (*it3)->getCur()->getQualifiedNameAsString() << ":\n";
		int j = (*it3)->getCallerNum();
		std::cout << "\tcaller:" << j << "\n";
		for (int i = 0; i < j; i++)
			std::cout << "\t\t" << (*it3)->getCaller(i)->getQualifiedNameAsString() << "\n";
		j = (*it3)->getCalleeNum();
		std::cout << "\tcallee:" << j << "\n";
		for (int i = 0; i < j; i++)
			std::cout << "\t\t" << (*it3)->getCallee(i)->getQualifiedNameAsString() << "\n";
	}

	return 0;
}

