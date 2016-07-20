#include"Tout.h"
Ttable::Ttable()
{
	head = NULL;
}
Ttable::~Ttable()
{
	Node*p = NULL;
	while (p == head)
	{
		p = head;
		while (p != NULL)
		{

			p = p->next;
		}
		delete p;
	}
}

bool Ttable::CreateXmlFile(Node*p, char*file)
{
	//创建一个XML的文档对象。
	TiXmlDocument *myDocument = new TiXmlDocument();
	//创建一个根元素并连接。
	TiXmlElement *RootElement = new TiXmlElement("Taints");
	myDocument->LinkEndChild(RootElement);
	//创建一个Person元素并连接。
	TiXmlElement *PersonElement = new TiXmlElement("Taint");
	RootElement->LinkEndChild(PersonElement);
	//设置Person元素的属性。
	//PersonElement->SetAttribute("type", p->Type);
	//创建name元素、age元素并连接。
	TiXmlElement *NameElement = new TiXmlElement("name");
	TiXmlElement *AgeElement = new TiXmlElement("line");
	PersonElement->LinkEndChild(NameElement);
	PersonElement->LinkEndChild(AgeElement);
	//设置name元素和age元素的内容并连接。
	TiXmlText *NameContent = new TiXmlText(p->Tname);
	TiXmlText *AgeContent = new TiXmlText(p->line);
	NameElement->LinkEndChild(NameContent);
	AgeElement->LinkEndChild(AgeContent);
	//保存到文件
	int i;

	myDocument->SaveFile(file);
	return 1;
}

void Ttable::outTtable()
{
	
}
bool Ttable::insert(const VarDecl*p, int line,string function)
{
	Node *t = makeTnode(p, line,function);
	if (head == NULL)
	{
		head = t;
		head->next = NULL;
	}
	else
	{
		t->next = head->next;
		head->next = t;
	}
	return true;
}
Ttable::Node* Ttable::makeTnode(const VarDecl * p, string line, string function)
{

	
	Node * t = NULL;
//	t = exchange(p, line, function);
	//将行号转换为char类型
	Node * t = new Node;
	t->line = new char[50];
//	sprintf(t->line, "%d", line);
	string str_line = line;
	str_line.copy(t->line,str_line.length(),0);
	*(t->line + str_line.length())= '\0';
	cout<<t->line<<"t->line"<<endl;
	//将名字转换为char类型
	//cout << p->getQualifiedNameAsString().data()<<" 23333" << endl;
	//cout <<(char*) p->getQualifiedNameAsString().data() << " 1111" << endl;
	t->Tname = new char[50];
	string test = (char*)p->getQualifiedNameAsString().data();
	//t->Tname = (char*)test.data();
	test.copy(t->Tname, test.length(), 0);
	*(t->Tname + test.length()) = '\0';
	cout << t->Tname << " t->Tname" << endl;
	//t->Tname = (char*)p->getQualifiedNameAsString().data();
	//cout <<t->Tname <<" 344444" << endl;
	/*为什么不用getDeclName函数获取变量的名字呢,huozhe shi p->getNameAsString*/
	//将函数名字转换为char
	t->Tfunction = new char[100];
	string funtion_name = function;
	//cout << funtion_name << 111212313 << endl;
	//t->Tfunction = (char*)funtion_name.data();
	funtion_name.copy(t->Tfunction, funtion_name.length(), 0);
	*(t->Tfunction + funtion_name.length())= '\0';
	cout << t->Tfunction << " Tfunction" << endl;
	//通过clang得到了该变量所在的文件名。
	t->Tfile = (char*)p->getASTContext().getSourceManager().getFilename(p->getSourceRange().getBegin()).data();
	return t;
	
}
Ttable::Node* Ttable::exchange(const VarDecl * p, int line, string function)
{
	//将行号转换为char类型
	Node * t = new Node;
	t->line = new char[50];
	sprintf(t->line, "%d", line);
	//将名字转换为char类型
	t->Tname = (char*)p->getQualifiedNameAsString().data();
	/*为什么不用getDeclName函数获取变量的名字呢,huozhe shi p->getNameAsString*/
	//将函数名字转换为char
	t->Tfunction = (char*)function.data();
	//通过clang得到了该变量所在的文件名。
	t->Tfile = (char*)p->getASTContext().getSourceManager().getFilename(p->getSourceRange().getBegin()).data();
	return t;

}
void Ttable::listout()
{
	Node * t = NULL;
	t = head;
	while (t != NULL)
	{
		cout << t->Tname << endl;
		cout << t->line << endl;
		cout << t->Tfunction << endl;
		cout << t->Tfile << endl;
		cout << endl;
		t = t->next;
	}
}
void Ttable::XMLout()
{
	char *file = "C:/Users/Chris/Desktop/111/%d.XML";
	char * f = NULL;
	int i = 1;
	Node*p = head;
	while (p != NULL)
	{
		f = new char[50];
		sprintf(f, file, i);
		CreateXmlFile(p, f);
		i++;
		p = p->next;
	}
}

