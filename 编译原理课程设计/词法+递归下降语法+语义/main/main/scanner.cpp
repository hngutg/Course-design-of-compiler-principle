#include "pch.h"
#include "scanner.h"

token* Scanner::getTokenList(FILE* fpin) {
	WordAnalyse(fpin);
	return tokenlist;
}

void Scanner::WordAnalyse(FILE* fpin) {
	int line = 1;//ȷ����ǰ����
	int index = 0;//tokenlist������±�

	char ch = fgetc(fpin);
	while (ch != EOF) {
		if (IsFilter(ch)) {//�ж��Ƿ��ǿո���ַ�    ��Щ�ַ�����Ҫ����tokenlist��
			if (ch == '\n') {
				line++;
			}
			ch = fgetc(fpin);
		}
		else if (IsLetter(ch)) {//�ж��Ƿ��Ǳ�ʶ����ؼ���
			string name = "";
			name += ch;
			ch = fgetc(fpin);
			while (IsLetter(ch) || IsDigit(ch)) {//��ʾ����һ��Ϊ��ĸ��֮��Ϊ��ĸ������
				name += ch;
				ch = fgetc(fpin);
			}
			//ch��������ĸ�����֣���ʱch�Ѿ������ȡ��һ���ַ��Ĳ�������������һ��ch = fgetc(fpin)
			//��ȥ�ж������ʶ���Ƿ��ǹؼ���
			int judge = IfisreservedWords(name);//��Ϊ�ؼ��֣�judge�����Ӧ�ؼ�����reservedWords�����еı��
			if (judge>=0) {//��Ϊ�ؼ���
				tokenlist[index].lineshow = line;
				tokenlist[index].word = reservedWords[judge];
				index++;
			}
			else {//��Ϊ��ͨ�ı�ʶ��
				tokenlist[index].lineshow = line;
				tokenlist[index].word.str = name;
				tokenlist[index].word.tok = ID;
				index++;
			}
		}
		else if (IsSeparater(ch)) {//�ж��Ƿ�Ϊ���ֽ��
			LexType type = GetSeparaterType(ch);
			tokenlist[index].word.str = ch;
			tokenlist[index].word.tok = type;
			tokenlist[index].lineshow = line;
			index++;
			ch = fgetc(fpin);
		}
		else if (IsDoubleSeparater(ch)) {//�ж��Ƿ�Ϊ˫�ֽ��
			string arr = "";
			if (ch == '.') {//�ж������±��޽��
				arr += ch;
				ch = fgetc(fpin);
				if (ch == '.') {//�������±��޽��
					arr += ch;
					tokenlist[index].lineshow = line;
					tokenlist[index].word.str = arr;
					tokenlist[index].word.tok = UNDERANGE;
					ch = fgetc(fpin);
					index++;
				}
				else {//ֻ��һ����
					tokenlist[index].lineshow = line;
					tokenlist[index].word.str = '.';
					tokenlist[index].word.tok = DOT;
					index++;
				}
			}
			else if (ch == ':') {//�ж��Ƿ�Ϊ��ֵ��
				//string test = "";
				//test += ch;
				ch = fgetc(fpin);
				tokenlist[index].lineshow = line;
				if (ch == '=') {
					tokenlist[index].word.str = ":=";
					tokenlist[index].word.tok = ASSIGN;
					ch = fgetc(fpin);
				}
				else {//��������
					//test += ch;
					tokenlist[index].word.str = ":";
					tokenlist[index].word.tok = ERROR;
					//���������󣬲�Ӧ��ȡ��һ���ַ���˵����ֻ��һ��:�����chȡ������һ���ַ��Ŀ�ͷ
				}
				index++;
			}
			else if (ch == '\'') {//�ж��ַ���  ������Ϊ'Ҳ��˫�ֽ������Ҫ�������м����charc��
				tokenlist[index].lineshow = line;
				tokenlist[index].word.tok = CHARC;
				string charc = "";
				ch = fgetc(fpin);
				while (ch != '\'') {
					charc += ch;
					ch = fgetc(fpin);
				}
				tokenlist[index].word.str = charc;
				index++;
				ch = fgetc(fpin);
			}
		}
		else if (IsDigit(ch)) {//�ж��Ƿ�Ϊ�޷�������
			string num = "";
			num += ch;
			ch = fgetc(fpin);
			while (IsDigit(ch) || IsLetter(ch)) {//Ϊ��֤����Ĵʷ�������ȷ����ʹ�����磺��123ret2�����֣�����ҲӦ�ð���ȫ������num�У��ٸ���һ��ERROR������Ӧ�ý�123��ret2��
				num += ch;
				ch = fgetc(fpin);
			}
			//��ȥ���num���Ƿ�ȫ������
			bool flag = true;
			for (int i = 0; i < num.length(); i++) {
				if (!IsDigit(num[i])) {//�����ֵ�iλ��������
					flag = false;
					break;
				}
			}
			if (flag) {
				tokenlist[index].word.tok = INTC;
			}
			else {
				tokenlist[index].word.tok = ERROR;
			}
			tokenlist[index].lineshow = line;
			tokenlist[index].word.str = num;
			index++;
			//ͬ��������whileѭ��������Ҫ�ٽ���һ��ch = fgetc(fpin);
		}
		else if (ch == '{') {//ȥע��
		    while (ch != '}') {
				ch = fgetc(fpin);
				if (ch == '\n')
					line += 1;
			}
			ch = fgetc(fpin);
        }
		else {//δ֪�ַ�
		    tokenlist[index].lineshow = line;
			tokenlist[index].word.str = ch;
			tokenlist[index].word.tok = ERROR;
			index++;
			ch = fgetc(fpin);
		}
	}

	//��������ļ���β
	tokenlist[index].lineshow = line;
	tokenlist[index].word.str = ch;
	tokenlist[index].word.tok = ENDFILE;
}

