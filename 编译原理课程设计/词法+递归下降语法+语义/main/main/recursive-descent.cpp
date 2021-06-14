#include "pch.h"
#include "recursive-descent.h"
#include <iostream>

using namespace std;

void Parse::getResult(token s[]) {
	this->s = s;//����tokenlist
	root = parse();
	//parse���������﷨�������ظ��ڵ㵽root
	//��ȥ�����������
}

TreeNode* Parse::parse() {
	TreeNode* root;
	//����һ������???
	root = Program();//�����ܳ������������
	if (s[opposition].word.tok == ENDFILE) {
		return root;
	}
	else {//�ļ���ǰ������Ӧ���д�����
		return NULL;//????
	}
}

TreeNode* Parse::Program() {
	TreeNode *programhead, *declarepart, *programbody;
	programhead = ProgramHead();
	declarepart = DeclarePart();
	programbody = ProgramBody();
	TreeNode *t = new TreeNode;
	t->nodekind = ProK;//���ڵ���Ϊ����־�ڵ�
	//Ӧ�ж����﷨��root�Ƿ񴴽��ɹ����˴�δ�����ж�
	if (programhead == NULL) {//��programheadΪ�գ�Ӧ���б���
		cout << "ProgramHeadΪ��" << "\n";
		return NULL;
	}
	else {
		t->child.push_back(programhead);
	}
	if (declarepart == NULL) {//��declarepartΪ�գ�Ӧ���б���
		cout << "DeclarePartΪ��" << "\n";
		return NULL;
	}
	else {
		t->child.push_back(declarepart);
	}
	if (programbody == NULL) {//��programbodyΪ�գ�Ӧ���б���
		cout << "programbodyΪ��" << "\n";
		return NULL;
	}
	else {
		t->child.push_back(programbody);
	}
	//match(DOT);//��ǰ���ʺ�DOT����ƥ��
	opposition++;
	return t;
}

TreeNode* Parse::ProgramHead() {
	TreeNode *t = new TreeNode;
	if (s[opposition].word.tok == PROGRAM) {//ƥ�䱣����PROGRAM
		opposition++;
		if (s[opposition].word.tok == ID) {//����ǰ����ΪID
			t->nodekind = PheadK;
			t->idnum++;
			t->name.push_back(s[opposition].word.str);
			opposition++;
			return t;
		}
		else {
			cout << "������δ�ҵ�" << "\n";
			return NULL;
		}

	}
	else {//***************************************************δƥ��ɹ���զ�죿����
		cout << "PROGRAMƥ��ʧ��\n";
		return NULL;
	}
}

TreeNode* Parse::DeclarePart() {//*****************************************�������⣿��������
	TreeNode *tp1 = TypeDec();
	TreeNode *typed = NULL;
	if (tp1 != NULL) {//��ȥ���Ƿ�������������������ڡ���ȥ����һ������������־�﷨���ڵ�
		typed = new TreeNode;
		typed->nodekind = TypeK;//��������������־�����﷨���ڵ�
		typed->child.push_back(tp1);
	}
	
	TreeNode *tp2 = VarDec();
	TreeNode *vard = NULL;
	if (tp2 != NULL) {//��ȥ���Ƿ���ڱ��������������ڡ���ȥ����һ������������־�﷨���ڵ�
		vard = new TreeNode;
		vard->nodekind = VarK;//��������������־�����﷨���ڵ�
		vard->child.push_back(tp2);
	}
	
	TreeNode *s = ProcDec();
	//���Ϸֱ�����������������������������������ִ����������

	if (typed != NULL) {
		if (vard != NULL) {
			typed->sibling = vard;
			vard->sibling = s;
			return typed;
		}
		else {
			typed->sibling = s;
			return typed;
		}
	}
	else {
		if (vard != NULL) {
			vard->sibling = s;
			return vard;
		}
		else {
			return s;
		}
	}
}

TreeNode* Parse::TypeDec() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == TYPE) {//����һλΪTYPE���ȴ���������������ô�����﷨���ڵ㣬���򷵻�null
		t = TypeDeclaration();
		//opposition++;   �˴�ֻ�ǽ����жϣ��Ƿ������������������Ҫȡ��һ��token
		return t;
	}
	else {
		if (s[opposition].word.tok == VAR || s[opposition].word.tok == PROCEDURE || s[opposition].word.tok == BEGIN) {//�����������Ͷ��壬����null
			return t;
		}
		else {//���ִ���
			cout << "�����������ִ���" << "\n";
			opposition++;//�����˴��󵥴ʣ�������һ��token
			return NULL;
		}
	}
}

TreeNode* Parse::TypeDeclaration() {
	opposition++;//��ǰ���ʺ�TYPE����ƥ��
	TreeNode* t = NULL;
	t = TypeDecList();
	if (t == NULL) {//��tΪ�գ���ʾ��ʾ��Ϣ
		cout << "��������Ϊ��" << "\n";
	}
	return t;
}

