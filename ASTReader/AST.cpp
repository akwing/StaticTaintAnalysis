#include "AST.h"
//ASTFunctionLoad
void ASTFunctionLoad::HandleTranslationUnit(ASTContext &Context){
	TranslationUnitDecl *D = Context.getTranslationUnitDecl();
	TraverseDecl(D);
}

bool ASTFunctionLoad::VisitFunctionDecl(FunctionDecl *FD) {
	if (FD && FD->isThisDeclarationADefinition()) {
		// Add C non-inline function 
		if (!FD->isInlined()){
			functions.push_back(FD);
		}
	}
	return true;
}

const std::vector<FunctionDecl *> &ASTFunctionLoad::getFunctions() const{
	return functions;
}

//ASTCalledFunctionLoad
bool ASTCalledFunctionLoad::VisitCallExpr(CallExpr *E) {
	if (FunctionDecl *FD = E->getDirectCallee()) {
		functions.insert(FD);
	}
	return true;
}

const std::vector<FunctionDecl *> ASTCalledFunctionLoad::getFunctions() {
	return std::vector<FunctionDecl *>(functions.begin(), functions.end());
}

//ASTCallExprLoad
bool ASTCallExprLoad::VisitCallExpr(CallExpr *E) {
	call_exprs.push_back(E);
	return true;
}

const std::vector<CallExpr *> ASTCallExprLoad::getCallExprs() {
	return call_exprs;
}

//ASTVarDeclLoad
bool ASTVarDeclLoad::VisitDeclStmt(DeclStmt *S) {
	for (auto D : S->decls()) {
		if (VarDecl *VD = dyn_cast<VarDecl>(D)) {
			variables.push_back(VD);
		}
	}
	return true;
}
const std::vector<VarDecl *> ASTVarDeclLoad::getVariables() {
	return variables;
}

//构造函数
callgraph::callgraph(FunctionDecl* f1)
{
	cur = f1;
	callerNum = 0;
	calleeNum = 0;
	ifCheck = 0;
	type = common;
	classDecl = NULL;
	map = new CTmap();
	paramNum = 0;
	varNum = 0;
	returnVar = new Tainted_Attr();
}

//构造函数（带有一个callee）
callgraph::callgraph(FunctionDecl* f1, FunctionDecl* f2)
{
	cur = f1;
	callee[0] = f2;
	callerNum = 0;
	calleeNum = 1;
	ifCheck = 0;
	type = common;
	classDecl = NULL;
	map = new CTmap();
	paramNum = 0;
	varNum = 0;
	returnVar = new Tainted_Attr();
}

FunctionDecl* callgraph::getCaller(int i)
{
	if (0 <= i&& i < 10)
		return caller[i];
	else
		return NULL;
}

FunctionDecl* callgraph::getCallee(int i)
{
	if (0 <= i&&i < 10)
		return callee[i];
	else
		return NULL;
}

FunctionDecl* callgraph::getCur()
{
	return cur;
}

int callgraph::getCallerNum()
{
	return callerNum;
}

int callgraph::getCalleeNum()
{
	return calleeNum;
}

void callgraph::addCaller(FunctionDecl* otherFD){
	caller[callerNum] = otherFD;
	callerNum++;
}

void callgraph::addCallee(FunctionDecl* otherFD){
	callee[calleeNum] = otherFD;
	calleeNum++;
}


void callgraph::delCallee(FunctionDecl* otherFD)
{
	int j;
	for (int i = 0; i < calleeNum&&i < 10; i++)
		if (callee[i] == otherFD)
		{
			j = i;
			break;
		}
	for (int i = j; i < calleeNum - 1 && i < 9; i++)
	{
		callee[i] = callee[i + 1];
	}
	callee[callerNum] = NULL;
	calleeNum--;
}

//函数名查找callgraph实例
callgraph* findById(std::vector<callgraph*> Callgraph, std::string id)
{
	std::vector<callgraph*>::iterator it;
	for (it = Callgraph.begin(); it != Callgraph.end(); it++)
	{
		if (id == (*it)->getCur()->getQualifiedNameAsString())
			return *it;
	}
	return NULL;
}

//查找callgraph中的环，对其进行除环操作
void ifcheck(std::vector<callgraph*> cg, callgraph* t)
{
	t->ifCheck = -1;
	for (int i = 0; i < t->getCalleeNum(); i++)
	{
		FunctionDecl* tempt = t->getCallee(i);
		callgraph* tempc = findById(cg, tempt->getQualifiedNameAsString());
		if (tempc != NULL)
		{
			if (tempc->ifCheck == -1)
			{
				t->delCallee(tempt);
				i--;
			}
			else
				ifcheck(cg, tempc);
		}
	}
	t->ifCheck = 1;
}

