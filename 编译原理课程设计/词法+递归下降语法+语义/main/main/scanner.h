#pragma once
#include "pch.h"
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

typedef enum {
	ENDFILE, //�ļ�������
	ERROR,//�����Ա����ڴʷ�����ʱ���ִʷ�����
	//***����Ϊ������**************************
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO, ENDWHILE,
	BEGIN, END, READ, WRITE, ARRAY, OF,
	RECORD, RETURN,
	INTEGER, CHAR,
	//***����Ϊ������**************************
	ID, INTC, CHARC,//��ʶ�����޷����������ַ��������ַ�����ʼ�ͽ��������壩
	ASSIGN, EQ, LT, //��ֵ����ȣ�С��
	PLUS, MINUS, TIMES, OVER,//�Ӽ��˳�
	LPAREN, RPAREN, //��������
	DOT,//�㣨.��
	SEMI, COMMA, //�ֺţ�����
	LMIDPAREN, RMIDPAREN,//���ҷ�����
	UNDERANGE//�����±���޷�

	//ע��ע�Ϳ����ڳ�����ȥ��������������һ�����ʷ���
}LexType;

static struct word {
	string str;   //������Ϣ
	LexType tok; //�ʷ���Ϣ
}reservedWords[21] = { {"program",PROGRAM},{"type",TYPE},{"var",VAR},
	{"procedure",PROCEDURE},{"begin",BEGIN},{"end",END},{"array",ARRAY},
	{"of",OF},{"record",RECORD},{"if",IF},{"then",THEN},{"else",ELSE},{"fi",FI},
	{"while",WHILE},{"do",DO},{"endwh",ENDWHILE},{"read",READ},{"write",WRITE},
	{"return",RETURN},{"integer",INTEGER},{"char",CHAR} };	//������

//���ʵ�token��ʾ
struct token {
	int lineshow;//��Դ�����е�����
	word word;
};

class Scanner {
private:
	bool IsFilter(char c);//�ж��Ƿ��ǿո���ַ�

	bool IsLetter(char c);//�ж��Ƿ�����ĸ

	//bool IsOperator(char c);//�ж��Ƿ��ǲ�����

	bool IsSeparater(char c);//�ж��Ƿ��ǵ��ֽ��

	bool IsDoubleSeparater(char c);//�ж��Ƿ�Ϊ˫�ֽ��

	bool IsDigit(char c);//�ж��Ƿ�Ϊ�޷�������

	//bool IsKeyWord(string ch);//�ж��Ƿ�Ϊ������

	void WordAnalyse(FILE* fpin);//���ʷ�����������������tokenlist

	LexType GetSeparaterType(char charList);//��ȡ���ֽ����LexType

	token tokenlist[10000];//���ڱ���token��Ϣ

	int IfisreservedWords(string ch);//�ж��Ƿ�Ϊ�ؼ���

	string toString(LexType type);//��ÿһ���ʵ�LexTypeת����Ϊstring���������

public:
	token* getTokenList(FILE* fpin);

	void PrintTokenList();
};