TreeNode* Parse::TypeDecList() {
	TreeNode* t = new TreeNode;
	t->nodekind = DecK;
	//Ӧ����t����ɹ����ж�

	TypeId(t);//���ú���typeid

	if (s[opposition].word.tok != EQ) {
		cout << "����������=���ִ���" << "\n";
		return NULL;
	}
	else {
		opposition++;//��ǰ���ʺ�EQƥ��
	}

	TypeDef(t);//���ú���typedef

	if (s[opposition].word.tok != SEMI) {
		cout << "���������У���������" << "\n";
		return NULL;
	}
	else {
		opposition++;//��ǰ���ʺ�SEMIƥ��
	}

	TreeNode *p = TypeDecMore();
	if (p == NULL) {
		return t;
	}
	else {
		t->sibling = p;
	}
	return t;
}

TreeNode* Parse::TypeDecMore() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == VAR || s[opposition].word.tok == PROCEDURE || s[opposition].word.tok == BEGIN) {//��֮�������������ֽ���(����������������ͷ)
		return t;
		//�˴�Ҳ����Ҫȡ��һ�����ʣ�ֻ�Ƕ�������ʽ��бȽϣ�����ƥ��
	}
	else {
		if (s[opposition].word.tok == ID) {
			t = TypeDecList();
			return t;
		}
		else {
			cout << "TypeDecMore�����ִ��󣿣���" << "\n";
			return t;
		}
	}
}

void Parse::TypeId(TreeNode *t) {
	if (s[opposition].word.tok == ID || t != NULL) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
	}
	else {
		cout << "TypeId����������" << "\n";
	}
	opposition++;//��ǰ���ʺ�ID����ƥ��
}

void Parse::TypeDef(TreeNode *t) {//�����⣡����������������������������������
	if (t != NULL) {
		if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR) {
			BaseType(t);
		}
		else if (s[opposition].word.tok == ARRAY || s[opposition].word.tok == RECORD) {
			StructureType(t);
		}
		else if (s[opposition].word.tok == ID) {
			t->k.dec = Idk; //�����ͱ�ʶ����Ϊ����
			t->type_name = s[opposition].word.str;//type_name������¼���ͱ�ʶ��(����)������***************??????????????????????????????
			opposition++;//ƥ��ID
		}
		else {
			cout << "TypeDef���ִ���" << "\n";
			opposition++;//�������󵥴ʡ�������һ������
		}
	}
}

void Parse::BaseType(TreeNode *t) {
	if (s[opposition].word.tok == INTEGER) {
		opposition++;//ƥ�䵥��INTEGER
		t->k.dec = IntegerK;
	}
	else if (s[opposition].word.tok == CHAR) {
		opposition++;//ƥ�䵥��CHAR
		t->k.dec = CharK;
	}
	else {
		cout << "BaseType��������" << "\n";
		opposition++;//������һ������
	}
}

void Parse::StructureType(TreeNode *t) {
	if (s[opposition].word.tok == ARRAY) {
		ArrayType(t);
	}
	else if (s[opposition].word.tok == RECORD) {
		t->k.dec = RecordK;
		RecType(t);
	}
}

void Parse::ArrayType(TreeNode *t) {
	opposition++;//��ǰ���ʺ�ATRRAY����ƥ��
	if (s[opposition].word.tok == LMIDPAREN) {
		opposition++;//��ǰ���ʺ�LMIDPAREN����ƥ��
		if (s[opposition].word.tok == INTC) {//ȡ�������½�
			//����ǰ�������ַ���ת��Ϊ�����������
			int sum = 0;//��¼����ת��Ϊ�������������
			int length = s[opposition].word.str.length();
			for (int i = 0; i < length; i++) {
				sum = sum * 10;
				sum += s[opposition].word.str[i] - '0';
			}
			t->array_low = sum;//�����ֵ��ֵ�������½�
		}
		else {
			cout << "�����½���ִ���" << "\n";
		}
		opposition++;//��ǰ���ʺ�INTC����ƥ��
		opposition++;//��ǰ���ʺ�UNDERANGE����ƥ��
		//************************************�費��Ҫ�жϵ�ǰ�����Ƿ���UNDERANGE����������

		if (s[opposition].word.tok == INTC) {//ȡ�������Ͻ�
			int sum = 0;
			int length = s[opposition].word.str.length();
			for (int i = 0; i < length; i++) {
				sum = sum * 10;
				sum += s[opposition].word.str[i] - '0';
			}
			t->array_high = sum;//�����ֵ��ֵ�������Ͻ�
		}
		else {
			cout << "�����Ͻ���ִ���" << "\n";
		}
		opposition++;//��ǰ���ʺ�INTC����ƥ��
		opposition++;//��ǰ���ʺ�RMIDPAREN����ƥ��
		opposition++;//��ǰ���ʺ�OF����ƥ��

		BaseType(t);

		t->childType = t->k.dec;//�������͸�ֵ
		t->k.dec = ArrayK;
	}
	else {
		cout << "��ǰ�����ų�������" << "\n";
	}
}

