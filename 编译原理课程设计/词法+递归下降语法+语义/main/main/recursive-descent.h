#pragma once
#include "pch.h"
#include <vector>

#include "scanner.h"

using namespace std;

//�﷨���ڵ������
typedef enum {
	//**************��־�ڵ�********************
	//���ֽڵ�ֻ��ʾ�ڵ�����ͣ�û�о�������
	ProK,     //����־�ڵ�                     /
	PheadK,   //����ͷ��־�ڵ�                 /
	TypeK,    //����������־�ڵ�               /
	VarK,     //����������־�ڵ�               /
	ProcDecK, //����������־�ڵ�               /
	StmLK,    //������б�־�ڵ�               /

	//**************����ڵ�********************
	DecK,	  //�����ڵ�                       /
	StmtK,    //���ڵ�                       /
	ExpK      //���ʽ�ڵ�
	//����ڵ���ڲ���Ϣ������
}TreeNodeType;

//�����﷨���������ڵ�����
typedef enum {
	ArrayK,   //������
	CharK,    //�ַ�����
	IntegerK, //��������
	RecordK,  //��¼����
	Idk       //�����ͱ�ʶ����Ϊ����
}DecKind;

//�����﷨�������ڵ�����
typedef enum {
	IfK,      //�ж��������
	WhileK,   //ѭ���������
	AssignK,  //��ֵ�������
	ReadK,    //���������
	WriteK,   //д�������
	CallK,    //���������������
	ReturnK   //�����������
}StmtKind;

//�����﷨���ı��ʽ�ڵ������
typedef enum {
	OpK,      //����������
	ConstK,   //����������
	IdEK      //��ʶ������
}ExpKind;

//������̣��������Ĳ�������
typedef enum {
	valparamtype,//ֵ��
	varparamtype//���
}paramType;

//��¼��ǰ����������
typedef enum {
	IdV,//�����Ǳ�־������
	ArrayMembV,//�����������Ա����
	FieldMembV//���������Ա����
}varKind;

union kind
{
	DecKind dec;   //��nodekind=DecKʱ��Ч
	StmtKind stmt; //��nodekind=StmtKʱ��Ч
	ExpKind EXP;   //��nodekind=ExpKʱ��Ч
};

//�����﷨���ڵ�
struct TreeNode {
	vector<TreeNode*> child;//ָ�����﷨��ڵ�ָ��
	TreeNode *sibling = NULL;//ָ���ֵ��﷨���ڵ�ָ��
	int lineno;//���ڼ�¼Դ�����к�
	TreeNodeType nodekind;//��¼�﷨���ڵ�����
	kind k;//��¼�﷨������ڵ�ľ�������
	/*
	union kind//��¼�﷨������ڵ�ľ�������
	{
		DecKind dec;   //��nodekind=DecKʱ��Ч
		StmtKind stmt; //��nodekind=StmtKʱ��Ч
		ExpKind EXP;   //��nodekind=ExpKʱ��Ч
	};*/
	int idnum = 0;//��¼һ���ڵ��б�־���ĸ���
	vector<string> name;//���ڴ洢�ڵ��б�־��������
	string type_name;//��¼����������ǰ�ڵ�Ϊ�������ͣ��������������ͱ�ʶ����ʾʱ��Ч

	int array_low;
	int array_high;//������¼��������½�
	DecKind childType;//��¼����ĳ�Ա����
	paramType paramt;//��¼���̣��������Ĳ�������
	LexType op;//��¼�﷨���ڵ����������ʣ���ȡ��ֵΪ��EQ, LT, PLUS, MINUS, TIMES, OVER

	int num;//�����﷨���ڵ�Ϊ�����ڵ㣬�˽ڵ�������¼��������Ĵ�С

	varKind vark;//��¼�﷨���ڵ�����¼�ı�������𣨱�־���������Ա�����Ա��
	//**************************************************************֮�����Щ�ڵ��Ǹ�ɶ�İ�������
};

static TreeNode* temp_name;//��ʱ��������Stm��������Ҫʹ�ã����ƴ��������Ǳ���Ⱥ����ı������ƣ�ֱ���õ��Ⱥ��ұߵı�����һ�������

class Parse {
public:
	void getResult(token s[]);
	void PrintParseTree();//�﷨���������
	TreeNode* getRoot();//�õ��﷨���ĸ��ڵ㣨�������������
private:
	token *s;
	int opposition = 0;//���ڼ�¼��ǰ���ڶ��ĸ�token���в���
	//**************************************************************��Ҫ��tokenlistҲ�ŵ�������private�У�

	TreeNode *root = NULL;//���ڱ�����ڵ��ָ��

	TreeNode* parse();//�����ܳ��������������﷨������

