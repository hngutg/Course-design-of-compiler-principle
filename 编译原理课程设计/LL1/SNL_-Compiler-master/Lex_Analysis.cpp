
#include<bits/stdc++.h>

#include"lex_Analysis.h"

string Reservations[21]=
{
	"program","type","var","procedure","begin",
	"end","array","of","record","if","then",
	"else","fi","while","do","endwh","read",
	"write","return","integer","char"
};

lex_analysis::lex_analysis(){
	i_Line=1;		//第一行
	i_IsWrong=0;	//0表示没有出错
	i_SmallBr=0;	//没有遇上(
	i_MidBr=0;		//没有遇上[
	i_BigBr=0;		//没有遇上{
	i_SgQu=0;		//没有遇上'

	t_first = NULL;
	t_last = NULL;
}

lex_analysis::~lex_analysis(){
	Token* t_tmp =  t_first;
	while(t_first != t_last){
		t_tmp = t_first;
		t_first = t_first->t_next;
		delete t_tmp;
	}
}



Token* lex_analysis::GetToken(){
	return t_first;
}
int lex_analysis::iswrong()
{
    return i_IsWrong;
}
void lex_analysis::result_hjz(vector<Token> &x)
{
    Token* t_tmp =  t_first;
	while(t_tmp != t_last){
        x.push_back(*t_tmp);
		t_tmp = t_tmp->t_next;
	}
}

void lex_analysis::AddNewToken(int r,string str1,string str2){
	if(t_first == NULL){
		t_first = new Token;
		t_first->i_line = r;
		t_first->str_type = str1;
		t_first->str_sem = str2;
		t_first->t_next = NULL;
		t_last = t_first;
	}
	else{
		t_last->t_next = new Token;
		t_last->t_next->i_line = r;
		t_last->t_next->str_type = str1;
		t_last->t_next->str_sem = str2;
		t_last->t_next->t_next = NULL;
		t_last = t_last->t_next;
	}
}

