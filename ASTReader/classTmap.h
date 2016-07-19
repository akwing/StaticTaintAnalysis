#ifndef CLASSTMAP_H
#define CLASSTMAP_H
#include "AST.h"

using namespace clang;
using namespace std;

class classTmap
{
public:
	classTmap();
	void setCXXRecordDecl(CXXRecordDecl* cxxrd);
	void addMethod(CXXMethodDecl* md);
	void addVar(FieldDecl* fd);
	const std::vector<CXXMethodDecl*>& get_cxxmds();
	const std::vector<FieldDecl*>& get_fds();
	CXXRecordDecl* get_cxxrd()const;
	int getMethodNum();
	int getVarNum();
	CTmap* getMap();

public://for tmap
	void classCopy(classTmap* temp);
	void classUnion(classTmap* m,classTmap* a, classTmap* b);
	void classClear();


private:
	CXXRecordDecl* rd;
	std::vector<CXXMethodDecl*> cxxmds;
	std::vector<FieldDecl*> fds;
	CTmap map;
	int methodNum;
	int varNum;
	//int publicVarNum;
	//int privateVarNum;
	//int protectedVarNum;
};

void printClassTmap(std::vector<classTmap*> CT);
bool if_find_class(vector<classTmap*>ClassTmap, CXXRecordDecl* rd);

#endif
