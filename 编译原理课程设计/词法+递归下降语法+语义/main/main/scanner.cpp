#include "pch.h"
#include "scanner.h"

token* Scanner::getTokenList(FILE* fpin) {
	WordAnalyse(fpin);
	return tokenlist;
}

void Scanner::WordAnalyse(FILE* fpin) {
	int line = 1;//确定当前行数
	int index = 0;//tokenlist数组的下标

	char ch = fgetc(fpin);
	while (ch != EOF) {
		if (IsFilter(ch)) {//判断是否是空格等字符    这些字符不需要放入tokenlist中
			if (ch == '\n') {
				line++;
			}
			ch = fgetc(fpin);
		}
		else if (IsLetter(ch)) {//判断是否是标识符或关键字
			string name = "";
			name += ch;
			ch = fgetc(fpin);
			while (IsLetter(ch) || IsDigit(ch)) {//表示符第一个为字母，之后为字母或数字
				name += ch;
				ch = fgetc(fpin);
			}
			//ch不再是字母或数字，此时ch已经完成了取下一个字符的操作，不用再来一次ch = fgetc(fpin)
			//再去判断这个标识符是否是关键字
			int judge = IfisreservedWords(name);//若为关键字，judge保存对应关键字在reservedWords数组中的编号
			if (judge>=0) {//若为关键字
				tokenlist[index].lineshow = line;
				tokenlist[index].word = reservedWords[judge];
				index++;
			}
			else {//若为普通的标识符
				tokenlist[index].lineshow = line;
				tokenlist[index].word.str = name;
				tokenlist[index].word.tok = ID;
				index++;
			}
		}
		else if (IsSeparater(ch)) {//判断是否为单分界符
			LexType type = GetSeparaterType(ch);
			tokenlist[index].word.str = ch;
			tokenlist[index].word.tok = type;
			tokenlist[index].lineshow = line;
			index++;
			ch = fgetc(fpin);
		}
		else if (IsDoubleSeparater(ch)) {//判断是否为双分界符
			string arr = "";
			if (ch == '.') {//判断数组下标限界符
				arr += ch;
				ch = fgetc(fpin);
				if (ch == '.') {//是数组下标限界符
					arr += ch;
					tokenlist[index].lineshow = line;
					tokenlist[index].word.str = arr;
					tokenlist[index].word.tok = UNDERANGE;
					ch = fgetc(fpin);
					index++;
				}
				else {//只是一个点
					tokenlist[index].lineshow = line;
					tokenlist[index].word.str = '.';
					tokenlist[index].word.tok = DOT;
					index++;
				}
			}
			else if (ch == ':') {//判断是否为赋值号
				//string test = "";
				//test += ch;
				ch = fgetc(fpin);
				tokenlist[index].lineshow = line;
				if (ch == '=') {
					tokenlist[index].word.str = ":=";
					tokenlist[index].word.tok = ASSIGN;
					ch = fgetc(fpin);
				}
				else {//发生错误
					//test += ch;
					tokenlist[index].word.str = ":";
					tokenlist[index].word.tok = ERROR;
					//若发生错误，不应该取下一个字符，说不定只有一个:，这个ch取到的是一串字符的开头
				}
				index++;
			}
			else if (ch == '\'') {//判断字符串  这里认为'也是双分界符（需要两个且中间的是charc）
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
		else if (IsDigit(ch)) {//判断是否为无符号整数
			string num = "";
			num += ch;
			ch = fgetc(fpin);
			while (IsDigit(ch) || IsLetter(ch)) {//为保证后面的词法分析正确，即使出现如：“123ret2”这种，我们也应该把它全部放入num中，再给它一个ERROR，而不应该将123与ret2拆开
				num += ch;
				ch = fgetc(fpin);
			}
			//再去检查num中是否全是数字
			bool flag = true;
			for (int i = 0; i < num.length(); i++) {
				if (!IsDigit(num[i])) {//若出现第i位不是数字
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
			//同样，由于while循环，不需要再进行一次ch = fgetc(fpin);
		}
		else if (ch == '{') {//去注释
		    while (ch != '}') {
				ch = fgetc(fpin);
				if (ch == '\n')
					line += 1;
			}
			ch = fgetc(fpin);
        }
		else {//未知字符
		    tokenlist[index].lineshow = line;
			tokenlist[index].word.str = ch;
			tokenlist[index].word.tok = ERROR;
			index++;
			ch = fgetc(fpin);
		}
	}

	//最后增加文件结尾
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


int Scanner::IfisreservedWords(string ch) {//判断ch是否是关键字
	for (int i = 0; i < 21; i++) {
		if (ch == reservedWords[i].str) {
			return i;
		}
	}
	return -1;
}

LexType Scanner::GetSeparaterType(char c) {//得到单分隔符的LexType
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
		/*保留字*/
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
		/*多字符单词符号*/
	case 23:return "ID";
	case 24:return "INTC";
	case 25:return "CHARC";

		/*特殊符号*/
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