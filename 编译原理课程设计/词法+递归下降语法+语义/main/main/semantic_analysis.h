#pragma once
#include "pch.h"

#include "recursive-descent.h"

#include <string>

using namespace std;

//***************************************ָ�룡���������Ժ󿴿��ǲ�����Ҫ��ǰ��ʼ��Ϊnull

//�����ʶ��������
typedef enum {
	type,//���ͱ�ʶ�� typeKind
	var,//������ʶ��  varKind
	proc//���̱�ʶ��  procKind
}IdKind;

//���������ֱ�ӻ��Ǽ�ӱ���
typedef enum {
	dir,//ֱ�ӱ���
	indir//��ӱ���
}AccessKind;

//����SNL�е����͵�����
typedef enum {
	//��׼���ͣ�    ���ڲ���ʾ����ʵ�ֹ���
	intTy,//��������
	charTy,//�ַ�����
	//**********************************
	//����ͼ�¼���͵ȹ�������Ҫ�ڱ�����������������ʱ����
	arrayTy,//��������
	recordTy,//��¼����
	//**********************************
	boolTy//��������
}Typekind;

//���ں���FindEntry�У���ʶ�Ƿ���Ҫ�ڸ���һ����߸���һ����ű��н��б�ʶ������
typedef enum {
	up,       //��Ҫ�ڸ���һ����в���
	current,  //ֻ��Ҫ�ڱ�����в���
	down      //��Ҫ�ڸ���һ����в���
}Flag;

struct TypeIR;
struct ParamTable;
struct fieldChain;
struct AttributeIR;
struct SymbTable;

//�����ڲ��ṹ
struct TypeIR {
	int size;//������ռ�ռ��С
	Typekind kind;//��������
	union
	{
		struct {//������Ϊ��������ʱ���Ը�����Ϣ�ļ�¼
			TypeIR *indexTy;//������Ϊ��������ʱ��Ч��ָ�������±�����
			TypeIR *elemTy;//������Ϊ��������ʱ��Ч��ָ������Ԫ������
		}ArrayAttr;
		fieldChain *body;//��¼�����е�����
	}More;
};

//�����β���Ϣ���һ��
struct ParamTable {
	ParamTable *next = NULL;//ָ����һ���β�
	SymbTable *entry = NULL;//ָ���βα�ʶ���ڷ��ű��е�λ��
};

//���������͵�Ԫ�ṹ
struct fieldChain {
	string idname;//��¼���еı�ʶ��
	TypeIR *unitType;//ָ�����г�Ա���ڲ���ʾ        ************��������?  �����ǳ�Ա���ڲ���ʾ��������
	int offset;//���ڼ�¼�е�ƫ��
	fieldChain *next = NULL;//����ָ��
};

//�����ʶ����Ϣ��
struct AttributeIR {
	TypeIR *idtype = NULL;//ָ�룬ָ���ʶ���������ڲ���ʾ
	IdKind kind;//��ʶ��������
	union //���治ͬ���͵ı�ʶ��������   ��ʶ���Ĳ�ͬ�����в�ͬ������
	{
		//������ʶ�������ԣ���kind==varKindʱ��Ч
		struct {
			AccessKind access;//ֱ�ӱ������Ǽ�ӱ���
			int level;//��ʾ�ñ�����������������/���̵Ĳ���
			int off;//��ʾ�ñ������������������/���̵�ƫ����
		}VarAttr;

		//���̱�ʶ��������,��kind==procKindʱ��Ч
		struct {
			int level;//��ʾ���̲���
			ParamTable *param;//�β���Ϣ���ַ
			int code;//��ʾ���̵�Ŀ������ַ    ��������ڴ������ɽ׶���д
			int size;//��ʾ��������Ŀռ��С
		}ProcAttr;
	}More;
};