void Parse::RecType(TreeNode *t) {
	opposition++;//��ǰ���ʺ�RECORD����ƥ��
	TreeNode *p = FieldDecList();

	if (p != NULL) {
		t->child.push_back(p);
	}
	else {
		cout << "FieldDecList���ص�ֵΪ�գ�RecType������" << "\n";
	}
	opposition++;//��ǰ���ʺ�END����ƥ��
}

TreeNode* Parse::FieldDecList() {
	TreeNode *t = new TreeNode;
	t->nodekind = DecK;

	TreeNode *p = NULL;
	//Ӧ����ж�t�Ƿ�Ϊ��
	if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR) {
		BaseType(t);
		IdList(t);
		opposition++;//��ǰ���ʺ�SEMI����ƥ��
		p = FieldDecMore();
	}
	else if (s[opposition].word.tok == ARRAY) {
		ArrayType(t);
		IdList(t);
		opposition++;//��ǰ���ʺ�SEMI����ƥ��
		p = FieldDecMore();
	}
	else {
		cout << "FieldDecList���ִ���" << "\n";
		opposition++;//������ǰ���󵥴�
	}
	t->sibling = p;
	return t;
}

TreeNode* Parse::FieldDecMore() {
	TreeNode* t = NULL;
	if (s[opposition].word.tok == END) {
		return t;
	}
	else {
		if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR || s[opposition].word.tok == ARRAY) {
			t = FieldDecList();
		}
		else {
			cout << "FieldDecMore��������" << "\n";
			opposition++;//������ǰ���󵥴�
		}
		return t;
	}
}

void Parse::IdList(TreeNode *t) {
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
		opposition++;//ƥ�䵱ǰ��ʶ��
	}
	else {
		cout << "IdList��û��ID" << "\n";
	}
	IdMore(t);
}

void Parse::IdMore(TreeNode *t) {
	if (s[opposition].word.tok != SEMI) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;
			IdList(t);
		}
		else {
			cout << "IdMore���ִ��󣬻�������Ϊû��','��';'" << "\n";
			opposition++;//��ȡ��һ������  
		}
	}
	//����ǰtokenΪSEMI��;����Ҳ����opposition++������fielddeclist����ƥ��
}

TreeNode* Parse::VarDec() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != PROCEDURE && s[opposition].word.tok != BEGIN) {
		if (s[opposition].word.tok == VAR) {
			t = VarDeclaration();
		}
		else {
			cout << "VarDec���������⣿" << "\n";
			opposition++;//ȡ��һ������
		}
	}
	return t;
}

TreeNode* Parse::VarDeclaration() {
	opposition++;//��ǰtoken��VAR����ƥ��
	TreeNode *t = VarDecList();
	if (t == NULL) {
		cout << "�����������ֳ��ִ���" << "\n";
	}
	return t;
}

TreeNode* Parse::VarDecList() {
	TreeNode *t = new TreeNode;
	t->nodekind = DecK;     //�����µ��﷨�������ڵ�

	TreeNode *p = NULL;
	if (t != NULL) {
		TypeDef(t);
		VarIdList(t);
		opposition++;//ƥ�䱣����SEMI
		p = VarDecMore();
		t->sibling = p;
	}
	return t;
}

TreeNode* Parse::VarDecMore() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != PROCEDURE && s[opposition].word.tok != BEGIN) {
		if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR || s[opposition].word.tok == ARRAY || s[opposition].word.tok == RECORD || s[opposition].word.tok == ID) {
			t = VarDecList();
		}
		else {
			cout << "VarDecMore���������⣿��" << "\n";
			opposition++;//������һ������
		}
	}
	return t;
}


void Parse::VarIdList(TreeNode *t) {
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
		opposition++;//��ǰtoken��ID����ƥ��
	}
	else {
		cout << "VarIdList��ID���ִ���" << "\n";
		opposition++;//������һ������
	}
	VarIdMore(t);
}

void Parse::VarIdMore(TreeNode *t) {
	if (s[opposition].word.tok != SEMI) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;//ƥ�䱣����COMMA
			VarIdList(t);
		}
		else {
			cout << "VarIdMore��������,��������Ϊȱʧ';'��','" << "\n";
			opposition++;//������һ������
		}
	}
	//��varidmoreΪ�գ���ô�ֺ���������ĺ�������ƥ��
}

TreeNode* Parse::ProcDec() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != BEGIN) {
		if (s[opposition].word.tok == PROCEDURE) {
			t = ProcDeclaration();
		}
		else {
			cout << "ProcDec����������" << "\n";
			opposition++;//������һ������
		}
	}
	//����ǰ����ΪBEGIN����ô�����κζ���
	return t;
}

