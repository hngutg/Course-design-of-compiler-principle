#include "pch.h"

#include "semantic_analysis.h"

#include <iostream>

using namespace std;

//���ű�ʵ��

void Semantic::CreateTable() {
	level++;//scopeջ�Ĳ���+1
	SymbTable *newscope = new SymbTable;
	Scope.push_back(*newscope);//��Scope��ѹ��һ���µ�SymbTable��������ʾ�����µ�һ�㣬�����������next=null����ʾ��һ���л�Ϊ�գ�����ڵ��в�����¼��Ϣ��
	//************************************************************************�����⣡��������������������������������
	//���ƫ����������ʲô��   ʵ�������б�������Ϊ����ȷ��
	int off = 7;
	OffsetScope.push_back(off);//��һ��ĳ�ʼƫ����offΪ0��������initoffΪ7
	//************************************************************************
}

void Semantic::DestroyTable() {
	//����һ�����ű���Ϊ������ű����Ч����delete��һ���ȫ��SymbTable����pop��Scope
	SymbTable *del = Scope[level].next;
	Scope[level].next = NULL;
	while (del != NULL) {
		SymbTable *op = del;
		del = del->next;
		delete op;
	}
	level--;
	Scope.pop_back();
	OffsetScope.pop_back();
	//*****************************************************************************
	//��������������һ��keep��scopeջ���Ǹ�ʲô��?
}

bool Semantic::SearchoneTable(string id, int currentLevel, SymbTable **Entry) {
	//���ҵ���EntryΪ��ʶ���ڷ��ű��е�λ�ã����Ӧ�÷���һ��ָ��(��ַ)
	(*Entry) = Scope[currentLevel].next;
	while ((*Entry) != NULL) {
		if (id == (*Entry)->idname) {
			return true;
		}
		(*Entry) = (*Entry)->next;
	}
	return false;
}

bool Semantic::FindEntry(string id, Flag flag, SymbTable **Entry) {//�ӷ��ű��в��ұ�ʶ����һ����˵flag=down
	bool present = SearchoneTable(id, level, Entry);
	//**********************************************************����ڱ����ҵ��������������ҵ���������һ��entry����
	//������Ϊ�����ȷ��ر���
	if (present == false) {//�ڱ��㲢û���ҵ������ʶ��
		if (flag == up) {//����߲���в���
			int oplevel = level + 1;//��ǰ�����Ĳ�������Ӧ�øı�level
			while (oplevel < Scope.size()) {
				Entry = &(Scope[oplevel].next);//ÿ�β�����Entry��ʼ��Ϊoplevel�������
				bool temp = SearchoneTable(id, oplevel, Entry);
				if (temp == true) {
					present = true;
					break;//���ҵ��ˣ�ֱ�ӷ���
				}
				else {
					oplevel++;
				}
			}
		}
		else if (flag == down) {//����Ͳ���в���
			int oplevel = level - 1;
			while (oplevel >= 0) {
				(*Entry) = Scope[oplevel].next;//ÿ�β�����Entry��ʼ��Ϊoplevel�������
				bool temp = SearchoneTable(id, oplevel, Entry);
				if (temp == true) {
					present = true;
					break;
				}
				else {
					oplevel--;
				}
			}
		}
	}

	if (present == false) {
		Entry == NULL;
	}

	return present;
}

bool Semantic::Enter(string id, AttributeIR *AttribP, SymbTable **Entry) {   //�ǼǱ�ʶ�������Ե����ű�
	bool present = FindEntry(id, current, Entry);//flag��Ϊcurrent�����ı�ʶ�����岢����Ӱ�챾�㣬������ֱ�ʶ���ظ���������

	if (present == true) {
		cout << id << "��ʶ���ظ���������" << "\n";
	}
	else {
		SymbTable *temp = new SymbTable;//�½�һ�����ű���
		temp->idname = id;//���Ʊ�ʶ����
		//���Ʊ�ʶ������
		temp->attrIR.idtype = AttribP->idtype;
		temp->attrIR.kind = AttribP->kind;
		switch (AttribP->kind)
		{
		case type://��kind == typeKindʱ
			break;
		case var://��kind == varKindʱ
			temp->attrIR.More.VarAttr.level = AttribP->More.VarAttr.level;
			temp->attrIR.More.VarAttr.access = AttribP->More.VarAttr.access;
			temp->attrIR.More.VarAttr.off = AttribP->More.VarAttr.off;
			break;
		case proc://��kind == procKindʱ
			//code������Ŀ�����ʱ����д���о������ò���
			temp->attrIR.More.ProcAttr.level = AttribP->More.ProcAttr.level;
			temp->attrIR.More.ProcAttr.param = AttribP->More.ProcAttr.param;
			temp->attrIR.More.ProcAttr.size = AttribP->More.ProcAttr.size;
			//***********************************************************************************************
		default:
			break;
		}
		//���˷��ű������뵱ǰ���ű�β
		if (Scope[level].next == NULL) {
			Scope[level].next = temp;
		}
		else {
			SymbTable *pos = Scope[level].next;
			while (pos->next != NULL) {
				pos = pos->next;
			}
			pos->next = temp;
		}
		//��Entry��Ϊ�գ���Entryָ���ʶ���ڱ�ʶ�����е�λ��***********************************************
		*Entry = temp;
	}
	return present;
}

