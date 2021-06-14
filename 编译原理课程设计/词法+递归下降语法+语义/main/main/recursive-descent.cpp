#include "pch.h"
#include "recursive-descent.h"
#include <iostream>

using namespace std;

void Parse::getResult(token s[]) {
	this->s = s;//读入tokenlist
	root = parse();
	//parse函数生成语法树，返回根节点到root
	//再去调用输出函数
}

TreeNode* Parse::parse() {
	TreeNode* root;
	//读入一个单词???
	root = Program();//调用总程序处理分析程序
	if (s[opposition].word.tok == ENDFILE) {
		return root;
	}
	else {//文件提前结束，应进行错误处理
		return NULL;//????
	}
}

TreeNode* Parse::Program() {
	TreeNode *programhead, *declarepart, *programbody;
	programhead = ProgramHead();
	declarepart = DeclarePart();
	programbody = ProgramBody();
	TreeNode *t = new TreeNode;
	t->nodekind = ProK;//根节点设为根标志节点
	//应判断新语法树root是否创建成功，此处未进行判断
	if (programhead == NULL) {//若programhead为空，应进行报错
		cout << "ProgramHead为空" << "\n";
		return NULL;
	}
	else {
		t->child.push_back(programhead);
	}
	if (declarepart == NULL) {//若declarepart为空，应进行报错
		cout << "DeclarePart为空" << "\n";
		return NULL;
	}
	else {
		t->child.push_back(declarepart);
	}
	if (programbody == NULL) {//若programbody为空，应进行报错
		cout << "programbody为空" << "\n";
		return NULL;
	}
	else {
		t->child.push_back(programbody);
	}
	//match(DOT);//当前单词和DOT进行匹配
	opposition++;
	return t;
}

TreeNode* Parse::ProgramHead() {
	TreeNode *t = new TreeNode;
	if (s[opposition].word.tok == PROGRAM) {//匹配保留字PROGRAM
		opposition++;
		if (s[opposition].word.tok == ID) {//若当前单词为ID
			t->nodekind = PheadK;
			t->idnum++;
			t->name.push_back(s[opposition].word.str);
			opposition++;
			return t;
		}
		else {
			cout << "程序名未找到" << "\n";
			return NULL;
		}

	}
	else {//***************************************************未匹配成功，咋办？？？
		cout << "PROGRAM匹配失败\n";
		return NULL;
	}
}

TreeNode* Parse::DeclarePart() {//*****************************************存在问题？？？？？
	TreeNode *tp1 = TypeDec();
	TreeNode *typed = NULL;
	if (tp1 != NULL) {//先去看是否存在类型声明，若存在、再去创建一个类型声明标志语法树节点
		typed = new TreeNode;
		typed->nodekind = TypeK;//创建类型声明标志类型语法树节点
		typed->child.push_back(tp1);
	}
	
	TreeNode *tp2 = VarDec();
	TreeNode *vard = NULL;
	if (tp2 != NULL) {//先去看是否存在变量声明，若存在、再去创建一个变量声明标志语法树节点
		vard = new TreeNode;
		vard->nodekind = VarK;//创建变量声明标志类型语法树节点
		vard->child.push_back(tp2);
	}
	
	TreeNode *s = ProcDec();
	//以上分别调用类型声明、变量声明、过程声明部分处理分析函数

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
	if (s[opposition].word.tok == TYPE) {//若这一位为TYPE、既存在类型声明，那么创建语法树节点，否则返回null
		t = TypeDeclaration();
		//opposition++;   此处只是进行判断：是否存在类型声明，不需要取下一个token
		return t;
	}
	else {
		if (s[opposition].word.tok == VAR || s[opposition].word.tok == PROCEDURE || s[opposition].word.tok == BEGIN) {//若不存在类型定义，返回null
			return t;
		}
		else {//出现错误
			cout << "类型声明出现错误" << "\n";
			opposition++;//跳过此错误单词，读入下一个token
			return NULL;
		}
	}
}