TreeNode* Parse::ProcDeclaration() {
	TreeNode *t = new TreeNode;
	t->nodekind = ProcDecK;
	opposition++;//��ǰ���ʺ�PROCEDURE����ƥ��
	//Ӧ�����ж��½ڵ��Ƿ�����ɹ�
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);//��¼������
		t->idnum++;
		opposition++;//ƥ�䵥��ID
	}
	else {
		cout << "û�к�������" << "\n";
	}

	opposition++;//��ǰ���ʺ�LPAREN�������ţ�����ƥ��
	ParamList(t);
	opposition++;//��ǰ���ʺ�RPAREN�������ţ�����ƥ��
	opposition++;//��ǰ���ʺ�COLON���Ⱥţ�����ƥ��

	TreeNode *temp1 = ProcDecPart();
	t->child.push_back(temp1);
	TreeNode *temp2 = ProcBody();
	t->child.push_back(temp2);

	return t;
}

void Parse::ParamList(TreeNode *t) {
	TreeNode *p = NULL;
	if (s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR || s[opposition].word.tok == RECORD || s[opposition].word.tok == ARRAY || s[opposition].word.tok == ID || s[opposition].word.tok == VAR) {
			p = ParamDecList();
			t->child.push_back(p);
		}
		else {
			cout << "ParamList����������" << "\n";
			opposition++;//������ǰtoken����ȡ��һ������
		}
	}
	//��Ϊ�����ţ�ֱ�ӽ����������κζ���
}

TreeNode* Parse::ParamDecList() {
	TreeNode *t = Param();
	TreeNode *p = ParamMore();
	if (p != NULL) {
		t->sibling = p;
	}
	return t;
}

TreeNode* Parse::ParamMore() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == SEMI) {
			opposition++;//��ǰ���ʺ�SEMI����ƥ��
			t = ParamDecList();
			if (t == NULL) {
				cout << "ParamMore�� ;��ParamDecList���ִ���" << "\n";
			}
		}
		else {
			cout << "ParamMore�������⣬������ȱʧ';'" << "\n";
			opposition++;//������һ������
		}
	}
	return t;
}

TreeNode* Parse::Param() {
	TreeNode *t = new TreeNode;
	t->nodekind = DecK;
	//Ӧ�����жϽڵ��Ƿ񴴽��ɹ����ж�
	if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR || s[opposition].word.tok == RECORD || s[opposition].word.tok == ARRAY || s[opposition].word.tok == ID) {
		t->paramt = valparamtype;
		TypeDef(t);
		FormList(t);
	}
	else if (s[opposition].word.tok == VAR) {
		t->paramt = varparamtype;
		TypeDef(t);
		FormList(t);
	}
	else {
		cout << "Param�������⣬��������" << "\n";
		opposition++;//��ȡ��һ������
	}
	return t;
}

void Parse::FormList(TreeNode *t) {
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
		opposition++;//ƥ��ID
	}
	else {
		cout << "FormListû��ID��" << "\n";
	}
	FidMore(t);
}

void Parse::FidMore(TreeNode *t) {
	if (s[opposition].word.tok != SEMI && s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;//ƥ�䵱ǰ����COMMA
			FormList(t);
		}
		else {
			cout << "FidMore��������" << "\n";
			opposition++;
		}
	}
	//����ǰ����Ϊ�����Ż��߷ֺţ���ôʲô��������
}

TreeNode* Parse::ProcDecPart() {
	TreeNode *t = DeclarePart();
	return t;
}

TreeNode* Parse::ProcBody() {
	TreeNode* t = ProgramBody();
	if (t == NULL) {
		cout << "ProcBody��ProgramBody���ص�tΪ��" << "\n";
	}
	return t;
}

TreeNode* Parse::ProgramBody() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmLK;
	if (s[opposition].word.tok == BEGIN) {
		opposition++;//��ǰ���ʺ�BEGIN���б���
	}
	else {
		cout << "ProgramBody��ȱʧBEGIN" << "\n";
	}

	TreeNode *temp = StmList();
	t->child.push_back(temp);

	if (s[opposition].word.tok == END) {
		opposition++;//��ǰ���ʺ�END����ƥ��
	}
	else {
		cout << "ProgramBody��ȱʧEND" << "\n";
	}
	return t;
}

TreeNode* Parse::StmList() {
	TreeNode *t = NULL;
	t = Stm();
	TreeNode *p = StmMore();
	if (p != NULL) {
		t->sibling = p;
	}
	return t;
}

TreeNode* Parse::StmMore() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != END && s[opposition].word.tok != ENDWHILE && s[opposition].word.tok != ELSE && s[opposition].word.tok != FI) {
		if (s[opposition].word.tok == SEMI) {
			opposition++;//��ǰ���ʺ�SEMIƥ��
			t = StmList();
		}
		else {//StmMore����������ǳ����ˣ�if��������дStmL
			cout << "StmMore�г�������" << "\n";
			opposition++;
		}
	}
	//��ΪEND��ENDWHILE��ֱ�ӷ���t��������
	return t;
}