bool Semantic::FindField(string id, fieldChain *head, fieldChain **Entry) {//������в�������
	//headָ�����ͷ��idΪ���ҵı�ʶ���������ҳɹ�����entry��¼��ʶ��������е�λ��
	fieldChain *temp = head;
	while (temp != NULL) {
		if (temp->idname == id) {//�����ҳɹ�
			(*Entry) = temp;
			return true;
		}
		else {
			temp = temp->next;
		}
	}
	Entry = NULL;
	return false;
}

//SNL�������ʵ��

void Semantic::getResult(TreeNode *t) {
	Analyze(t);
}

void Semantic::Analyze(TreeNode *currentP) {//***************************************************

	SymbTable * Entry = NULL;
	TreeNode * p = NULL;
	TreeNode * pp = currentP;

	CreateTable();
	initialize();
	//ѭ���������������������
	//ֱ���������ڵ㿪ʼ
	//��һ���ڵ���ProgramHead��û����

	p = currentP->child[1];//����DeclarePart
	while (p != NULL) {
		if (p->nodekind == TypeK) {
			TypeDecPart(p->child[0]);
		}
		else if (p->nodekind == VarK) {
			VarDecList(p->child[0]);
		}
		else if (p->nodekind == ProcDecK) {
			ProcDecPart(p);
		}
		else {
			cout << "�﷨����û���������͵Ľڵ�" << endl;
		}
		p = p->sibling;
	}

	//if(Error) return;
	//ʵ�������У������ִ���ֱ�ӷ���

	//������������
	p = currentP->child[2];//����ProgramBody
	if (p->nodekind == StmLK) {
		Body(p);
	}

	PrintSymbTable();

	//�������ű�
	if (level != -1)
		DestroyTable();
}

void Semantic::initialize() {//***************************************************
	//��ʼ���������ͣ��ַ����ͣ��������͵��ڲ���ʾ��˵���������������;�Ϊ�������ͣ��ڲ���ʾ�̶�
	intPtr = NewTy(intTy);
	charPtr = NewTy(charTy);
	boolPtr = NewTy(boolTy);
}

TypeIR* Semantic::TypeProcess(TreeNode *t, DecKind deckind) {
	TypeIR *ptr = NULL;
	if (deckind == Idk) {
		ptr = nameType(t);
	}
	else if (deckind == ArrayK) {
		ptr = arrayType(t);
	}
	else if (deckind == RecordK) {
		ptr = recordType(t);
	}
	else if (deckind == IntegerK) {//ֱ�ӷ�������ָ��
		ptr = intPtr;
	}
	else if (deckind == CharK) {//ֱ�ӷ����ַ�ָ��
		ptr = charPtr;
	}
	return ptr;
}

TypeIR* Semantic::nameType(TreeNode *t) {//�ú��������ڷ��ű���Ѱ���Ѷ������������
	//***************************************************************************************************
	TypeIR* ptr = NULL;
	SymbTable *Entry = NULL;
	Entry = Scope[level].next;  // SearchoneTable���Ѿ������˳�ʼ�� û��Ҫ��
	bool present = FindEntry(t->type_name, down, &Entry);//�˴�flagӦΪdown������ڱ���û���ҵ���Ӧ����������ȥ��
	if (present == true) {
		if (Entry->attrIR.kind == type) {
			ptr = Entry->attrIR.idtype;
		}
		else {
			cout << "����" << t->type_name << "��δ����ʱ��ʹ��" << "\n";
			//�Ƿ�Ӧ�����ļ������������Ϣ?   ���纯��ErrorPrompt
		}
	}
	else {
		cout << "����δ����" << "\n";
		//�Ƿ�Ӧ�����ļ������������Ϣ?   ���纯��ErrorPrompt
	}
	return ptr;
}

TypeIR* Semantic::arrayType(TreeNode *t) {//���������ڲ���ʾ������

	TypeIR  * Ptr0 = NULL;//�����±����͵ķ���
	TypeIR  * Ptr1 = NULL;//����Ԫ�����͵ķ���
	TypeIR  * Ptr = NULL;

	if (t->array_high < t->array_low) {
		cout << "�����±�Խ�����" << "\n";
	}
	else {
		//***********************************************************************************
		//�кܴ�����
		Ptr0 = TypeProcess(t, IntegerK);//�������ͷ��������������±�����?
		Ptr1 = TypeProcess(t, t->childType);//�������ͷ�������������Ԫ������
		                             //����Ԫ�����͵��״������ʲô��   �����������д洢������
		//***********************************************************************************

		Ptr = NewTy(arrayTy);//�����µ�������Ϣ��
		Ptr->size = (t->array_high - t->array_low + 1)*(Ptr1->size);//���㱾���ͳ���

		//��д������Ϣ
		Ptr->More.ArrayAttr.elemTy = Ptr1;//������ǰ�����Ԫ������
		Ptr->More.ArrayAttr.indexTy = Ptr0;//������ǰ������±�����
		//***************************************************�Ƿ���Ҫ��¼��������½磿

	}
	return Ptr;
}