TreeNode* Parse::TypeDeclaration() {
	opposition++;//当前单词和TYPE进行匹配
	TreeNode* t = NULL;
	t = TypeDecList();
	if (t == NULL) {//若t为空，显示提示信息
		cout << "类型声明为空" << "\n";
	}
	return t;
}

TreeNode* Parse::TypeDecList() {
	TreeNode* t = new TreeNode;
	t->nodekind = DecK;
	//应进行t申请成功的判定

	TypeId(t);//调用函数typeid

	if (s[opposition].word.tok != EQ) {
		cout << "类型声明中=出现错误" << "\n";
		return NULL;
	}
	else {
		opposition++;//当前单词和EQ匹配
	}

	TypeDef(t);//调用函数typedef

	if (s[opposition].word.tok != SEMI) {
		cout << "类型声明中；出现问题" << "\n";
		return NULL;
	}
	else {
		opposition++;//当前单词和SEMI匹配
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
	if (s[opposition].word.tok == VAR || s[opposition].word.tok == PROCEDURE || s[opposition].word.tok == BEGIN) {//若之后类型声明部分结束(出现了其他的声明头)
		return t;
		//此处也不需要取下一个单词，只是对这个单词进行比较，并不匹配
	}
	else {
		if (s[opposition].word.tok == ID) {
			t = TypeDecList();
			return t;
		}
		else {
			cout << "TypeDecMore处出现错误？？？" << "\n";
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
		cout << "TypeId处出现问题" << "\n";
	}
	opposition++;//当前单词和ID进行匹配
}

void Parse::TypeDef(TreeNode *t) {//有问题！！！！！！！！！！！！！！！！！！
	if (t != NULL) {
		if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR) {
			BaseType(t);
		}
		else if (s[opposition].word.tok == ARRAY || s[opposition].word.tok == RECORD) {
			StructureType(t);
		}
		else if (s[opposition].word.tok == ID) {
			t->k.dec = Idk; //以类型标识符作为类型
			t->type_name = s[opposition].word.str;//type_name用来记录类型标识符(类型)的名字***************??????????????????????????????
			opposition++;//匹配ID
		}
		else {
			cout << "TypeDef出现错误" << "\n";
			opposition++;//跳过错误单词、读入下一个单词
		}
	}
}

void Parse::BaseType(TreeNode *t) {
	if (s[opposition].word.tok == INTEGER) {
		opposition++;//匹配单词INTEGER
		t->k.dec = IntegerK;
	}
	else if (s[opposition].word.tok == CHAR) {
		opposition++;//匹配单词CHAR
		t->k.dec = CharK;
	}
	else {
		cout << "BaseType出现问题" << "\n";
		opposition++;//读入下一个单词
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
	opposition++;//当前单词和ATRRAY进行匹配
	if (s[opposition].word.tok == LMIDPAREN) {
		opposition++;//当前单词和LMIDPAREN进行匹配
		if (s[opposition].word.tok == INTC) {//取得数组下界
			//将当前的数字字符串转换为有意义的数字
			int sum = 0;//记录最终转换为的有意义的数字
			int length = s[opposition].word.str.length();
			for (int i = 0; i < length; i++) {
				sum = sum * 10;
				sum += s[opposition].word.str[i] - '0';
			}
			t->array_low = sum;//将这个值赋值给数组下界
		}
		else {
			cout << "数组下界出现错误" << "\n";
		}
		opposition++;//当前单词和INTC进行匹配
		opposition++;//当前单词和UNDERANGE进行匹配
		//************************************需不需要判断当前单词是否是UNDERANGE？？？？？

		if (s[opposition].word.tok == INTC) {//取得数组上界
			int sum = 0;
			int length = s[opposition].word.str.length();
			for (int i = 0; i < length; i++) {
				sum = sum * 10;
				sum += s[opposition].word.str[i] - '0';
			}
			t->array_high = sum;//将这个值赋值给数组上界
		}
		else {
			cout << "数组上界出现错误" << "\n";
		}
		opposition++;//当前单词和INTC进行匹配
		opposition++;//当前单词和RMIDPAREN进行匹配
		opposition++;//当前单词和OF进行匹配

		BaseType(t);

		t->childType = t->k.dec;//给子类型赋值
		t->k.dec = ArrayK;
	}
	else {
		cout << "当前左方括号出现问题" << "\n";
	}
}

void Parse::RecType(TreeNode *t) {
	opposition++;//当前单词和RECORD进行匹配
	TreeNode *p = FieldDecList();

	if (p != NULL) {
		t->child.push_back(p);
	}
	else {
		cout << "FieldDecList返回的值为空，RecType处出错" << "\n";
	}
	opposition++;//当前单词和END进行匹配
}

TreeNode* Parse::FieldDecList() {
	TreeNode *t = new TreeNode;
	t->nodekind = DecK;

	TreeNode *p = NULL;
	//应添加判断t是否为空
	if (s[opposition].word.tok == INTEGER || s[opposition].word.tok == CHAR) {
		BaseType(t);
		IdList(t);
		opposition++;//当前单词和SEMI进行匹配
		p = FieldDecMore();
	}
	else if (s[opposition].word.tok == ARRAY) {
		ArrayType(t);
		IdList(t);
		opposition++;//当前单词和SEMI进行匹配
		p = FieldDecMore();
	}
	else {
		cout << "FieldDecList出现错误" << "\n";
		opposition++;//跳过当前错误单词
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
			cout << "FieldDecMore出现问题" << "\n";
			opposition++;//跳过当前错误单词
		}
		return t;
	}
}

void Parse::IdList(TreeNode *t) {
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
		opposition++;//匹配当前标识符
	}
	else {
		cout << "IdList中没有ID" << "\n";
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
			cout << "IdMore出现错误，或许是因为没有','或';'" << "\n";
			opposition++;//读取下一个单词  
		}
	}
	//若当前token为SEMI（;），也不用opposition++，留给fielddeclist进行匹配
}

