#include<bits/stdc++.h>
using namespace std;

struct Token{
	int i_line;		//行号
	string str_type;	//类型
	string str_sem;	//单词语义
	Token* t_next;	//链表next指针
};

class lex_analysis{
public:
	lex_analysis();
	~lex_analysis();

	int Scan(string file);

	void AddNewToken(int row,string str1,string str2);
	void ResultPrint(int type=0);		//输出结果，
	void result_hjz(vector<Token> &x);
	Token* GetToken();
    int iswrong();
private:
	int i_Line;			//当前行数
	int i_IsWrong;		//判断是否出错
	int i_SmallBr;			// ()
	int i_MidBr;			// []
	int i_BigBr;			// {}
	int i_SgQu;			// ''

	Token * t_first;
	Token * t_last;
};

