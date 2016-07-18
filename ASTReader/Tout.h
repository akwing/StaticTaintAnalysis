#include<stdio.h>
#include<iostream>
#include<string>
#include"AST.h"
#include"tinyxml.h"
#include"tinystr.h"
using namespace std;
class Ttable
{
public:
	struct Node
	{
		char*Tname;
		char*Tfile;
		char*Tfunction;
		char*line;
		Node * next;
	}node;
	Ttable();
	~Ttable();
	void make_node(const VarDecl *p);
	bool insert(const VarDecl *p,  int line,string function);
	void outTtable();
	bool CreateXmlFile(Node*p, char*file);
	VarDecl * serach(const VarDecl * p);
	void listout();
	void XMLout();

private:
	
	Node * head;

	Node* makeTnode(const VarDecl * p, int line, string function);

	Node* exchange(const VarDecl * p, int line, string function);
};

