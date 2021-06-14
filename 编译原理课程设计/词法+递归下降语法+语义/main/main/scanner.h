#pragma once
#include "pch.h"
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

typedef enum {
	ENDFILE, //文件结束符
	ERROR,//错误，以表明在词法分析时出现词法错误
	//***以下为保留字**************************
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWHILE,
	BEGIN, END, READ, WRITE, ARRAY, OF,
	RECORD, RETURN,
	INTEGER, CHAR,
	//***以上为保留字**************************
	ID, INTC, CHARC,//标识符，无符号整数，字符串（由字符串起始和结束符定义）
	ASSIGN, EQ, LT, //赋值，相等，小于
	PLUS, MINUS, TIMES, OVER,//加减乘除
	LPAREN, RPAREN, //左右括号
	DOT,//点（.）
	SEMI, COMMA, //分号，逗号
	LMIDPAREN, RMIDPAREN,//左右方括号
	UNDERANGE//数组下标界限符

	//注：注释可以在程序中去掉不用在这里列一个单词分类
}LexType;

static struct word {
	string str;   //语义信息
	LexType tok; //词法信息
}reservedWords[21] = { {"program",PROGRAM},{"type",TYPE},{"var",VAR},
	{"procedure",PROCEDURE},{"begin",BEGIN},{"end",END},{"array",ARRAY},
	{"of",OF},{"record",RECORD},{"if",IF},{"then",THEN},{"else",ELSE},{"fi",FI},
	{"while",WHILE},{"do",DO},{"endwh",ENDWHILE},{"read",READ},{"write",WRITE},
	{"return",RETURN},{"integer",INTEGER},{"char",CHAR} };	//保留字

//单词的token表示
struct token {
	int lineshow;//在源程序中的行数
	word word;
};

class Scanner {
private:
	bool IsFilter(char c);//判断是否是空格等字符

	bool IsLetter(char c);//判断是否是字母

	//bool IsOperator(char c);//判断是否是操作符

	bool IsSeparater(char c);//判断是否是单分界符

	bool IsDoubleSeparater(char c);//判断是否为双分界符

	bool IsDigit(char c);//判断是否为无符号整数

	//bool IsKeyWord(string ch);//判断是否为保留字

	void WordAnalyse(FILE* fpin);//做词法分析，函数中填入tokenlist

	LexType GetSeparaterType(char charList);//获取单分界符的LexType

	token tokenlist[10000];//用于保存token信息

	int IfisreservedWords(string ch);//判断是否为关键字

	string toString(LexType type);//将每一个词的LexType转化成为string，用于输出

public:
	token* getTokenList(FILE* fpin);

	void PrintTokenList();
};