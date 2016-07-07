#include"classTmap.h"
//classTmap::
classTmap::classTmap()
{
	methodNum = 0;
	varNum = 0;
	rd = NULL;
}
void classTmap::setCXXRecordDecl(CXXRecordDecl* cxxrd)
{
	rd = cxxrd;
}
void classTmap::addMethod(CXXMethodDecl* md)
{
	cxxmds.push_back(md);
	methodNum++;
}
void classTmap::addVar(FieldDecl* fd)
{
	fds.push_back(fd);
	varNum++;
}
const std::vector<CXXMethodDecl*>& classTmap::get_cxxmds()
{
	return cxxmds;
}
const std::vector<FieldDecl*>& classTmap::get_fds()
{
	return fds;
}
CXXRecordDecl* classTmap::get_cxxrd()const
{
	return rd;
}
int classTmap::getMethodNum()
{
	return methodNum;
}
int classTmap::getVarNum()
{
	return varNum;
}

void printClassTmap(std::vector<classTmap*> CT)
{
	std::vector<classTmap*>::iterator it_ct = CT.begin(), it_ct_end = CT.end();
	std::vector<CXXMethodDecl*>::iterator it_cxxmd;
	std::vector<CXXMethodDecl*> cxxmds;
	int methodNum;
	std::vector<FieldDecl*>::iterator it_fd;
	std::vector<FieldDecl*>fds;
	int varNum;
	for (; it_ct != it_ct_end; it_ct++)
	{
		cxxmds = (*it_ct)->get_cxxmds();
		fds = (*it_ct)->get_fds();
		std::cout << (*it_ct)->get_cxxrd()->getQualifiedNameAsString() << "\n";
		it_cxxmd = cxxmds.begin();
		methodNum = (*it_ct)->getMethodNum();
		it_fd = fds.begin();
		varNum = (*it_ct)->getVarNum();
		std::cout << "\tVar:\n";
		if (fds.size() > 0)
		{
			for (int i = 0; i < varNum; i++)
			{
				std::cout << "\t\t" << (*it_fd)->getQualifiedNameAsString() << "\n";
				it_fd++;
			}
		}
		if (cxxmds.size() > 0)
		{
			std::cout << "\tMethod:\n";
			for (int i = 0; i < methodNum; i++)
			{
				std::cout << "\t\t" << (*it_cxxmd)->getQualifiedNameAsString() << "\n";
				it_cxxmd++;
			}
		}
	}
}