	TreeNode* Program();//�ܳ������������
	//**************************************************************DOTӦ����δ���

	TreeNode* ProgramHead();//����ͷ���ִ����������

	TreeNode* DeclarePart();//�����������ִ����������

	TreeNode* TypeDec();//�������������������

	//���������е���������
	TreeNode* TypeDeclaration();//6 ���ݲ���ʽ��ƥ�䱣����TYPE�����ú���TypeDecList
	TreeNode* TypeDecList();//7 ���ݲ���ʽ�������µ��������ͽڵ�t������TypeId()��ƥ�䱣����EQ�����ã�����TyoeDef(),ƥ�䱣����SEMI�����ú���TypeDecMore()��
	TreeNode* TypeDecMore();//8

	void TypeId(TreeNode *t);//9 �������������������������ƴ����������

	void TypeDef(TreeNode *t);//10 ���ʹ����������

	void BaseType(TreeNode *t);//11 �������ʹ����������

	void StructureType(TreeNode *t);//12 �ṹ���ʹ����������

	void ArrayType(TreeNode *t);//13 �������͵Ĵ����������

	void RecType(TreeNode *t);//14 ��¼���͵Ĵ����������

	TreeNode* FieldDecList();//15 ��¼�����е������������������

	TreeNode* FieldDecMore();//16 ��¼�����е�����������������������

	void IdList(TreeNode *t);//17 ��¼�������б�ʶ���������������

	void IdMore(TreeNode *t);//18 ��¼��������������ʶ���������������

	TreeNode* VarDec();//19 ������������ֳ���

	//�����������ִ������
	TreeNode* VarDeclaration();//20
	TreeNode* VarDecList();//21
	TreeNode* VarDecMore();//22

	//�����������ֱ��������ִ������
	void VarIdList(TreeNode *t);//23
	void VarIdMore(TreeNode *t);//24

	TreeNode* ProcDec();//25 �������������ܵĴ����������

	TreeNode* ProcDeclaration();//26 �����������־���Ĵ����������

	void ParamList(TreeNode *t);//27 ���������еĲ����������ֵĴ����������

	//���������еĲ��������������ֵĴ����������
	TreeNode* ParamDecList();//28 
	TreeNode* ParamMore();//29

	TreeNode* Param();//30 ���������еĲ��������в������ֵĴ����������

	//���������еĲ��������в��������ֵĴ����������
	void FormList(TreeNode *t);//31 
	void FidMore(TreeNode *t);//32

	TreeNode* ProcDecPart();//33 �������������ֵķ����������

	TreeNode* ProcBody();//34 �����岿�ִ����������

	TreeNode* ProgramBody();//35 �������岿�ִ����������

	TreeNode* StmList();//36 ������в��ִ����������

	TreeNode* StmMore();//37 ������䲿�ִ����������

	TreeNode* Stm();//38 ���ݹ鴦���������

	TreeNode* AssCall();//39 ��ֵ���ͺ���������䲿�ֵĴ����������

	TreeNode* AssignmentRest();//40 ��ֵ��䴦���������

	TreeNode* ConditionalStm();//41 ������䴦���������        ���д������⣬StmL����ûд

	TreeNode* LoopStm();//42 ѭ����䴦���������

	TreeNode* InputStm();//43 �������Ĵ����������

	TreeNode* OutputStm();//44 �����䴦���������

	TreeNode* ReturnStm();//45 ���̷�����䴦���������

	TreeNode* CallStmRest();//46 ���̵�����䴦���������

	TreeNode* ActParamList();//47 ʵ�β��ִ����������

	TreeNode* ActParamMore();//48 ����ʵ�β��ִ����������

	TreeNode* Exp();//49 ���ʽ�ݹ鴦���������

	TreeNode* Simple_exp();//50 �򵥱��ʽ�ݹ鴦���������

	TreeNode* term();//51 ��ݹ鴦���������

	TreeNode* factor();//52 ���ӵݹ鴦���������

	TreeNode* variable();//53 �����������

	void variMore(TreeNode *t);//54 �������ಿ�ִ������

	TreeNode* fieldvar();//55 ��������ִ������

	void fieldvarMore(TreeNode *t);//56 ������������ִ������

	void match(LexType expected);//57 �ռ���ƥ�䴦�����

	void DFS(TreeNode *t, int depth);//������ȱ������������ڵݹ�����﷨��

	string transform(TreeNodeType type);//�����Ľڵ�ö������ת��Ϊ�������string

	string transform_deckind_to_string(DecKind kind);//������deckindö������ת��Ϊ�������string

	string tranform_op_to_string(LexType op);//�����Ĳ������ڵ��opö������ת��Ϊ�������string
};