//重置ifcheck
void resetIfCheck(std::vector<callgraph*>Callgraph)
{
	std::vector<callgraph*>::iterator it;
	for (it = Callgraph.begin(); it < Callgraph.end(); it++)
		(*it)->ifCheck = 0;
}

//打印callgraph中存在的环的信息
void getRing(std::vector<callgraph*>&Callgraph, int n, std::vector<FunctionDecl*>& ringVector)
{
	std::stack<FunctionDecl* > ring;
	std::vector<callgraph*>::iterator it,it4;
	std::vector<FunctionDecl*>::iterator it2,it3;
	//FunctionDecl* temp;
	it = Callgraph.begin() + n;
	if ((*it)->ifCheck == 0)//new node
	{
		ringVector.push_back((*it)->getCur());
		int calleeNum = (*it)->getCalleeNum();
		(*it)->ifCheck = -1;
		for (int i = 0; i < calleeNum; i++)
		{
			for (it4 = Callgraph.begin(); it4 < Callgraph.end();it4++)
				if ((*it4)->getCur() == (*it)->getCallee(i))
				{
					getRing(Callgraph, it4-Callgraph.begin(), ringVector);
					break;
				}	
		}
		(*it)->ifCheck = 1;
		ringVector.pop_back();
	}
	else if ((*it)->ifCheck == -1)
	{
		for (it2 = ringVector.begin(); it2 < ringVector.end(); it2++)
		{
			if ((*it2) == (*it)->getCur())
			break;
		}
		//print
		for (it3 = it2; it3 < ringVector.end(); it3++)
		{
			std::cout << (*it3)->getQualifiedNameAsString() << " ";
		}
		std::cout << std::endl;
	}
	else//ifCheck==1
	{
		return;
	}
}

void printCallGraph(std::vector<callgraph*> Callgraph)
{
	std::vector<callgraph*>::iterator it3;
	for (it3 = Callgraph.begin(); it3 != Callgraph.end(); it3++)
	{
		std::cout << (*it3)->getCur()->getQualifiedNameAsString() << "\n";
		int paramNum = (*it3)->getCur()->getNumParams();
		std::cout << "\tParamNum: " << paramNum << "\n";
		if (paramNum > 0)
			for (unsigned i = 0; i < (*it3)->getCur()->getNumParams(); i++)
				std::cout << "\t\t" << (*it3)->getCur()->getParamDecl(i)->getQualifiedNameAsString() << "\n";

		int varNum = (*it3)->getVarNum();
		CTmap map = (*it3)->getCTmap();
		std::cout << "\tVarNum: " << varNum << "\n";
		if (varNum > 0)
		{
			for (int i = 0; i < varNum; i++)
			{
				VarDecl* tt = map.get_VarDecl(i + paramNum);
				std::cout << "\t\t" << tt->getQualifiedNameAsString() << "    ";
				map.getmap(tt)->output();
				std::cout<< "\n";
			}
		}
		
		int j = (*it3)->getCallerNum();
		std::cout << "\tcaller:" << j << "\n";
		for (int i = 0; i < j; i++)
			std::cout << "\t\t" << (*it3)->getCaller(i)->getQualifiedNameAsString() << "\n";
		j = (*it3)->getCalleeNum();
		std::cout << "\tcallee:" << j << "\n";
		for (int i = 0; i < j; i++)
			std::cout << "\t\t" << (*it3)->getCallee(i)->getQualifiedNameAsString() << "\n";
	}
}

//改变方法的类型（普通，类方法）
void callgraph::changeMethodType()
{
	if (type == common)
		type = inclass;
	else
		type = common;
}

//设置方法的实例指针
void callgraph::setRoot(VarDecl* r)
{
	root = r;
}

//获取实例指针
VarDecl* callgraph::getRoot()
{
	return root;
}

//获取方法类型
methodType callgraph::getMethodType()
{
	return type;
}

//设置类定义的指针
void callgraph::setClass(CXXRecordDecl* rd)
{
	classDecl = rd;
}
CXXRecordDecl* callgraph::getClass()
{
	return classDecl;
}

//获取当前函数语句块信息指针
std::unique_ptr<CFG> callgraph::get_cfg()
{
	return CFG::buildCFG(cur, cur->getBody(), &cur->getASTContext(), CFG::BuildOptions());
}

//打印cfg信息，使用dump()
void callgraph::print_cfg()
{
	get_cfg()->dump(LangOptions(), true);
}

//获取当前函数Tmap指针
CTmap& callgraph::getCTmap()
{
	return *map;
}

//添加参数
void callgraph::addParam(VarDecl* vd)
{
	map->insert(vd);
	paramNum++;
}

//添加变量
void callgraph::addVar(VarDecl* vd)
{
	map->insert(vd);
	varNum++;
}

int callgraph::getParamNum()
{ 
	return paramNum; 
}

int callgraph::getVarNum()
{ 
	return varNum; 
}