TreeNode* Parse::VarDec() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != PROCEDURE && s[opposition].word.tok != BEGIN) {
		if (s[opposition].word.tok == VAR) {
			t = VarDeclaration();
		}
		else {
			cout << "VarDec处出现问题？" << "\n";
			opposition++;//取下一个单词
		}
	}
	return t;
}

TreeNode* Parse::VarDeclaration() {
	opposition++;//当前token与VAR进行匹配
	TreeNode *t = VarDecList();
	if (t == NULL) {
		cout << "变量声明部分出现错误" << "\n";
	}
	return t;
}

TreeNode* Parse::VarDecList() {
	TreeNode *t = new TreeNode;
	t->nodekind = DecK;     //创建新的语法树声明节点

	TreeNode *p = NULL;
	if (t != NULL) {
		TypeDef(t);
		VarIdList(t);
		opposition++;//匹配保留字SEMI
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
			cout << "VarDecMore处出现问题？？" << "\n";
			opposition++;//读入下一个单词
		}
	}
	return t;
}


void Parse::VarIdList(TreeNode *t) {
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
		opposition++;//当前token与ID进行匹配
	}
	else {
		cout << "VarIdList中ID出现错误" << "\n";
		opposition++;//读入下一个单词
	}
	VarIdMore(t);
}

void Parse::VarIdMore(TreeNode *t) {
	if (s[opposition].word.tok != SEMI) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;//匹配保留字COMMA
			VarIdList(t);
		}
		else {
			cout << "VarIdMore出现问题,可能是因为缺失';'或','" << "\n";
			opposition++;//读入下一个单词
		}
	}
	//若varidmore为空，那么分号留给外面的函数进行匹配
}