void Scanner::PrintTokenList() {
	int i = 0;
	ofstream mycout0("tokenlist.txt");
	while (tokenlist[i].word.tok != ENDFILE) {
		mycout0 << setw(4) << std::left << tokenlist[i].lineshow << std::left << setw(25) << toString(tokenlist[i].word.tok) << tokenlist[i].word.str << endl;
		i++;
	}
	mycout0 << tokenlist[i].lineshow << " " << toString(tokenlist[i].word.tok) << " " << tokenlist[i].word.str << endl;
	mycout0.close();
}


int Scanner::IfisreservedWords(string ch) {//�ж�ch�Ƿ��ǹؼ���
	for (int i = 0; i < 21; i++) {
		if (ch == reservedWords[i].str) {
			return i;
		}
	}
	return -1;
}

LexType Scanner::GetSeparaterType(char c) {//�õ����ָ�����LexType
	if (c == '+') {
		return PLUS;
	}
	else if (c == '-') {
		return MINUS;
	}
	else if (c == '*') {
		return TIMES;
	}
	else if (c == '/') {
		return OVER;
	}
	else if (c == '(') {
		return LPAREN;
	}
	else if (c == ')') {
		return RPAREN;
	}
	else if (c == '=') {
		return EQ;
	}
	else if (c == '<') {
		return LT;
	}
	else if (c == ';') {
		return SEMI;
	}
	else if (c == ',') {
		return COMMA;
	}
	else if (c == '[') {
		return LMIDPAREN;
	}
	else if (c == ']') {
		return RMIDPAREN;
	}
	else if (c == EOF) {//*******************************????????
		return ENDFILE;
	}
	/*
	else if (c == '.') {
		return DOT;
	}
	*/
	else {
		return ERROR;
	}
}

bool Scanner::IsFilter(char ch) {
	if (ch == ' ') {
		return true;
	}
	else if (ch == '\n') {
		return true;
	}
	else if (ch == '\t') {
		return true;
	}
	else if (ch == '\r') {
		return true;
	}
	else {
		return false;
	}
}

bool Scanner::IsLetter(char ch) {
	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) {
		return true;
	}
	else {
		return false;
	}
}

bool Scanner::IsSeparater(char ch) {
	if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
		return true;
	}
	else if (ch == '(' || ch == ')' || ch == '[' || ch == ']') {
		return true;
	}
	else if (ch == '<' || ch == '=') {
		return true;
	}
	else if (ch == ',' || ch == ';') {
		return true;
	}
	else if (ch == EOF) {
		return true;
	}
	else {
		return false;
	}
}

bool Scanner::IsDoubleSeparater(char ch){
	if (ch == '.' || ch == ':' || ch == '\'') {
		return true;
	}
	else {
		return false;

	}
}

bool Scanner::IsDigit(char ch) {
	if (ch >= '0'&&ch <= '9') {
		return true;
	}
	else {
		return false;
	}
}

string Scanner::toString(LexType type) {
	int t = type;
	switch(t) {
	case 0:return "ENDFILE";
	case 1:return "ERROR";
		/*������*/
	case 2:return "PROGRAM";
	case 3:return "PROCEDURE";
	case 4:return "TYPE";
	case 5:return "VAR";
	case 6:return "IF";
	case 7:return "THEN";
	case 8:return "ELSE";
	case 9:return "FI";
	case 10:return "WHILE";
	case 11:return "DO";
	case 12:return "ENDWH";
	case 13:return "BEGIN";
	case 14:return "END";
	case 15:return "READ";
	case 16:return "WRITE";
	case 17:return "ARRAY";
	case 18:return "OF";
	case 19:return "RECORD";
	case 20:return "RETURN";
	case 21:return "INTEGER";
	case 22:return "CHAR";
		/*���ַ����ʷ���*/
	case 23:return "ID";
	case 24:return "INTC";
	case 25:return "CHARC";

		/*�������*/
	case 26:return "ASSIGN";
	case 27:return "EQ";
	case 28:return "LT";
	case 29:return "PLUS";
	case 30:return "MINUS";
	case 31:return "TIMES";
	case 32:return "OVER";
	case 33:return "LPAREN";
	case 34:return "RPAREN";
	case 35:return "DOT";
	//case 36:return "COLON";
	case 36:return "SEMI";
	case 37:return "COMMA";
	case 38:return "LMIDPAREN";
	case 39:return "RMIDPAREN";
	case 40:return "UNDERANGE";
	}
}