TreeNode* Parse::Stm() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == IF) {
		t = ConditionalStm();
	}
	else if (s[opposition].word.tok == WHILE) {
		t = LoopStm();
		//opposition++;//ƥ��endwh������*****************************************
	}
	else if (s[opposition].word.tok == RETURN) {
		t = ReturnStm();
	}
	else if (s[opposition].word.tok == READ) {
		t = InputStm();
	}
	else if (s[opposition].word.tok == WRITE) {
		t = OutputStm();
	}
	else if (s[opposition].word.tok == ID) {
		//********************************************************************************************
		temp_name = new TreeNode;
		temp_name->name.push_back(s[opposition].word.str);//��name��ʱ������ʱ����ID����Ϣ
		temp_name->nodekind = ExpK;
		temp_name->k.EXP = IdEK;
		temp_name->vark = IdV;
		//temp_name.name.push_back(s[opposition].word.str);//��name��ʱ������ʱ����ID����Ϣ
		//temp_name.nodekind = ExpK;
		//********************************************************************************************
		opposition++;//ƥ��ID
		t = AssCall();
	}
	else {
		cout << "Stm�У���ǰ����Ϊ�������ʣ������������﷨����" << "\n";
		opposition++;//������һ������
	}
	return t;
}

TreeNode* Parse::AssCall() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == ASSIGN) {
		t = AssignmentRest();
	}
	else if (s[opposition].word.tok == LPAREN) {
		t = CallStmRest();
	}
	//********************************************************************************************
	else if (s[opposition].word.tok == LMIDPAREN) {//A[i]:=?
		opposition++;//ƥ��LMIDPAREN
		temp_name->vark = ArrayMembV;
		temp_name->child.push_back(Exp());
		//temp_name->k.dec = ArrayK;
		temp_name->child[0]->vark = IdV;
		opposition++;//ƥ��RMIDPAREN
		t = AssignmentRest();
	}
	//********************************************************************************************
	else {//�Ⱥ�����޷�����a[]�����ı�����ֻ���ڵȺ��Ҳ���������г���
		cout << "AssCall���ִ���" << "\n";
	}
	return t;
}

TreeNode* Parse::AssignmentRest() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = AssignK;
	if (s[opposition].word.tok == ASSIGN) {
		opposition++;//��ǰ���ʺ�ASSIGN����ƥ��
	}
	else {
		cout << "AssignmentRest��EQƥ��ʧ�ܣ�������ȱʧ=" << "\n";
	}
	
	//********************************************************************************************
	t->child.push_back(temp_name);//�Ѹ�ֵ�����Ҳ����һ����=�ڵ��child
	//********************************************************************************************
	t->child.push_back(Exp());
	//t->name.push_back(temp_name);

	return t;
}

TreeNode* Parse::ConditionalStm() {//*********************************************************
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = IfK;

	opposition++;//��ǰ���ʺ�IF����ƥ��
	t->child.push_back(Exp());//IF�����������ʽ
	opposition++;//��ǰ���ʺ�THEN����ƥ��
	

	//t->child.push_back(StmL());//����Ϊ��Ĵ������        �˴���StmL������Ҫ��дһ���µģ���
	t->child.push_back(StmList());
	//��ʱ����StmList��֮���ٸ�********************************************************************************
	

	if (s[opposition].word.tok == ELSE) {
		opposition++;//��ǰ���ʺ�ELSE����ƥ��


		//t->child.push_back(StmL());//����Ϊ�ٵĴ������    �����StmL����ͬ��!!!
		t->child.push_back(StmList());
		//��ʱ����StmList��֮���ٸ�****************************************************************************


	}

	if (s[opposition].word.tok == FI) {
		opposition++;//��ǰ���ʺ�FI����ƥ��
	}
	else {
		cout << "ConditionalStmȱʧFI" << "\n";
	}
	return t;
}

TreeNode* Parse::LoopStm() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = WhileK;
	opposition++;//��ǰ���ʺ�Whileƥ��
	t->child.push_back(Exp());//WHILE�����������ʽ
	if (s[opposition].word.tok == DO) {
		opposition++;//��ǰ���ʺ�DOƥ��
	}
	else {
		cout << "LoopStm��DOƥ��ʧ��" << "\n";
	}
	t->child.push_back(StmList());//ѭ����䲿��
	opposition++;//ƥ��endwh
	return t;
}

TreeNode* Parse::InputStm() {
	TreeNode* t = new  TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = ReadK;
	opposition++;//��ǰ���ʺ�READƥ��
	opposition++;//��ǰ���ʺ�LPARENƥ��

	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
	}
	opposition++;//��ǰ���ʺ�ID����ƥ��
	opposition++;//��ǰ���ʺ�RPARENƥ��
	return t;
}

TreeNode* Parse::OutputStm() {
	TreeNode* t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = WriteK;
	opposition++;//��ǰ���ʺ�WRITEƥ��
	opposition++;//��ǰ���ʺ�LPARENƥ��
	t->child.push_back(Exp());//WRITE���ı��ʽ
	opposition++;//��ǰ���ʺ�RPARENƥ��
	return t;
}