TreeNode* Parse::ProcDec() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != BEGIN) {
		if (s[opposition].word.tok == PROCEDURE) {
			t = ProcDeclaration();
		}
		else {
			cout << "ProcDec处出现问题" << "\n";
			opposition++;//读入下一个单词
		}
	}
	//若当前单词为BEGIN，那么不做任何动作
	return t;
}

TreeNode* Parse::ProcDeclaration() {
	TreeNode *t = new TreeNode;
	t->nodekind = ProcDecK;
	opposition++;//当前单词和PROCEDURE进行匹配
	//应进行判断新节点是否申请成功
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);//记录函数名
		t->idnum++;
		opposition++;//匹配单词ID
	}
	else {
		cout << "没有函数名？" << "\n";
	}

	opposition++;//当前单词和LPAREN（左括号）进行匹配
	ParamList(t);
	opposition++;//当前单词和RPAREN（右括号）进行匹配
	opposition++;//当前单词和COLON（等号）进行匹配

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
			cout << "ParamList处出现问题" << "\n";
			opposition++;//跳过当前token，读取下一个单词
		}
	}
	//若为右括号，直接结束，不做任何动作
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
			opposition++;//当前单词和SEMI进行匹配
			t = ParamDecList();
			if (t == NULL) {
				cout << "ParamMore中 ;后ParamDecList出现错误" << "\n";
			}
		}
		else {
			cout << "ParamMore出现问题，可能是缺失';'" << "\n";
			opposition++;//读入下一个单词
		}
	}
	return t;
}

TreeNode* Parse::Param() {
	TreeNode *t = new TreeNode;
	t->nodekind = DecK;
	//应增加判断节点是否创建成功的判断
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
		cout << "Param出现问题，参数不对" << "\n";
		opposition++;//读取下一个单词
	}
	return t;
}

void Parse::FormList(TreeNode *t) {
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
		t->idnum++;
		opposition++;//匹配ID
	}
	else {
		cout << "FormList没有ID？" << "\n";
	}
	FidMore(t);
}

void Parse::FidMore(TreeNode *t) {
	if (s[opposition].word.tok != SEMI && s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;//匹配当前单词COMMA
			FormList(t);
		}
		else {
			cout << "FidMore出现问题" << "\n";
			opposition++;
		}
	}
	//若当前单词为右括号或者分号，那么什么都不用做
}

TreeNode* Parse::ProcDecPart() {
	TreeNode *t = DeclarePart();
	return t;
}

TreeNode* Parse::ProcBody() {
	TreeNode* t = ProgramBody();
	if (t == NULL) {
		cout << "ProcBody中ProgramBody返回的t为空" << "\n";
	}
	return t;
}

TreeNode* Parse::ProgramBody() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmLK;
	if (s[opposition].word.tok == BEGIN) {
		opposition++;//当前单词和BEGIN进行比配
	}
	else {
		cout << "ProgramBody中缺失BEGIN" << "\n";
	}

	TreeNode *temp = StmList();
	t->child.push_back(temp);

	if (s[opposition].word.tok == END) {
		opposition++;//当前单词和END进行匹配
	}
	else {
		cout << "ProgramBody中缺失END" << "\n";
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
			opposition++;//当前单词和SEMI匹配
			t = StmList();
		}
		else {//StmMore这种情况就是出错了，if函数请另写StmL
			cout << "StmMore中出现问题" << "\n";
			opposition++;
		}
	}
	//若为END或ENDWHILE，直接返回t结束函数
	return t;
}

