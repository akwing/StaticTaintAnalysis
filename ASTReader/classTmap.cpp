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
const vector<CXXMethodDecl*>& classTmap::get_cxxmds()
{
	return cxxmds;
}
const vector<FieldDecl*>& classTmap::get_fds()
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

void printClassTmap(vector<classTmap*> CT)
{
	vector<classTmap*>::iterator it_ct = CT.begin(), it_ct_end = CT.end();
	vector<CXXMethodDecl*>::iterator it_cxxmd;
	vector<CXXMethodDecl*> cxxmds;
	int methodNum;
	vector<FieldDecl*>::iterator it_fd;
	vector<FieldDecl*>fds;
	int varNum;
	for (; it_ct != it_ct_end; it_ct++)
	{
		cxxmds = (*it_ct)->get_cxxmds();
		fds = (*it_ct)->get_fds();
		cout << (*it_ct)->get_cxxrd()->getQualifiedNameAsString() << "\n";
		it_cxxmd = cxxmds.begin();
		methodNum = (*it_ct)->getMethodNum();
		it_fd = fds.begin();
		varNum = (*it_ct)->getVarNum();
		cout << "\tVar:\n";
		if (fds.size() > 0)
		{
			for (int i = 0; i < varNum; i++)
			{
				cout << "\t\t" << (*it_fd)->getQualifiedNameAsString() << "\n";
				it_fd++;
			}
		}
		if (cxxmds.size() > 0)
		{
			cout << "\tMethod:\n";
			for (int i = 0; i < methodNum; i++)
			{
				cout << "\t\t" << (*it_cxxmd)->getQualifiedNameAsString() << "\n";
				it_cxxmd++;
			}
		}
	}
}

bool if_find_class(vector<classTmap*>ClassTmap, CXXRecordDecl* rd)
{
	if (ClassTmap.size() == 0)
		return false;
	vector<classTmap*>::iterator it = ClassTmap.begin(), it_end = ClassTmap.end();
	for (; it != it_end; it++)
	{
		if ((*it)->get_cxxrd() == rd) 
			return true;
	}
	return false;
}

CTmap* classTmap::getMap()
{
	return &map;
}

void classTmap::classCopy(classTmap* temp)
{
	rd = temp->rd;

	cxxmds.clear();
	vector<CXXMethodDecl*>::iterator it_cxxmd=temp->cxxmds.begin(),
		it_cxxmd_end=temp->cxxmds.end();
	for (; it_cxxmd != it_cxxmd_end; it_cxxmd++)
	{
		cxxmds.push_back((*it_cxxmd));
	}
	
	fds.clear();
	vector<FieldDecl*>::iterator it_fd = temp->fds.begin(),
		it_fd_end = temp->fds.end();
	for (; it_fd != it_fd_end; it_fd++)
	{
		fds.push_back((*it_fd));
	}

	map.CopyMap(temp->map);

	methodNum = temp->methodNum;
	varNum = temp->varNum;

}

void classTmap::classUnion(classTmap* m,classTmap* a, classTmap* b)
{
	if (a->rd != b->rd)
		cout << "不允许不同的classTmap的合并" << endl;

	//classTmap map_re;
	m->classCopy(a);
	m->map.unionMap(b->map);
}

void classTmap::classClear()
{
	cxxmds.clear();
	fds.clear();
	map.clear();
}