//����SNL���ű�
struct SymbTable {
	string idname;//��¼��ʶ��������
	AttributeIR attrIR;//��ʶ����Ϣ��  ��¼��ʶ���ĸ�����Ϣ
	SymbTable *next = NULL;//ָ����ű����һ�
};


class Semantic {
private:
	//�ֲ㽨�����ű�ʹ���ֳ���ķ��ű���������������һ��
	vector<SymbTable> Scope;//�ֳ����������¼����ֳ�����ű����ʼλ��

	vector<int> OffsetScope;//�����vectorȥ����ÿһ���ֽ׶ε�ƫ����

	int level = -1;//��ʾ��ǰ���ű�Ĳ���,��δ����ʱΪ-1

	TypeIR * intPtr;		//��ָ��һֱָ���������͵��ڲ���ʾ
	TypeIR * charPtr;		//��ָ��һֱָ���ַ����͵��ڲ���ʾ
	TypeIR * boolPtr;		//��ָ��һֱָ�򲼶����͵��ڲ���ʾ

	//***********************����Ϊ����***********************

	void Analyze(TreeNode *currentP);//1 �������������

	void initialize();//2 ��ʼ�����������ڲ���ʾ����

	TypeIR* TypeProcess(TreeNode *t, DecKind deckind);//3 ���ͷ���������

	TypeIR* nameType(TreeNode *t);//4 �Զ��������ڲ��ṹ��������

	TypeIR* arrayType(TreeNode *t);//5 ���������ڲ���ʾ������

	TypeIR* recordType(TreeNode *t);//6 �����¼���͵��ڲ���ʾ����

	void TypeDecPart(TreeNode *t);//7 �����������ַ���������

	void VarDecList(TreeNode *t);//8 �����������ַ���������

	void ProcDecPart(TreeNode *t);//9 �����������ַ���������

	SymbTable* HeadProcess(TreeNode *t);//10 ��������ͷ��������

	ParamTable* ParaDecList(TreeNode *t);//11 �βη�������

	void Body(TreeNode *t);//12 ִ���岿�ַ���������

	void statement(TreeNode *t);//13 ������������

	TypeIR* Expr(TreeNode *t, AccessKind *Ekind);//14 ���ʽ����������

	TypeIR* arrayVar(TreeNode *t);//15 ��������Ĵ����������

	TypeIR* recordVar(TreeNode *t);//16 ��¼������������ķ���������

	void assignstatement(TreeNode *t);//17 ��ֵ����������

	void callstatement(TreeNode *t);//18 ���̵���������������

	void ifstatement(TreeNode *t);//19 ����������������

	void whilestatement(TreeNode *t);//20 ѭ��������������

	void readstatement(TreeNode *t);//21 ��������������

	void writestatement(TreeNode *t);//22 д������������

	void returnstatement(TreeNode *t);//23 ����������������   ����û�У�����

	void CreateTable();//����һ�����ű�

	void DestroyTable();//����һ�����ű�

	bool SearchoneTable(string id, int currentLevel, SymbTable **Entry);//�ӱ�ͷ��ʼ�����ν��ڵ��еı�ʶ�����ֺ�id�Ƚ��Ƿ���ͬ��ֱ���ҵ���ʶ���򵽴��β

	bool FindEntry(string id, Flag flag, SymbTable **Entry);//�ڷ��ű��в��ұ�ʶ��

	void PrintSymbTable();//�ӵ�һ�㿪ʼ�����δ�ӡ������ű�����

	void PrintOneLayer(int level);//��ӡ��ǰ��ķ��ű�����

	bool Enter(string id, AttributeIR *AttribP, SymbTable **Entry);//�ǼǱ�ʶ�������Ե����ű�

	bool FindField(string id, fieldChain *head, fieldChain **Entry);//������в�������

	TypeIR * NewTy(Typekind  kind);//������ǰ�������ڲ���ʾ

	fieldChain* NewBody();//������ǰ�ռ�¼�������������
public:
	void getResult(TreeNode *root);//
};