TreeNode* Parse::Stm() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == IF) {
		t = ConditionalStm();
	}
	else if (s[opposition].word.tok == WHILE) {
		t = LoopStm();
		//opposition++;//匹配endwh？？？*****************************************
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
		temp_name->name.push_back(s[opposition].word.str);//用name临时变量暂时保存ID的信息
		temp_name->nodekind = ExpK;
		temp_name->k.EXP = IdEK;
		temp_name->vark = IdV;
		//temp_name.name.push_back(s[opposition].word.str);//用name临时变量暂时保存ID的信息
		//temp_name.nodekind = ExpK;
		//********************************************************************************************
		opposition++;//匹配ID
		t = AssCall();
	}
	else {
		cout << "Stm中，当前单词为其他单词，非期望单词语法错误" << "\n";
		opposition++;//读入下一个单词
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
		opposition++;//匹配LMIDPAREN
		temp_name->vark = ArrayMembV;
		temp_name->child.push_back(Exp());
		//temp_name->k.dec = ArrayK;
		temp_name->child[0]->vark = IdV;
		opposition++;//匹配RMIDPAREN
		t = AssignmentRest();
	}
	//********************************************************************************************
	else {//等号左侧无法出现a[]这样的变量，只能在等号右侧或函数调用中出现
		cout << "AssCall出现错误" << "\n";
	}
	return t;
}

TreeNode* Parse::AssignmentRest() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = AssignK;
	if (s[opposition].word.tok == ASSIGN) {
		opposition++;//当前单词和ASSIGN进行匹配
	}
	else {
		cout << "AssignmentRest中EQ匹配失败，可能是缺失=" << "\n";
	}
	
	//********************************************************************************************
	t->child.push_back(temp_name);//把赋值号左侧也做成一个：=节点的child
	//********************************************************************************************
	t->child.push_back(Exp());
	//t->name.push_back(temp_name);

	return t;
}

TreeNode* Parse::ConditionalStm() {//*********************************************************
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = IfK;

	opposition++;//当前单词好IF进行匹配
	t->child.push_back(Exp());//IF语句的条件表达式
	opposition++;//当前单词和THEN进行匹配
	

	//t->child.push_back(StmL());//条件为真的处理语句        此处的StmL函数还要再写一个新的！？
	t->child.push_back(StmList());
	//暂时先用StmList，之后再改********************************************************************************
	

	if (s[opposition].word.tok == ELSE) {
		opposition++;//当前单词和ELSE进行匹配


		//t->child.push_back(StmL());//条件为假的处理语句    这里的StmL函数同上!!!
		t->child.push_back(StmList());
		//暂时先用StmList，之后再改****************************************************************************


	}

	if (s[opposition].word.tok == FI) {
		opposition++;//当前单词和FI进行匹配
	}
	else {
		cout << "ConditionalStm缺失FI" << "\n";
	}
	return t;
}

TreeNode* Parse::LoopStm() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = WhileK;
	opposition++;//当前单词和While匹配
	t->child.push_back(Exp());//WHILE语句的条件表达式
	if (s[opposition].word.tok == DO) {
		opposition++;//当前单词和DO匹配
	}
	else {
		cout << "LoopStm中DO匹配失败" << "\n";
	}
	t->child.push_back(StmList());//循环语句部分
	opposition++;//匹配endwh
	return t;
}

TreeNode* Parse::InputStm() {
	TreeNode* t = new  TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = ReadK;
	opposition++;//当前单词和READ匹配
	opposition++;//当前单词和LPAREN匹配

	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);
	}
	opposition++;//当前单词和ID进行匹配
	opposition++;//当前单词和RPAREN匹配
	return t;
}

TreeNode* Parse::OutputStm() {
	TreeNode* t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = WriteK;
	opposition++;//当前单词和WRITE匹配
	opposition++;//当前单词和LPAREN匹配
	t->child.push_back(Exp());//WRITE语句的表达式
	opposition++;//当前单词和RPAREN匹配
	return t;
}

TreeNode* Parse::ReturnStm() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = ReturnK;
	opposition++;//当前单词和RETURN匹配
	return t;
}

