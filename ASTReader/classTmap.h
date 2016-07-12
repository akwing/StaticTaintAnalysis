#ifndef CLASS_TMAP_H
#define CLASS_TMAP_H
#include "AST.h"
class classTmap
{
public:
	classTmap();
	classTmap(classTmap&);
	~classTmap();
	void setCXXRecordDecl(CXXRecordDecl* cxxrd);
	void addMethod(CXXMethodDecl* md);
	void addVar(FieldDecl* fd);
	const std::vector<CXXMethodDecl*>& get_cxxmds();
	const std::vector<FieldDecl*>& get_fds();
	CXXRecordDecl* get_cxxrd()const;
	int getMethodNum();
	int getVarNum();
	CTmap* getTmap();
	CXXRecordDecl* get_rd();
	void clearTmap(){tmap->clear();}
private:
	CXXRecordDecl* rd;
	std::vector<CXXMethodDecl*> cxxmds;
	std::vector<FieldDecl*> fds;
	CTmap* tmap;
	int methodNum;
	int varNum;
	//int publicVarNum;
	//int privateVarNum;
	//int protectedVarNum;
};

void printClassTmap(std::vector<classTmap*> CT);
#endif