int lex_analysis::Scan(string file){
	FILE* Input = fopen(file.c_str(),"r");
	if(Input == NULL){
		i_IsWrong++;
		cout<<"文件错误,打开源文件失败"<<endl;
	}
	else{
		char ch;
		ch =  fgetc(Input);
		string str_tmp;
		while(ch != EOF){
			if(isalpha(ch) != 0){ //判断标识符(含保留字)
				str_tmp = "";
				while(isalpha(ch) != 0 || isdigit(ch) != 0){
					str_tmp += ch;
					ch = fgetc(Input);
				}
				int i_IsReserve = 0;	//判断标识符是否是保留字
				for(int i=0;i<21;i++){
					if(str_tmp == Reservations[i]){
						i_IsReserve = 1;
						AddNewToken(i_Line,"RESERVEWORD",str_tmp);//保留字
						break;
					}
				}
				if(i_IsReserve == 0){
					AddNewToken(i_Line,"ID",str_tmp);//标识符
				}
			}
			else if(isdigit(ch) != 0){  //判断数字
				str_tmp = "";
				while(isdigit(ch) != 0){
					str_tmp += ch;
					ch = fgetc(Input);
				}
				if(isalpha(ch)!=0){
					i_IsWrong++;
					cout<<"第"<<i_Line<<"行	"<<"词法错误,数字上的错误后缀！"<<endl;
				}
				else
					AddNewToken(i_Line,"INTEGER",str_tmp);
			}
			else if(ch=='+'||ch=='-'||ch=='*'||ch=='/'||ch==';'||ch=='<'||ch=='='||ch==','||ch=='.'){
				str_tmp="";
				str_tmp+=ch;
				if(ch=='.'){
					ch=fgetc(Input);
					if(ch=='.')
					{
						AddNewToken(i_Line,"RANGE","..");  //数组下标
						ch=fgetc(Input);
					}
					else
					{
						AddNewToken(i_Line,"S_CHAR",".");

					}
				}
				else{
					AddNewToken(i_Line,"S_CHAR",str_tmp); //单分界符
					ch=fgetc(Input);
				}
			}
			else if(ch=='('||ch==')'||ch=='['||ch==']'||ch=='{'||ch=='}'){
				switch(ch){
					case '(':
						i_SmallBr++;
						AddNewToken(i_Line,"S_CHAR","(");
						break;
					case ')':
						i_SmallBr--;
						AddNewToken(i_Line,"S_CHAR",")");
						break;
					case '[':
						i_MidBr++;
						AddNewToken(i_Line,"S_CHAR","[");
						break;
					case ']':
						i_MidBr--;
						AddNewToken(i_Line,"S_CHAR","]");
						break;
					case '{':
						i_BigBr++;
						ch=fgetc(Input);
						while(ch!='}'&&ch!=EOF||ch=='\n')
						{
							if(ch=='\n')
								i_Line++;
							ch=fgetc(Input);
						}
						break;
					case '}':
						i_BigBr--;
						break;
					default:break;
				}
				ch=fgetc(Input);
			}
			else if(ch=='\''){
				i_SgQu++;
				//AddNewToken(i_Line,"S_CHAR","\'");
				ch=fgetc(Input);
				str_tmp="";
				str_tmp+=ch;

				if(ch=='\\'){
					ch=fgetc(Input);
					if(ch=='a'||ch=='b'||ch=='f'||ch=='n'||ch=='r'||ch=='t'||ch=='v'||ch=='\\'||ch=='\''||ch=='\"'||ch=='0'){
						str_tmp+=ch;
					}
				}
				AddNewToken(i_Line,"ID",str_tmp); //字符常量
				ch=fgetc(Input);
				if(ch!='\''){
					i_IsWrong++;
					cout<<"第"<<i_Line<<"行	"<<"词法错误,过长的字符！"<<endl;
					ch=fgetc(Input);
				}
				while(ch!='\''&&ch!='\n'){
					ch=fgetc(Input);
				}
				if(ch=='\''){
					i_SgQu--;
					//AddNewToken(i_Line,"S_CHAR","\'");
					ch=fgetc(Input);
				}
			}
			else if(ch==':')
			{
				ch=fgetc(Input);
				if(ch=='=')
				{
					AddNewToken(i_Line,"D_CHAR",":="); //双分界符

					ch=fgetc(Input);
				}
				else
				{
					i_IsWrong++;
					cout<<"第"<<i_Line<<"行	"<<"词法错误,未识别的字符':'，是否少输入了'='！"<<endl;
				}
			}
			else if(ch==' '||ch=='\t')
				ch=fgetc(Input);
			else if(ch=='\n')
			{
				ch=fgetc(Input);

				i_SmallBr=0;
				i_MidBr=0;
				i_BigBr=0;
				i_SgQu=0;
				i_Line++;
			}
			else
			{
				i_IsWrong++;
				cout<<"第"<<i_Line<<"行	"<<"词法错误,未识别的字符'"<<ch<<"'！"<<endl;
				ch=fgetc(Input);
			}
		}
		if(i_BigBr<0)  //注释匹配
		{
			i_IsWrong++;
			cout<<"第"<<i_Line<<"行	"<<"语法错误,缺少"<<i_BigBr<<"'{'！"<<endl;
		}
		else if(i_BigBr>0)
		{
			i_IsWrong++;
			cout<<"第"<<i_Line<<"行	"<<"语法错误,缺少"<<i_BigBr<<" '}'！"<<endl;
		}
		AddNewToken(i_Line,"EOF","");  //文件结束
	}
	return 1;
}

void lex_analysis::ResultPrint(int type)//默认参数0表示输出到控制台，1表示输出到文本
{
	Token * t_temp=t_first;
	if(type == 0){
		while(t_temp!=t_last)
		{
			cout<<t_temp->i_line<<'\t'<<t_temp->str_type<<'\t'<<t_temp->str_sem<<endl;
			t_temp=t_temp->t_next;
		}
		cout<<t_temp->i_line<<'\t'<<t_temp->str_type<<'\t'<<t_temp->str_sem<<endl;
	}
	else if(type == 1){
		ofstream output("answer/lex_result.txt",ios::trunc);
		while(t_temp!=t_last)
		{
			output<<t_temp->i_line<<'\t'<<t_temp->str_type<<'\t'<<t_temp->str_sem<<endl;
			t_temp=t_temp->t_next;
		}
		output<<t_temp->i_line<<'\t'<<t_temp->str_type<<'\t'<<t_temp->str_sem<<endl;
		output.close();
	}

}