TreeNode* Parse::CallStmRest() {
	TreeNode *t = new TreeNode;
	t->nodekind = StmtK;
	t->k.stmt = CallK;
	opposition++;//当前单词和LPAREN匹配

	//**********************************************************************************************
	t->child.push_back(temp_name);
	//**********************************************************************************************

	t->child.push_back(ActParamList());
	//t->name.push_back(temp_name);
	opposition++;//当前单词和RPAREN匹配
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
			cout << "ActParamList中出现错误?" << "\n";
			opposition++;//读入下一个单词
		}
	}
	return t;
	//若当前单词为RPAREN，直接返回t,退出函数
}

TreeNode* Parse::ActParamMore() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok != RPAREN) {
		if (s[opposition].word.tok == COMMA) {
			opposition++;//当前单词和COMMA进行匹配
			t = ActParamList();
		}
		else {
			cout << "ActParamMore中出现错误? 缺失]或," << "\n";
			opposition++;//读入下一个单词
		}
	}
	//若当前单词为RPAREN，直接返回t
	return t;
}

TreeNode* Parse::Exp() {
	TreeNode *t = Simple_exp();//***********************有问题????????????????????????
	if (s[opposition].word.tok == LT || s[opposition].word.tok == EQ) {
		TreeNode *p = new TreeNode;
		p->nodekind = ExpK;
		p->k.EXP = OpK;//创建一个新的运算表达式类型语法树节点？

		p->child.push_back(t);//作为运算表达式的左运算简式
		p->op = s[opposition].word.tok;//作为运算表达式的运算符
		opposition++;//匹配当前单词（EQ或LT）
		p->child.push_back(Simple_exp());//作为运算表达式的右运算简式
		return p;
	}
	return t;
	//若不是LT或EQ，直接返回t
}

TreeNode* Parse::Simple_exp() {
	TreeNode *t = term();//调用项处理函数
	while (s[opposition].word.tok == PLUS || s[opposition].word.tok == MINUS) {
		TreeNode *p = new TreeNode;
		p->nodekind = ExpK;
		p->k.EXP = OpK;//创建一个新的运算表达式类型语法树节点

		p->child.push_back(t);//项处理函数的返回值作为运算简单表达式的左运算因子
		p->op = s[opposition].word.tok;//作为运算项的运算符

		//之后，p,t位置互换，t的child为p
		TreeNode *temp = t;
		t = p;
		p = temp;
		//保证每一次循环之后t为根节点，而p是child

		opposition++;//匹配当前单词（PLUS或者MINUS）

		t->child.push_back(term());//作为运算简单表达式的右运算项

		//若为形如a+b+c的式子，构建出来的结构为a+b在底层（结果设为m），m+c在更高一层，最终返回的t一定是最高层
	}
	return t;
}

TreeNode* Parse::term() {
	TreeNode *t = factor();//调用因子处理函数
	while (s[opposition].word.tok == TIMES || s[opposition].word.tok == OVER) {
		TreeNode *p = new TreeNode;
		p->nodekind = ExpK;
		p->k.EXP = OpK;//创建一个新的运算表达式类型语法树节点

		p->child.push_back(t);//将因子处理函数的返回值t作为运算简单表达式的左运算因子
		p->op = s[opposition].word.tok;//作为运算项的运算符

		//之后，p,t位置互换，t的child为p
		TreeNode *temp = t;
		t = p;
		p = temp;
		//保证每一次循环之后t为根节点，而p是child

		opposition++;//匹配当前单词(TIMES或OVER)

		t->child.push_back(factor());//作为运算项的右运算因子

        //本函数结构与Simple_exp相同,参考Simple_exp即可
	}
	return t;
}