TreeNode* Parse::ReturnStm() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = ReturnK;
	opposition++;//��ǰ���ʺ�RETURNƥ��
	return t;
}

TreeNode* Parse::CallStmRest() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = CallK;
	opposition++;//��ǰ���ʺ�LPARENƥ��

	//**********************************************************************************************
	t->child.push_back(temp_name);
	//**********************************************************************************************

	t->child.push_back(ActParamList());
	//t->name.push_back(temp_name);
	opposition++;//��ǰ���ʺ�RPARENƥ��
	return t;
}

TreeNode* Parse::ActParamList() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == ID || s[opposition].word.tok == INTC) {
			t = Exp();
			if (t != NULL) {
				t->sibling = ActParamMore();
			}
		}
		else {
			cout << "ActParamList�г��ִ���?" << "\n";
			opposition++;//������һ������
		}
	}
	return t;
	//����ǰ����ΪRPAREN��ֱ�ӷ���t,�˳�����
}

TreeNode* Parse::ActParamMore() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;//��ǰ���ʺ�COMMA����ƥ��
			t = ActParamList();
		}
		else {
			cout << "ActParamMore�г��ִ���? ȱʧ]��," << "\n";
			opposition++;//������һ������
		}
	}
	//����ǰ����ΪRPAREN��ֱ�ӷ���t
	return t;
}

TreeNode* Parse::Exp() {
	TreeNode *t = Simple_exp();//***********************������????????????????????????
	if (s[opposition].word.tok == LT || s[opposition].word.tok == EQ) {
		TreeNode *p = new TreeNode;
		p->nodekind = ExpK;
		p->k.EXP = OpK;//����һ���µ�������ʽ�����﷨���ڵ㣿

		p->child.push_back(t);//��Ϊ������ʽ���������ʽ
		p->op = s[opposition].word.tok;//��Ϊ������ʽ�������
		opposition++;//ƥ�䵱ǰ���ʣ�EQ��LT��
		p->child.push_back(Simple_exp());//��Ϊ������ʽ���������ʽ
		return p;
	}
	return t;
	//������LT��EQ��ֱ�ӷ���t
}

TreeNode* Parse::Simple_exp() {
	TreeNode *t = term();//���������
	while (s[opposition].word.tok == PLUS || s[opposition].word.tok == MINUS) {
		TreeNode *p = new TreeNode;
		p->nodekind = ExpK;
		p->k.EXP = OpK;//����һ���µ�������ʽ�����﷨���ڵ�

		p->child.push_back(t);//������ķ���ֵ��Ϊ����򵥱��ʽ������������
		p->op = s[opposition].word.tok;//��Ϊ������������

		//֮��p,tλ�û�����t��childΪp
		TreeNode *temp = t;
		t = p;
		p = temp;
		//��֤ÿһ��ѭ��֮��tΪ���ڵ㣬��p��child

		opposition++;//ƥ�䵱ǰ���ʣ�PLUS����MINUS��

		t->child.push_back(term());//��Ϊ����򵥱��ʽ����������

		//��Ϊ����a+b+c��ʽ�ӣ����������ĽṹΪa+b�ڵײ㣨�����Ϊm����m+c�ڸ���һ�㣬���շ��ص�tһ������߲�
	}
	return t;
}

TreeNode* Parse::term() {
	TreeNode *t = factor();//�������Ӵ�����
	while (s[opposition].word.tok == TIMES || s[opposition].word.tok == OVER) {
		TreeNode *p = new TreeNode;
		p->nodekind = ExpK;
		p->k.EXP = OpK;//����һ���µ�������ʽ�����﷨���ڵ�

		p->child.push_back(t);//�����Ӵ������ķ���ֵt��Ϊ����򵥱��ʽ������������
		p->op = s[opposition].word.tok;//��Ϊ������������

		//֮��p,tλ�û�����t��childΪp
		TreeNode *temp = t;
		t = p;
		p = temp;
		//��֤ÿһ��ѭ��֮��tΪ���ڵ㣬��p��child

		opposition++;//ƥ�䵱ǰ����(TIMES��OVER)

		t->child.push_back(factor());//��Ϊ�����������������

        //�������ṹ��Simple_exp��ͬ,�ο�Simple_exp����
	}
	return t;
}

