#include "AST.h"
#include "callgraph.h"
#include "classTmap.h"
//Callgraph
std::vector<callgraph*> Callgraph;
std::vector<classTmap*> ClassTmap;
//void printCallGraph(std::vector<callgraph*> Callgraph);

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
	std::vector<VarDecl*> varDec;
	std::vector<VarDecl*>::iterator itvarDec;
	std::vector<ParmVarDecl*> parmDec;
	std::vector<ParmVarDecl*>::iterator itParmDec;

	callgraph* temp;
	//add cur->callee
	for (it = func.begin(); it != func.end(); it++)
	{
		callgraph* tempCallNode = new callgraph(*it);
		Callgraph.insert(Callgraph.begin(), tempCallNode); 

		ASTCallExprLoad load2;
		load2.TraverseStmt((*it)->getBody());
		func2 = load2.getCallExprs();
		//添加调用的函数callee
		for (it2 = func2.begin(); it2 != func2.end(); it2++)
		{
			temp = findById(Callgraph, (*it)->getQualifiedNameAsString());
			temp->addCallee((*it2)->getDirectCallee());
			//	std::cout << (*it)->getQualifiedNameAsString() << " -> " << (*it2)->getDirectCallee()->getQualifiedNameAsString() << std::endl;
		}


		//把参数加入Tmap
		int paramNum=(*it)->getNumParams();
		//VarDecl* parmTemp;
		if (paramNum > 0)
		{
			for (int i = 0; i < paramNum; i++)
			{
				tempCallNode->addParam((*it)->getParamDecl(i));
			}
		}
		
		//把中间变量加入Tmap
		ASTVarDeclLoad loadVar;
		loadVar.TraverseStmt((*it)->getBody());
		std::vector<VarDecl*> varList = loadVar.getVariables();
		for (std::vector<VarDecl*>::iterator varIt = varList.begin(); varIt != varList.end(); varIt++)
		{
			//std::cout << (*varIt)->getQualifiedNameAsString() << "\n";
			tempCallNode->addVar(*varIt);
		}
	}

	//添加调用本函数的其他函数caller
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

	std::vector<FunctionDecl*> ringVector;
	getRing(Callgraph, 0, ringVector);

	resetIfCheck(Callgraph);
	ifcheck(Callgraph, *Callgraph.begin());
	
	printCallGraph(Callgraph);

	//check class
	ASTCXXRecordLoad loadClass;
	loadClass.HandleTranslationUnit(context);
	std::vector<CXXRecordDecl*>   cxxrds= loadClass.getClassDecl();
	//std::cout << cxxrds.size()<< "\n";
	std::vector<CXXRecordDecl*>::iterator rd_it,rd_it_end=cxxrds.end();

	for (rd_it = cxxrds.begin(); rd_it != rd_it_end; rd_it++)
	{
		//在ClassTmap中加入新发现的class
		classTmap* curClass = new classTmap();
		curClass->setCXXRecordDecl(*rd_it);
		ClassTmap.push_back(curClass);

		ASTCXXMethodDeclLoad loadClassMethod;
		std::vector<CXXMethodDecl *> cxxmds;
		std::vector<CXXMethodDecl *>::iterator it_cxxmds;
		//std::cout << (*rd_it)->getQualifiedNameAsString() << "\n";
		loadClassMethod.TraverseDecl(*rd_it);
		cxxmds = loadClassMethod.getCXXMethodDecl();
		for (it_cxxmds = cxxmds.begin(); it_cxxmds != cxxmds.end(); it_cxxmds++)
		{
			curClass->addMethod(*it_cxxmds);
			//std::cout << (*it_cxxmds)->getQualifiedNameAsString() << "\n";
		}

		ASTFieldDeclLoad loadClassVar;
		std::vector<FieldDecl*> fds;
		std::vector<FieldDecl*>::iterator it_fds;
		loadClassVar.TraverseDecl(*rd_it);
		fds = loadClassVar.getFieldDecl();
		for (it_fds = fds.begin(); it_fds != fds.end(); it_fds++)
		{
			curClass->addVar(*it_fds);
			//std::cout << (*it_fds)->getQualifiedNameAsString() << "\n";
		}
	}
	printClassTmap(ClassTmap);
	return 0;
}