TreeNode* Parse::factor() {
	TreeNode *t = NULL;
	if (s[opposition].word.tok == INTC) {
		//数字处理部分？？？字符串——》数字？
		//***************************************************************
		//以下为自己的猜想：
		t = new TreeNode;
		t->nodekind = ExpK;
		t->k.EXP = ConstK;//构建一个新的常数类型语法树节点
		//cout << "const存在哦" << "\n";

		//将当前的数字字符串转换为有意义的数字
		int sum = 0;//记录最终转换为的有意义的数字
		int length = s[opposition].word.str.length();
		for (int i = 0; i < length; i++) {
			sum = sum * 10;
			sum += s[opposition].word.str[i] - '0';
		}
		t->num = sum;//保存这个常数语法树节点的数值

		opposition++;//单词与当前INTC进行匹配
		//***************************************************************
	}
	else if (s[opposition].word.tok == ID) {
		t = variable();
	}
	else if (s[opposition].word.tok == LPAREN) {
		opposition++;//当前单词和LPAREN匹配
		t = Exp();//调用表达式处理函数
		opposition++;//当前单词和RPAREN匹配
	}
	else {
		cout << "当前单词为其他单词，非期望单词错误" << "\n";
		opposition++;//读入下一个单词
	}
	return t;
}

TreeNode* Parse::variable() {
	TreeNode *t = new TreeNode;
	t->nodekind = ExpK;
	t->k.EXP = IdEK;
	t->vark = IdV;
	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);//保存ID名字
		t->lineno = s[opposition].lineshow;//保存行号
		opposition++;//匹配ID
		variMore(t);
	}
	else {
		cout << "variable中ID出现错误??" << "\n";
	}
	return t;
}

void Parse::variMore(TreeNode *t) {//pdf p112
	LexType temptype = s[opposition].word.tok;
	if (temptype != ASSIGN && temptype != TIMES && temptype != EQ && temptype != LT && temptype != PLUS && temptype != MINUS && temptype != OVER && temptype != RPAREN && temptype != RMIDPAREN && temptype != SEMI && temptype != COMMA && temptype != THEN && temptype != ELSE && temptype != FI && temptype != DO && temptype != ENDWHILE && temptype != END) {
		if (temptype == LMIDPAREN) {
			opposition++;//匹配LMIDPAREN
			t->child.push_back(Exp());//T的第一个儿子赋值为exp()
			//这tmd又是个啥？？？？？
			t->vark = ArrayMembV;//t的varkind属性设为ArrayMembV
			t->child[0]->vark = IdV;//t的第一个儿子的varkind设置为IdV
			opposition++;//匹配RMIDPAREN
		}
		else if (temptype == DOT) {
			opposition++;//匹配DOT
			t->child.push_back(fieldvar());//t的第一个儿子赋值为fieldvar()
			//这tmd又是个啥？？？
			t->vark = FieldMembV;
			t->child[0]->vark = IdV;
		}
		else {
			cout << "variMore中出现错误" << "\n";
			opposition++;//读取下一个token
		}
	}
	//若temptype等于以上那些，直接退出函数
}

TreeNode* Parse::fieldvar() {
	TreeNode *t = new TreeNode;
	t->nodekind = ExpK;
	t->k.EXP = IdEK;//创建新的表达式节点

	if (s[opposition].word.tok == ID) {
		t->name.push_back(s[opposition].word.str);//保存当前ID的名字
		t->lineno = s[opposition].lineshow;
		opposition++;//匹配ID
		fieldvarMore(t);
	}
	else {
		cout << "fieldvar中出现错误？？" << "\n";
	}
	return t;
}

void Parse::fieldvarMore(TreeNode *t) {
	LexType temptype = s[opposition].word.tok;
	if (temptype != ASSIGN && temptype != TIMES && temptype != EQ && temptype != LT && temptype != PLUS && temptype != MINUS && temptype != OVER && temptype != RPAREN && temptype != SEMI && temptype != COMMA && temptype != THEN && temptype != ELSE && temptype != FI && temptype != DO && temptype != ENDWHILE && temptype != END) {
		if (temptype == LMIDPAREN) {
			opposition++;//匹配LMIDPAREN
			t->child.push_back(Exp());//t的第一个儿子赋值为exp()
			//这tmd是个啥？？？
			t->child[0]->vark = ArrayMembV;//将t的第一个儿子的varkind设置为ArrayMembV
			opposition++;//匹配PMIDPAREN
		}
		else {
			cout << "fieldvarMore中出现错误??" << "\n";
			opposition++;//读取下一个token
		}
	}
}