TreeNode* Parse::factor() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == INTC) {
		//���ִ����֣������ַ������������֣�
		//***************************************************************
		//����Ϊ�Լ��Ĳ��룺
		t = new TreeNode;
		t->nodekind = ExpK;
		t->k.EXP = ConstK;//����һ���µĳ��������﷨���ڵ�
		//cout << "const����Ŷ" << "\n";

		//����ǰ�������ַ���ת��Ϊ�����������
		int sum = 0;//��¼����ת��Ϊ�������������
		int length = s[opposition].word.str.length();
		for (int i = 0; i < length; i++) {
			sum = sum * 10;
			sum += s[opposition].word.str[i] - '0';
		}
		t->num = sum;//������������﷨���ڵ����ֵ

		opposition++;//�����뵱ǰINTC����ƥ��
		//***************************************************************
	}
	else if (s[opposition].word.tok == ID) {
		t = variable();
	}
	else if (s[opposition].word.tok == LPAREN) {
		opposition++;//��ǰ���ʺ�LPARENƥ��
		t = Exp();//���ñ��ʽ������
		opposition++;//��ǰ���ʺ�RPARENƥ��
	}
	else {
		cout << "��ǰ����Ϊ�������ʣ����������ʴ���" << "\n";
		opposition++;//������һ������
	}
	return t;
}

TreeNode* Parse::variable() {
	TreeNode *t = new TreeNode;
	t->nodekind = ExpK;
	t->k.EXP = IdEK;
	t->vark = IdV;
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);//����ID����
		t->lineno = s[opposition].lineshow;//�����к�
		opposition++;//ƥ��ID
		variMore(t);
	}
	else {
		cout << "variable��ID���ִ���??" << "\n";
	}
	return t;
}

void Parse::variMore(TreeNode *t) {//pdf p112
	LexType temptype = s[opposition].word.tok;
	if (temptype != ASSIGN && temptype != TIMES && temptype != EQ && temptype != LT && temptype != PLUS && temptype != MINUS && temptype != OVER && temptype != RPAREN && temptype != RMIDPAREN && temptype != SEMI && temptype != COMMA && temptype != THEN && temptype != ELSE && temptype != FI && temptype != DO && temptype != ENDWHILE && temptype != END) {
		if (temptype == LMIDPAREN) {
			opposition++;//ƥ��LMIDPAREN
			t->child.push_back(Exp());//T�ĵ�һ�����Ӹ�ֵΪexp()
			//��tmd���Ǹ�ɶ����������
			t->vark = ArrayMembV;//t��varkind������ΪArrayMembV
			t->child[0]->vark = IdV;//t�ĵ�һ�����ӵ�varkind����ΪIdV
			opposition++;//ƥ��RMIDPAREN
		}
		else if (temptype == DOT) {
			opposition++;//ƥ��DOT
			t->child.push_back(fieldvar());//t�ĵ�һ�����Ӹ�ֵΪfieldvar()
			//��tmd���Ǹ�ɶ������
			t->vark = FieldMembV;
			t->child[0]->vark = IdV;
		}
		else {
			cout << "variMore�г��ִ���" << "\n";
			opposition++;//��ȡ��һ��token
		}
	}
	//��temptype����������Щ��ֱ���˳�����
}

TreeNode* Parse::fieldvar() {
	TreeNode *t = new TreeNode;
	t->nodekind = ExpK;
	t->k.EXP = IdEK;//�����µı��ʽ�ڵ�

	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);//���浱ǰID������
		t->lineno = s[opposition].lineshow;
		opposition++;//ƥ��ID
		fieldvarMore(t);
	}
	else {
		cout << "fieldvar�г��ִ��󣿣�" << "\n";
	}
	return t;
}

void Parse::fieldvarMore(TreeNode *t) {
	LexType temptype = s[opposition].word.tok;
	if (temptype != ASSIGN && temptype != TIMES && temptype != EQ && temptype != LT && temptype != PLUS && temptype != MINUS && temptype != OVER && temptype != RPAREN && temptype != SEMI && temptype != COMMA && temptype != THEN && temptype != ELSE && temptype != FI && temptype != DO && temptype != ENDWHILE && temptype != END) {
		if (temptype == LMIDPAREN) {
			opposition++;//ƥ��LMIDPAREN
			t->child.push_back(Exp());//t�ĵ�һ�����Ӹ�ֵΪexp()
			//��tmd�Ǹ�ɶ������
			t->child[0]->vark = ArrayMembV;//��t�ĵ�һ�����ӵ�varkind����ΪArrayMembV
			opposition++;//ƥ��PMIDPAREN
		}
		else {
			cout << "fieldvarMore�г��ִ���??" << "\n";
			opposition++;//��ȡ��һ��token
		}
	}
}

void Parse::match(LexType expected) {
	//????????
	//�ú�������ǰ�����뺯����������������Ƚϣ����һ�£���ȡ��һ�����ʣ��������˳��﷨��������
	//����exit(0)�˳�����

	if (s[opposition].word.tok == expected) {
		opposition++;//��Ϊָ���ĵ��ʣ�ȡ��һ������
	}
	else {
		//�����˳���ǰ�﷨��������
		exit(0);
	}
}

void Parse::PrintParseTree() {    //��һ���ļ�ָ�������Ӧ�ÿ���ͨ���ݹ�ʵ��
	cout << "ִ������﷨��" << "\n";
	TreeNode *nowop = root;
	if (nowop == NULL) {
		cout << "����??" << "\n";
	}
	int depth = 0;//���ڼ�¼��ǰ���﷨���ڵ�����,���ڿ�������ո�����
	DFS(nowop, 0);
}