TypeIR* Semantic::recordType(TreeNode *t) {//�����¼���͵��ڲ���ʾ����

	TypeIR  * Ptr = NewTy(recordTy);//�½���¼���͵Ľڵ�
	TreeNode *opt = t;
	opt = opt->child[0];//��ȡ����Ϣ

	fieldChain  *  Ptr2 = NULL;
	fieldChain  *  Ptr1 = NULL;//����Ptr2�����һ������ָ��

	fieldChain  *  body = NULL;//����������������ɵ������������ͷfieldChain���൱������head��

	while (opt != NULL) {
		//ѭ������һ���ڵ��ж��id�����
		//�˴�ѭ���Ǵ���������int a,b;
		for (int i = 0; i < opt->idnum; i++) {
			Ptr2 = NewBody();//�����µ������͵�Ԫ�ṹPtr2
			if (body == NULL) {//��bodyΪ��ʱ�����⴦��
				body = Ptr2;
				Ptr1 = Ptr2;
			}
			//��дPtr2�ĸ�����Ա����
			Ptr2->idname = opt->name[i];
			Ptr2->unitType = TypeProcess(opt, opt->k.dec);
			Ptr2->next = NULL;

			//���ptr1��=ptr2,��ptr1ָ�봦��֮�����
			if (Ptr1 != Ptr2) {
				Ptr2->offset = Ptr1->offset + Ptr1->unitType->size;//����������Ľڵ��off
				//���ִ���ʽҲʹ�õ�һ���ڵ��offsetΪ0
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
		}
		//������ͬ���͵ı�����ȡ�﷨�����ֵܽڵ�
		opt = opt->sibling;
	}

	//ȡ����Ptr2��(offset+size)Ϊ���������¼��size
	Ptr->size = Ptr2->offset + Ptr2->unitType->size;
	//�����������¼���͵�body����
	Ptr->More.body = body;

	return Ptr;
}

//**********************�������������**********************

void Semantic::TypeDecPart(TreeNode *t) {//�����������ִ�����
	bool present = false;

	AttributeIR *AttribP = new AttributeIR;
	AttribP->kind = type;
	
	SymbTable * Entry = NULL;
	TreeNode *opt = t;

	while (opt != NULL) {
		//���ü�¼���Ժ����������Ƿ��ظ����������ڵ�ַ
		present = Enter(opt->name[0], AttribP, &Entry);
		//��û���ҵ�����ôenter�����������������entry�����ڷ��ű��е�λ��

		if (present == true) {
			cout << opt->name[0] << "�����ظ�����" << "\n";
			//���ļ������������Ϣ������
			//�����б���״̬��������   �Ƿ����
			Entry = NULL;
		}
		else {//δ����,û���ظ�����
			Entry->attrIR.idtype = TypeProcess(opt, opt->k.dec);
		}
		opt = opt->sibling;
	}
}

void Semantic::VarDecList(TreeNode *t) {//�����������ַ���������
	//��������������������
	AttributeIR *AttribP = new AttributeIR;
	bool present = false;
	SymbTable *Entry = NULL;

	TreeNode *opt = t;
	while (opt != NULL) {
		AttribP->kind = var;
		for (int i = 0; i < opt->name.size(); i++) {//ѭ������ͬһ���ڵ��id
			AttribP->idtype = TypeProcess(opt, opt->k.dec);
			if (opt->paramt == varparamtype) {//������Ϊ���
				AttribP->More.VarAttr.level = level;//��¼����
				AttribP->More.VarAttr.access = indir;//����Ϊ��ӱ���
				//*****************************************************************************************
				//���offset�ı䶯��  ������
				AttribP->More.VarAttr.off = OffsetScope[level];
				//����Ǳ�Σ�ƫ��+1
				OffsetScope[level] += 1;
			}
			else {//������Ϊʵ��
				AttribP->More.VarAttr.level = level;
				AttribP->More.VarAttr.access = dir;//����Ϊֱ�ӱ���
				//����ֵ�ε�ƫ��
				if (AttribP->idtype != NULL)
				{
					AttribP->More.VarAttr.off = OffsetScope[level];
					//cout << OffsetScope[level] << endl;
					OffsetScope[level] += AttribP->idtype->size;//ƫ�Ƽӱ������͵�size
				}
			}

			//�ǼǸñ��������Լ�����,�ж��Ƿ��ظ�����,�������������ڲ�ָ��
			present = Enter(opt->name[i], AttribP, &Entry);
			if (present == true) {//�ҵ��ˣ�˵���������ظ�����
				cout << opt->name[0] << "�������ظ�����" << "\n";
				//���ļ������������Ϣ������
			    //�����б���״̬��������   �Ƿ����
				return;
			}
			//*********************************************************************************************
			//opt->table�Ǹ�ʲô������������
		}
		opt = opt->sibling;
	}
	//*********************************************************************************************
	//�˴���ʾ������ͬ������������
}

void Semantic::ProcDecPart(TreeNode *t) {//�����������ַ���������**********************************************
	//��������������������
	//�ڵ�ǰ����ű�����д���̱�ʶ�������ԣ����²���ű�����д�βα�ʶ�������ԡ�
	//TreeNode *p = t;
	SymbTable *Entry = HeadProcess(t);//�������ͷ

	//*****************************
	if (Entry->attrIR.More.ProcAttr.param->entry == NULL) {
		cout << "null" << endl;
	}
	//*****************************
	
	TreeNode *opt = t->child[1];//��ʱ��ΪDeclarePart���ִ���
	//��������ڲ������������֣�������������
	//***************************************************************************
	//������Ϊ��Щ����������ȫ��!!!!!!
	while (opt != NULL) {
		switch (opt->nodekind)
		{
		case TypeK:
			TypeDecPart(opt->child[0]);
			break;
		case VarK:
			VarDecList(opt->child[0]);
			break;
		case ProcDecK:
			//*****************************************************************************
			//������������к���������������ѭ��������дnoff��moff����Ϣ
			//��д�ĺ����в�û��noff��moff���Ƿ�Ҫ�ģ�������
			ProcDecPart(opt->child[0]);
			break;
		default:
			cout << "ProcDecPart��������  û��������������" << "\n";
			break;
		}
		if (opt->nodekind == ProcDecK)
			break;
		else
			opt = opt->sibling;
	}
	//�����岿��
	Body(t->child[2]);
	//���ú�����������ǰscope
	//������ǰ���ű�֮ǰ�����
	PrintOneLayer(level);
	if (level != -1) {
		DestroyTable();
	}
}

SymbTable* Semantic::HeadProcess(TreeNode *t) {//��������ͷ��������
	//�ڵ�ǰ����ű�����д������ʶ�������ԣ����²���ű�����д�βα�ʶ�������ԡ�
	//���й��̵Ĵ�С�ʹ��붼���Ժ���
	AttributeIR *AttribP = new AttributeIR;
	bool present = false;
	SymbTable *Entry = NULL;

	//��д���̱�ʶ��������
	AttribP->kind = proc;
	AttribP->idtype = NULL;
	AttribP->More.ProcAttr.level = level + 1;//�����̵Ĳ���=��ǰ����+1

	if (t != NULL) {
		//�Ǽǹ��̵ķ��ű���
		present = Enter(t->name[0], AttribP, &Entry);
	}
	//�����βδ�����
	Entry->attrIR.More.ProcAttr.param = ParaDecList(t);

	//*******************************
	SymbTable *test = NULL;
	FindEntry(t->name[0], down, &test);
	if (test->attrIR.More.ProcAttr.param->entry == NULL) {
		cout << "null" << endl;
	}
	//*******************************
	return Entry;
}

ParamTable* Semantic::ParaDecList(TreeNode *t) {//�βη���������
	//���µķ��ű��еǼ������βεı�������βα���ĵ�ַ������paramָ��˵�ַ��
	TreeNode * p = NULL;
	ParamTable  * Ptr1 = NULL;
	ParamTable  * Ptr2 = NULL;
	ParamTable  * head = NULL;

	if (t != NULL) {
		if (t->child.size() != 0) {//��ǰ�﷨���ڵ�ĵ�һ�����Ӳ�Ϊnull
			p = t->child[0];
		}

		//�����µľֲ�����
		CreateTable();
		//�ӳ����еı�����ʼƫ����Ϊ8
		OffsetScope[level] = 7;    
		//���������������
		VarDecList(p);

		SymbTable * Ptr0 = Scope[level].next;

		while (Ptr0 != NULL) {
			//�����βη��ű���ʹ�����������ű��param��
			Ptr2 = new ParamTable;
			//����NewParamֻ����next��entry�ĳ�ʼ�������Ѿ���.h��ʵ����
			if (head == NULL) {
				Ptr1 = Ptr2;
				head = Ptr2;
			}

			//Ptr2->entry->idname = Ptr0->idname;
			Ptr2->entry = new SymbTable;
			Ptr2->entry->attrIR = Ptr0->attrIR;

			Ptr2->next = NULL;
			//****

			if (Ptr2 != Ptr1)//��������
			{
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
			Ptr0 = Ptr0->next;
		}
	}
	return head;//�����βη��ű��ͷָ��
}

void Semantic::Body(TreeNode *t) {//ִ���岿�ַ���������
	//SNL����ϵͳ��ִ���岿�ּ�Ϊ�������,��ֻ�账��������в��֡�
	if (t->nodekind == StmLK) {
		TreeNode * p = t->child[0];
		while (p != NULL) {//ѭ�������������
			statement(p);//�������״̬������
			p = p->sibling;
		}
	}
}

void Semantic::statement(TreeNode *t) {//������з���������
	//�����﷨���ڵ��е�kind���ж�Ӧ��ת�����ĸ�������ͺ���
	if (t->k.stmt == IfK) {
		ifstatement(t);
	}
	else if (t->k.stmt == WhileK) {
		whilestatement(t);
	}
	else if (t->k.stmt == AssignK) {
		assignstatement(t);
	}
	else if (t->k.stmt == ReadK) {
		readstatement(t);
	}
	else if (t->k.stmt == WriteK) {
		writestatement(t);
	}
	else if (t->k.stmt == CallK) {
		callstatement(t);
	}
	else if (t->k.stmt == ReturnK) {
		returnstatement(t);
	}
	else {
		cout << "statement��������ͳ���\n";
	}
}

TypeIR* Semantic::Expr(TreeNode *t, AccessKind *Ekind) {//���ʽ����������
	//���ʽ����������ص��Ǽ��������������������ԣ�����ʽ�����͡����в���Ekind������ʾʵ���Ǳ�λ���ֵ�Ρ�
	int present = false;
	SymbTable *Entry = NULL;
	
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;

	if (t != NULL) {
		if (t->k.EXP == ConstK) {//��Ϊ����
			Eptr = TypeProcess(t, IntegerK);
			Eptr->kind = intTy;
			if (Ekind != NULL) {
				*Ekind = dir;//ֱ�ӱ���
			}
		}
		else if (t->k.EXP == IdEK) {//��Ϊ����
			//Var = id������
			if (t->vark == IdV) {
				//�ڷ��ű��в��Ҵ˱�ʶ��
				present = FindEntry(t->name[0], down, &Entry);
				//�ڳ�����,:=����Ҳ���ŵ���child�У���ˣ����if���жϱ�Ϊ������vark����id��Ϊ��t->child[0]->name[0]

				if (present == true) {//�ҵ���
					if (Entry->attrIR.kind != var) {//��id��Ϊ����
						cout << "Expr�г���" << t->name[0] << "���Ǳ���" << "\n";
						//���ļ������������Ϣ������
				        //�����б���״̬��������   �Ƿ����
					}
					else {
						Eptr = Entry->attrIR.idtype;
						if (Ekind != NULL) {
							*Ekind = indir;//��ӱ���
						}
					}
				}
				else {
					//��ʶ��������
					cout << "Expr�г���" << t->name[0] << "û������" << "\n";
					//���ļ������������Ϣ������
					//�����б���״̬��������   �Ƿ����
				}
			}
			//Var = Var0[E]������
			else if (t->vark == ArrayMembV) {
				Eptr = arrayVar(t);
			}
			//Var = Var0.id������
			else if (t->vark == FieldMembV) {
				Eptr = recordVar(t);
			}
			else {
				cout << "Expr�г��ִ���" << "\n";
			}
		}
		else if (t->k.EXP == OpK) {//��Ϊ������
			//�ݹ���ö��ӽڵ�
			Eptr0 = Expr(t->child[0], NULL);
			if (Eptr0 == NULL) {
				return NULL;
			}
			Eptr1 = Expr(t->child[1], NULL);
			if (Eptr1 == NULL) {
				return NULL;
			}

			//�����б�
			if (Eptr0 != Eptr1) {
				present = false;//���Ͳ����
			}
			else {
				present = true;//�������
			}

			if (present != false) {//��������ͬ
				switch (t->op)
				{
					//LT��EQ��Ϊbool���ʽ
				case LT:
				case EQ:
					Eptr = boolPtr;
					break;  /*�������ʽ*/
					//�������ֱ��ʽ�õ��ľ���intptr
				case PLUS:
				case MINUS:
				case TIMES:
				case OVER:
					Eptr = intPtr;
					break;  /*�������ʽ*/
				}
				if (Ekind != NULL) {
					(*Ekind) = dir;//ֱ�ӱ���
				}
			}
			else {
				cout << "��������಻����\n";
				//���ļ������������Ϣ������
				//�����б���״̬��������   �Ƿ����
			}
		}
	}
	return Eptr;
}

TypeIR* Semantic::arrayVar(TreeNode *t) {//��������Ĵ����������
	//�ú�����������������±����
	//���var := var0[E]��var0�ǲ����������ͱ�����E�ǲ��Ǻ�������±��������ƥ��.
	bool present = false;
	SymbTable *	Entry = NULL;

	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;

	//�ڷ��ű��в��Ҵ˱�ʶ��
	present = FindEntry(t->name[0], down, &Entry);

	if (present == true) {
		if (Entry->attrIR.kind != var) {//Var0���Ǳ���
			cout << "arrayVar�У�" << t->name[0] << "���Ǳ���\n";
			Eptr = NULL;
			//���ļ������������Ϣ������
		    //�����б���״̬��������   �Ƿ����
		}
		else {
			if (Entry->attrIR.idtype != NULL) {//���մ�ʩ����ֹ��null����
				if (Entry->attrIR.idtype->kind != arrayTy) {//Var0�����������ͱ���
					cout << "arrayVar�У�" << t->name[0] << "�����������\n";
					Eptr = NULL;
					//���ļ������������Ϣ������
			        //�����б���״̬��������   �Ƿ����
				}
				else {
					//���E�������Ƿ����±��������
					Eptr0 = Entry->attrIR.idtype->More.ArrayAttr.indexTy;//�õ��趨���±������
					if (Eptr0 == NULL) {
						cout<<"arrayVar��," << t->name[0] << "�±�û���趨������?\n";
						return NULL;
					}
					Eptr1 = Expr(t->child[0], NULL);//�õ�ʵ�ʵ��±������
					if (Eptr1 == NULL) {
						cout << "arrayVar��," << t->name[0] << "ʵ�ʵ��±��ӽڵ�û������?\n";
						return NULL;
					}
					//�Ƚ����������Ƿ���ͬ
					if (Eptr0 == Eptr1) {
						present == true;
					}
					else {
						present = false;
					}

					if (present != true) {
						cout << "arrayVar��," << t->name[0] << "������±겢��ƥ��\n";
						Eptr = NULL;
						//���ļ������������Ϣ������
					    //�����б���״̬��������   �Ƿ����
					}
					else {
						Eptr = Entry->attrIR.idtype->More.ArrayAttr.elemTy;//ָ������Ԫ�������ڲ���ʾ��ָ��
					}
				}
			}
		}
	}
	else {//���ű���û�ҵ�����������
		cout << "arrayVar�У�" << t->name[0] << "û������\n";
		//���ļ������������Ϣ������
		//�����б���״̬��������   �Ƿ����
	}
	return Eptr;
}

TypeIR* Semantic::recordVar(TreeNode *t) {//��¼������������ķ���������
	//���var:=var0.id�е�var0�ǲ��Ǽ�¼���ͱ�����id�ǲ��Ǹü�¼�����е����Ա��
	bool present = false;
	SymbTable *	Entry = NULL;
	bool result = true;

	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	fieldChain *currentP = NULL;

	//�ڷ��ű��в��Ҵ˱�ʶ��
	present = FindEntry(t->name[0], down, &Entry);
	//����ҵ���
	if (present == true) {
		//Var0���Ǳ���
		if (Entry->attrIR.kind != var) {
			cout << "recordVar�У�" << t->name[0] << "���Ǳ���\n";
			Eptr = NULL;
			//���ļ������������Ϣ������
			//�����б���״̬��������   �Ƿ����
		}
		else {//����Ҫ��Ҫ���մ�ʩ������
			
			//Var0���Ǽ�¼���ͱ���
			if (Entry->attrIR.idtype->kind != recordTy) {
				cout << "recordVar�У�" << t->name[0] << "���Ǽ�¼���ͱ���\n";
				Eptr = NULL;
				//���ļ������������Ϣ������
			    //�����б���״̬��������   �Ƿ����
			}
			//���id�Ƿ��ǺϷ�����
			else {
				Eptr0 = Entry->attrIR.idtype;//�õ���ʶ���������ڲ���ʾ
				currentP = Eptr0->More.body;//�õ�������ڲ��ĵ�һ����Ԫ
				while ((currentP != NULL) && (result == true)) {
					//��������ڲ�ȥѰ��id��id���洢��t->child[0]->name[0]�����Ǻ����岿�֣�����������child����Ǵ����id��
					//ͨ��currentP��������������ڲ�
					if (t->child[0]->name[0] == currentP->idname) {//�ҵ���
						result = false;
						Eptr = currentP->unitType;
					}
					else {//����
						result = true;
						currentP = currentP->next;
					}
				}
				//���û���ҵ�
				if (currentP == NULL) {
					cout << "recordVar�У�" << t->child[0]->name[0] << "��������\n";
					Eptr = NULL;
				}
				else {
					//���id��һ���������
					if (t->child[0]->child.size() > 0) {
						Eptr = arrayVar(t->child[0]);
					}
				}
			}
		}
	}
	//���򣬱�ʶ��������
	else {
		cout << "recordVar��" << t->name[0] << "û������\n";
	}
	return Eptr;//����ָ���¼Ԫ�������ڲ���ʾ��ָ��
}

void Semantic::assignstatement(TreeNode *t) {//��ֵ����������
	//��ֵ��������������ص��Ǽ�鸳ֵ�����˷��������������ԡ�
	SymbTable *Entry = NULL;

	int present = false;
	TypeIR *Ptr = NULL;
	TypeIR *Eptr = NULL;

	TreeNode *child1 = t->child[0];
	TreeNode *child2 = t->child[1];

	if (child1->child.size() == 0) {//���Ⱥ���ߵ�Ϊ���������������������
		//�ڷ��ű��в��Ҵ˱�ʶ��
		present = FindEntry(child1->name[0], down, &Entry);

		if (present == true) {
			//���Ⱥ����Ĳ��Ǳ���
			if (Entry->attrIR.kind != var) {
				Eptr = NULL;
				cout << "assignstatement�У��Ⱥ�����" << child1->name[0] << "���Ǳ���" << endl;
			}
			else {
				Eptr = Entry->attrIR.idtype;
			}
		}
		//���򣬱�ʶ��������
		else {
			cout << "assignstatement�У��Ⱥ�����" << child1->name[0] << "û������" << endl;
		}
	}
	else {
		//���Ⱥ�����Ϊ���飬��Var0[E]
		if (child1->vark == ArrayMembV) {
			Eptr = arrayVar(child1);
		}
		//���Ⱥ����Ϊ���������Var0.id
		else if (child1->vark == FieldMembV) {
			Eptr = recordVar(child1);
		}
		else {
			cout << "����Ⱥ���ߵ����Ǹ�ɶ�أ�" << endl;
		}
	}
	if (Eptr != NULL) {
		if ((t->nodekind == StmtK) && (t->k.stmt == AssignK)) {//���գ�������ڵ���һ����ֵ���
			//����ǲ��Ǹ�ֵ������ ���͵ȼ�
			Ptr = Expr(child2, NULL);
			if (Ptr != Eptr) {
				cout << "assignstatement�У���ֵ�������������Ͳ�ƥ��" << endl;
			}
		}
		//��ֵ����в��ܳ��ֺ�������
	}
}

void Semantic::callstatement(TreeNode *t) {//���̵���������������
	//����������������������ȼ����ű�����������е�Param���֣��βη��ű����ַ����
	//����������βκ�ʵ��֮��Ķ�Ӧ��ϵ�Ƿ���ȷ��
	AccessKind *Ekind = new AccessKind;
	bool present = false;
	SymbTable *Entry = NULL;
	TreeNode *p = NULL;

	//��id����������ű�
	present = FindEntry(t->child[0]->name[0], down, &Entry);

	//*********************************
	if (Entry->attrIR.More.ProcAttr.param->entry == NULL) {
		cout << "null" << endl;
	}
	//*********************************

	//��δ�ҵ�������������
	if (present == false) {
		cout << "callstatement�У�" << t->child[0]->name[0] << "����δ����" << endl;
	}
	else {
		//��id���Ǻ�����
		if (Entry->attrIR.kind != proc) {
			cout << "callstatement�У�" << t->child[0]->name[0] << "���Ǻ�����" << endl;
		}
		else {
			//�β�ʵ��ƥ��
			p = t->child[1];
			//paramPָ���βη��ű�ı�ͷ
			ParamTable *paramP = Entry->attrIR.More.ProcAttr.param;
			while (p != NULL && paramP != NULL) {
				SymbTable *paraEntry = paramP->entry;
				TypeIR *Etp = Expr(p, Ekind);//ʵ��
				//�������Ͳ�ƥ��
				if (paraEntry->attrIR.More.VarAttr.access == indir && *Ekind == dir) {
					cout << "callstatement�У�" << "�������Ͳ�ƥ��" << endl;
				}
				else {
					//�������Ͳ�ƥ��
					if (paraEntry->attrIR.idtype != Etp) {
						cout << "callstatement�У�" << "�������Ͳ�ƥ��" << endl;
					}
				}
				p = p->sibling;
				paramP = paramP->next;
			}
			//����������ƥ��
			if (p != NULL || paramP != NULL) {
				cout << "callstatement�У�" << "����������ƥ��" << endl;
			}
		}
	}
}

void Semantic::ifstatement(TreeNode *t) {//����������������
	//�����﷨�����������ӽڵ�
	//child[0]��if�е��ж�ʽ
	//child[1]��then�е����
	//child[2]��else�е����    else��һ������
	AccessKind * Ekind = NULL;
	TypeIR *Ept = Expr(t->child[0], Ekind);
	if (Ept != NULL) {
		//�����������ʽ
		if (Ept->kind != boolTy) {
			cout << "ifstatement�У��߼����ʽ����" << endl;
		}
		else {
			TreeNode *p = t->child[1];
			while (p != NULL) {
				statement(p);
				p = p->sibling;
			}
			if (t->child.size() == 3) {//���else�ڵ����
				TreeNode *p = t->child[2];
				while (p != NULL) {
					statement(p);
					p = p->sibling;
				}
			}
		}
	}
}

void Semantic::whilestatement(TreeNode *t) {//ѭ��������������
	TypeIR *  Etp = Expr(t->child[0], NULL);
	if (Etp != NULL) {//�����������ʽ����		
		//����������ʽ����bool����
		if (Etp->kind != boolTy) {
			cout << "whilestatement�У�WHILE�����������ʽ����" << endl;
		}
		else {
			TreeNode *p = t;
			//����ѭ�����ڵ����
			p = p->child[1];
			while (p != NULL) {
				statement(p);
				p = p->sibling;
			}
		}
	}
}

void Semantic::readstatement(TreeNode *t) {
	SymbTable * Entry = NULL;
	int present = false;

	//��id����������ű�
	present = FindEntry(t->name[0], down, &Entry);

	//���û�ҵ���˵������û������
	if (present = false) {
		cout << "readstatement�����У�" << t->name[0] << "û������" << endl;
	}
	else {
		//���t->name[0]���Ǳ�����ʶ��
		if (Entry->attrIR.kind != var) {
			cout << "readstatement�����У�" << t->name[0] << "���Ǳ�����ʶ��" << endl;
		}
	}
}

void Semantic::writestatement(TreeNode *t) {//д������������
	//�����������еı��ʽ�Ƿ�Ϸ�
	TypeIR * Etp = Expr(t->child[0], NULL);

	if (Etp != NULL) {
		//������ʽ����Ϊbool���ͣ�����
		if (Etp->kind == boolTy) {
			cout << "writestatement�����У�" << t->name[0] << "������ʹ���" << endl;
		}
	}
}

void Semantic::returnstatement(TreeNode *t) {//�ú�������������������
	//����������������Ƿ����������г���
	if (level == 0)
		/*����������������������У�����*/
		cout << "returnstatement�����У�return������" << endl;
}

//*****************���ຯ��******************

TypeIR* Semantic::NewTy(Typekind kind) {//������ǰ�������ڲ���ʾ
	//����Ϊ���ͣ��������ظ����͵��ڲ���ʾ�ĵ�ַ
	TypeIR *table = new TypeIR;
	if (table == NULL) {
		cout << "out of memory" << "\n";
	}
	else {
		//�����ڲ���ʾ����ָ��table����NULL,�ڴ浥Ԫ�Ѿ��ɹ�����
		switch (kind) {
		case intTy:
			table->kind = kind;
			table->size = 1;
			break;
		case charTy:
			table->kind = kind;
			table->size = 1;
			break;
		case boolTy:
			table->kind = kind;
			table->size = 1;
			break;
		case arrayTy:
			table->kind = kind;
			table->More.ArrayAttr.elemTy = NULL;//ָ������Ԫ�����͵��ڲ���ʾ  ��ʼ��
			table->More.ArrayAttr.indexTy = NULL;//ָ�������±����͵��ڲ���ʾ ��ʼ��
			table->size = 0;//�����С��Ҫ�������ȷ��
			break;
		case recordTy:
			table->kind = kind;
			table->More.body = NULL;//��¼�����е����� ��ʼ��
			table->size = 0;//��¼���͵Ĵ�С��Ҫ����ȷ��
			break;
		}
		
	}
	return table;
}

fieldChain* Semantic::NewBody() {//������ǰ�ռ�¼�������������
	//�������ظ����͵��µ�����ĵ�Ԫ��ַ

	fieldChain *ptr = new fieldChain;

	//�Ե�ǰ�µ�����Ԫ���г�ʼ��
	ptr->next = NULL;
	ptr->unitType = NULL;
	ptr->offset = 0;

	return ptr;
}

//*****************�������******************
/*
void YuYi::PrintFieldChain(fieldChain  *currentP)
{
	fprintf(listing, "\n--------------Field  chain--------------------\n");
	fieldChain  *t = currentP;
	while (t != NULL)
	{
		fprintf(listing, "%s:  ", t->id);


		switch (t->UnitType->kind)
		{
		case  intTy:  fprintf(listing, "intTy     ");   break;
		case  charTy:	fprintf(listing, "charTy    ");  break;
		case  arrayTy: fprintf(listing, "arrayTy   "); break;
		case  recordTy:fprintf(listing, "recordTy  "); break;
		default: fprintf(listing, "error  type!  "); break;
		}
		fprintf(listing, "off = %d\n", t->off);

		t = t->Next;
	}
}
*/
void  Semantic::PrintOneLayer(int level)
{
	SymbTable  *t = Scope[level].next;
	cout << "\n-------SymbTable  in level " << level << "---------" << endl;
	//fprintf(listing, "\n-------SymbTable  in level %d ---------\n", level);

	while (t != NULL)
	{ /*�����ʶ������*/
		cout << t->idname<<" ";
		//fprintf(listing, "%s:   ", t->idName);
		AttributeIR  *Attrib = &(t->attrIR);
		/*�����ʶ����������Ϣ�����̱�ʶ������*/
		if (Attrib->idtype != NULL)  /*���̱�ʶ��*/
			switch (Attrib->idtype->kind)
			{
			case  intTy:  
				cout << "intTy  ";
				//fprintf(listing, "intTy  ");   
				break;
			case  charTy:	
				cout << "charTy  ";
				//fprintf(listing, "charTy  ");  
				break;
			case  arrayTy: 
				cout << "arrayTy  ";
				//fprintf(listing, "arrayTy  "); 
				break;
			case  recordTy:
				cout << "recordTy  ";
				//fprintf(listing, "recordTy  "); 
				break;
			default: 
				cout << "error  type!  ";
				//fprintf(listing, "error  type!  "); 
				break;
			}
		/*�����ʶ������𣬲����ݲ�ͬ���������ͬ��������*/
		switch (Attrib->kind)
		{
		case  type:
			cout << "typekind  ";
			//fprintf(listing, "typekind  "); 
			break;
		case  var:
			cout << "varkind  Level = " << Attrib->More.VarAttr.level << "  Offset= " << Attrib->More.VarAttr.off << "  ";
			//fprintf(listing, "varkind  ");
			//fprintf(listing, "Level = %d  ", Attrib->More.VarAttr.level);
			//fprintf(listing, "Offset= %d  ", Attrib->More.VarAttr.off);

			switch (Attrib->More.VarAttr.access)
			{
			case  dir:  
				cout << "dir  ";
				//fprintf(listing, "dir  "); 
				break;
			case  indir:
				cout << "indir  ";
				//fprintf(listing, "indir  "); 
				break;
			default:
				cout << "errorkind  ";
				//fprintf(listing, "errorkind  ");  
				break;
			}
			break;
		case  proc:
			cout << "funckind   Level= " << Attrib->More.ProcAttr.level;
			//�Ƿ���Ҫ����һ��noff���洢sp��display���ƫ������
			//cout << " Noff= " << Attrib->More.ProcAttr.nOff << " ";

			//fprintf(listing, "funckind   ");
			//fprintf(listing, "Level= %d  ", Attrib->More.ProcAttr.level);
			//fprintf(listing, "Noff= %d  ", Attrib->More.ProcAttr.nOff);
			break;
		default:
			cout << "error   ";
			//fprintf(listing, "error  ");
		}
		cout << "\n";
		//fprintf(listing, "\n");
		t = t->next;
	}

	cout << "\n----------------------------------------------------" << endl;
}

void   Semantic::PrintSymbTable()
{ /*������0��ʼ*/
	int  l = 0;
	while (l<Scope.size())
	{
		PrintOneLayer(l);
		l++;
	}
}

