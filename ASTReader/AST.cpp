#include "AST.h"
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