void Parse::DFS(TreeNode *t, int depth) {
	while (t != NULL) {
		for (int i = 0; i < depth; i++) {
			cout << "		";
		}
		cout << transform(t->nodekind);
		//*******************************************************************
		//�����ǶԸ����ڵ�ľ����������
		if (t->nodekind == PheadK || t->nodekind == ProcDecK) {
			cout << " " << t->name[0] << "\n";
		}
		else if (t->nodekind == DecK) {
			cout << " " << transform_deckind_to_string(t->k.dec);
			for (int j = 0; j < t->name.size(); j++) {
				cout << " " << t->name[j];
			}
			cout << "\n";
		}
		else if (t->nodekind == StmtK) {
			//cout << " " << t->op << "\n";
			if (t->k.stmt == IfK) {
				cout << " " << "If" << "\n";
				//if���������ʽ������Ϊ��Ĵ�����������Ϊ�ٵĴ�����䶼����if�ڵ��child��
				//if�д������⣬���ÿһ������ж��ж�����䣬��������
			}
			else if (t->k.stmt == WhileK) {
				cout << " " << "While" << "\n";
				//while���������ʽ��ѭ����䲿�־�����while�ڵ��child��
			}
			else if (t->k.stmt == AssignK) {
				cout << " " << "Assign" << "\n";
				/*
				//assign�ĸ�ֵ�����ı�����name�У����Ҫ�����һ����һ��ĵȺ����
				for (int i = 0; i <= depth; i++) {
					cout << "		";
				}
				cout << "ExpK" << " " << t->name[0] << " IdV" << "\n";
				//���������û��a[3]:=?���ֲ���    ���忴����stm��asscall   û��ƥ��[�Ĳ���
				*/
				//���ڵȺ����Ҳ���ŵ�child��
			}
			else if (t->k.stmt == ReadK) {
				cout << " " << "Read" << " " << t->name[0] << "\n";
			}
			else if (t->k.stmt == WriteK) {
				cout << " Write" << "\n";
				//write���ı��ʽ��child�У����㲻������
			}
			else if (t->k.stmt == CallK) {
				cout << " Call" << "\n";
				/*
				//��������������call�ڵ��name�У����Ӧ�������һ��ĺ�����
				for (int i = 0; i <= depth; i++) {
					cout << "		";
				}
				cout << "ExpK " << t->name[0] << " IdV" << "\n";
				*/
				//���ڸ�Ϊ������Ҳ�ŵ�child��
			}
			else if (t->k.stmt == ReturnK) {
				cout << " Return" << "\n";
				//?  ��Ҳ��֪��զ������Ӧ��������
			}
		}
		else if (t->nodekind == ExpK) {
			if (t->k.EXP == OpK) {
				cout << " OP " << tranform_op_to_string(t->op) << "\n";
				//������exp��simple_exp��term����factor����������ߵ��������������child��
			}
			else if (t->k.EXP == ConstK) {
				cout << " Const " << t->num << "\n";
			}
			else if (t->k.EXP == IdEK) {
				if (t->vark == ArrayMembV) {
					cout << " " << t->name[0] << "[] IdV\n";
				}
				else {
					cout << " " << t->name[0] << " IdV\n";
				}
				//*********************************************************************************�������⣡
				//��û�п���a[i+1]��a.field�����������ô����
			}
		}
		else {
			cout << "\n";
		}
		//�����﷨���ڵ�󲻻������������ֻ���﷨���ڵ�����
		//*******************************************************************
		for (int j = 0; j < t->child.size(); j++) {//��ÿһ��t�ĺ��Ӿ����б���
			DFS(t->child[j], depth + 1);
		}
		t = t->sibling;//ȥ����t���ֵܽڵ�
	}
}

string Parse::transform(TreeNodeType type) {
	switch (type)
	{
	case 0:return "ProK";
	case 1:return "PheadK";
	case 2:return "TypeK";
	case 3:return "VarK";
	case 4:return "ProcDecK";
	case 5:return "StmLK";
	case 6:return "DecK";
	case 7:return "StmtK";
	case 8:return "ExpK";
	default:
		break;
	}
}

string Parse::transform_deckind_to_string(DecKind kind) {
	switch(kind) {
	case 0:return "Array";
	case 1:return "CharK";
	case 2:return "IntegerK";
	case 3:return "RecordK";
	case 4:return "Idk";
	default:
		break;
	}
}

string Parse::tranform_op_to_string(LexType op) {
	if (op == EQ) {
		return "=";
	}
	else if (op == LT) {
		return "<";
	}
	else if (op == PLUS) {
		return "+";
	}
	else if (op == MINUS) {
		return "-";
	}
	else if (op == TIMES) {
		return "*";
	}
	else if (op == OVER) {
		return "/";
	}
	else {
		return "????";
	}
}

TreeNode* Parse::getRoot() {
	return root;
}