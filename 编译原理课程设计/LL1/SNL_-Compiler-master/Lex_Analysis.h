#include<bits/stdc++.h>
using namespace std;

struct Token{
	int i_line;		//�к�
	string str_type;	//����
	string str_sem;	//��������
	Token* t_next;	//����nextָ��
};

class lex_analysis{
public:
	lex_analysis();
	~lex_analysis();

	int Scan(string file);

	void AddNewToken(int row,string str1,string str2);
	void ResultPrint(int type=0);		//��������
	void result_hjz(vector<Token> &x);
	Token* GetToken();
    int iswrong();
private:
	int i_Line;			//��ǰ����
	int i_IsWrong;		//�ж��Ƿ����
	int i_SmallBr;			// ()
	int i_MidBr;			// []
	int i_BigBr;			// {}
	int i_SgQu;			// ''

	Token * t_first;
	Token * t_last;
};