void Parse::match(LexType expected) {
	//????????
	//该函数将当前单词与函数参数给定单词相比较，如果一致，则取下一个单词，否则函数退出语法分析程序
	//可用exit(0)退出程序

	if (s[opposition].word.tok == expected) {
		opposition++;//若为指定的单词，取下一个单词
	}
	else {
		//否则，退出当前语法分析程序
		exit(0);
	}
}

void Parse::PrintParseTree() {    //导一个文件指针进来，应该可以通过递归实现
	cout << "执行输出语法树" << "\n";
	TreeNode *nowop = root;
	if (nowop == NULL) {
		cout << "空了??" << "\n";
	}
	int depth = 0;//用于记录当前的语法树节点的深度,用于控制输出空格数量
	DFS(nowop, 0);
}

void Parse::DFS(TreeNode *t, int depth) {
	while (t != NULL) {
		for (int i = 0; i < depth; i++) {
			cout << "		";
		}
		cout << transform(t->nodekind);
		//*******************************************************************
		//以下是对各个节点的具体输出处理
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
				//if的条件表达式、条件为真的处理语句和条件为假的处理语句都放在if节点的child中
				//if中存在问题，如果每一种情况中都有多条语句，输出会混乱
			}
			else if (t->k.stmt == WhileK) {
				cout << " " << "While" << "\n";
				//while的条件表达式和循环语句部分均放在while节点的child中
			}
			else if (t->k.stmt == AssignK) {
				cout << " " << "Assign" << "\n";
				/*
				//assign的赋值号左侧的变量在name中，因此要先输出一下下一层的等号左侧
				for (int i = 0; i <= depth; i++) {
					cout << "		";
				}
				cout << "ExpK" << " " << t->name[0] << " IdV" << "\n";
				//这里面好像并没有a[3]:=?这种操作    具体看函数stm和asscall   没有匹配[的操作
				*/
				//现在等号左侧也被放到child中
			}
			else if (t->k.stmt == ReadK) {
				cout << " " << "Read" << " " << t->name[0] << "\n";
			}
			else if (t->k.stmt == WriteK) {
				cout << " Write" << "\n";
				//write语句的表达式在child中，本层不做处理
			}
			else if (t->k.stmt == CallK) {
				cout << " Call" << "\n";
				/*
				//函数名被放在了call节点的name中，因此应先输出下一层的函数名
				for (int i = 0; i <= depth; i++) {
					cout << "		";
				}
				cout << "ExpK " << t->name[0] << " IdV" << "\n";
				*/
				//现在改为函数名也放到child中
			}
			else if (t->k.stmt == ReturnK) {
				cout << " Return" << "\n";
				//?  我也不知道咋整啊，应该是这样
			}
		}
		else if (t->nodekind == ExpK) {
			if (t->k.EXP == OpK) {
				cout << " OP " << tranform_op_to_string(t->op) << "\n";
				//不论是exp，simple_exp，term还是factor，运算符两边的数或变量均放在child中
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
				//*********************************************************************************存在问题！
				//并没有考虑a[i+1]和a.field的情况，先这么放着
			}
		}
		else {
			cout << "\n";
		}
		//其他语法树节点后不会跟其他东西，只有语法树节点类型
		//*******************************************************************
		for (int j = 0; j < t->child.size(); j++) {//对每一个t的孩子均进行遍历
			DFS(t->child[j], depth + 1);
		}
		t = t->sibling;//去遍历t的兄弟节点
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