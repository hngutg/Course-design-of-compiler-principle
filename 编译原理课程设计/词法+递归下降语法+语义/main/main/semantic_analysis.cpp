#include "pch.h"

#include "semantic_analysis.h"

#include <iostream>

using namespace std;

//符号表实现

void Semantic::CreateTable() {
	level++;//scope栈的层数+1
	SymbTable *newscope = new SymbTable;
	Scope.push_back(*newscope);//在Scope中压入一个新的SymbTable变量，表示创立新的一层，且这个变量的next=null，表示这一层中还为空（这个节点中并不记录信息）
	//************************************************************************有问题！！！！！！！！！！！！！！！！！
	//这个偏移量到底是什么？   实例程序中表明我认为的正确的
	int off = 7;
	OffsetScope.push_back(off);//这一层的初始偏移量off为0，但存在initoff为7
	//************************************************************************
}

void Semantic::DestroyTable() {
	//撤销一个符号表，认为本层符号表均无效，先delete这一层的全部SymbTable，再pop了Scope
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
	//事例程序中留了一个keep的scope栈，是干什么的?
}

bool Semantic::SearchoneTable(string id, int currentLevel, SymbTable **Entry) {
	//若找到，Entry为标识符在符号表中的位置，因此应该返回一个指针(地址)
	(*Entry) = Scope[currentLevel].next;
	while ((*Entry) != NULL) {
		if (id == (*Entry)->idname) {
			return true;
		}
		(*Entry) = (*Entry)->next;
	}
	return false;
}

bool Semantic::FindEntry(string id, Flag flag, SymbTable **Entry) {//从符号表中查找标识符，一般来说flag=down
	bool present = SearchoneTable(id, level, Entry);
	//**********************************************************如果在本层找到，又在其他层找到，返回哪一个entry啊？
	//暂且认为是优先返回本层
	if (present == false) {//在本层并没有找到这个标识符
		if (flag == up) {//向更高层进行查找
			int oplevel = level + 1;//当前操作的层数，不应该改变level
			while (oplevel < Scope.size()) {
				Entry = &(Scope[oplevel].next);//每次操作，Entry初始化为oplevel层的首项
				bool temp = SearchoneTable(id, oplevel, Entry);
				if (temp == true) {
					present = true;
					break;//若找到了，直接返回
				}
				else {
					oplevel++;
				}
			}
		}
		else if (flag == down) {//向更低层进行查找
			int oplevel = level - 1;
			while (oplevel >= 0) {
				(*Entry) = Scope[oplevel].next;//每次操作，Entry初始化为oplevel层的首项
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

bool Semantic::Enter(string id, AttributeIR *AttribP, SymbTable **Entry) {   //登记标识符和属性到符号表
	bool present = FindEntry(id, current, Entry);//flag定为current，外层的标识符定义并不会影响本层，不会出现标识符重复声明错误

	if (present == true) {
		cout << id << "标识符重复声明错误" << "\n";
	}
	else {
		SymbTable *temp = new SymbTable;//新建一个符号表项
		temp->idname = id;//复制标识符名
		//复制标识符属性
		temp->attrIR.idtype = AttribP->idtype;
		temp->attrIR.kind = AttribP->kind;
		switch (AttribP->kind)
		{
		case type://当kind == typeKind时
			break;
		case var://当kind == varKind时
			temp->attrIR.More.VarAttr.level = AttribP->More.VarAttr.level;
			temp->attrIR.More.VarAttr.access = AttribP->More.VarAttr.access;
			temp->attrIR.More.VarAttr.off = AttribP->More.VarAttr.off;
			break;
		case proc://当kind == procKind时
			//code在生成目标代码时期填写，感觉现在用不到
			temp->attrIR.More.ProcAttr.level = AttribP->More.ProcAttr.level;
			temp->attrIR.More.ProcAttr.param = AttribP->More.ProcAttr.param;
			temp->attrIR.More.ProcAttr.size = AttribP->More.ProcAttr.size;
			//***********************************************************************************************
		default:
			break;
		}
		//将此符号表项链入当前符号表尾
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
		//若Entry不为空，令Entry指向标识符在标识符表中的位置***********************************************
		*Entry = temp;
	}
	return present;
}

bool Semantic::FindField(string id, fieldChain *head, fieldChain **Entry) {//在域表中查找域名
	//head指向域表头，id为所找的标识符。若查找成功，则entry记录标识符在域表中的位置
	fieldChain *temp = head;
	while (temp != NULL) {
		if (temp->idname == id) {//若查找成功
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

//SNL语义分析实现

void Semantic::getResult(TreeNode *t) {
	Analyze(t);
}

void Semantic::Analyze(TreeNode *currentP) {//***************************************************

	SymbTable * Entry = NULL;
	TreeNode * p = NULL;
	TreeNode * pp = currentP;

	CreateTable();
	initialize();
	//循环处理主程序的声明部分
	//直接在声明节点开始
	//第一个节点是ProgramHead，没有用

	p = currentP->child[1];//处理DeclarePart
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
			cout << "语法树中没有这种类型的节点" << endl;
		}
		p = p->sibling;
	}

	//if(Error) return;
	//实例程序中，若出现错误，直接返回

	//处理主程序体
	p = currentP->child[2];//处理ProgramBody
	if (p->nodekind == StmLK) {
		Body(p);
	}

	PrintSymbTable();

	//撤销符号表
	if (level != -1)
		DestroyTable();
}

void Semantic::initialize() {//***************************************************
	//初始化整数类型，字符类型，布尔类型的内部表示，说明由于这三种类型均为基本类型，内部表示固定
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
	else if (deckind == IntegerK) {//直接返回整形指针
		ptr = intPtr;
	}
	else if (deckind == CharK) {//直接返回字符指针
		ptr = charPtr;
	}
	return ptr;
}

TypeIR* Semantic::nameType(TreeNode *t) {//该函数用来在符号表中寻找已定义的类型名字
	//***************************************************************************************************
	TypeIR* ptr = NULL;
	SymbTable *Entry = NULL;
	Entry = Scope[level].next;  // SearchoneTable中已经进行了初始化 没必要了
	bool present = FindEntry(t->type_name, down, &Entry);//此处flag应为down，如果在本层没有找到，应尝试向更外层去找
	if (present == true) {
		if (Entry->attrIR.kind == type) {
			ptr = Entry->attrIR.idtype;
		}
		else {
			cout << "类型" << t->type_name << "在未声明时就使用" << "\n";
			//是否应该向文件中输入错误信息?   比如函数ErrorPrompt
		}
	}
	else {
		cout << "类型未定义" << "\n";
		//是否应该向文件中输入错误信息?   比如函数ErrorPrompt
	}
	return ptr;
}

TypeIR* Semantic::arrayType(TreeNode *t) {//数组类型内部表示处理函数

	TypeIR  * Ptr0 = NULL;//保存下标类型的返回
	TypeIR  * Ptr1 = NULL;//保存元素类型的返回
	TypeIR  * Ptr = NULL;

	if (t->array_high < t->array_low) {
		cout << "数组下标越界错误" << "\n";
	}
	else {
		//***********************************************************************************
		//有很大问题
		Ptr0 = TypeProcess(t, IntegerK);//调用类型分析函数，处理下标类型?
		Ptr1 = TypeProcess(t, t->childType);//调用类型分析函数，处理元素类型
		                             //处理元素类型到底处理的是什么？   可能是数组中存储的类型
		//***********************************************************************************

		Ptr = NewTy(arrayTy);//创建新的类型信息表
		Ptr->size = (t->array_high - t->array_low + 1)*(Ptr1->size);//计算本类型长度

		//填写其他信息
		Ptr->More.ArrayAttr.elemTy = Ptr1;//给出当前数组的元素类型
		Ptr->More.ArrayAttr.indexTy = Ptr0;//给出当前数组的下标类型
		//***************************************************是否需要记录数组的上下界？

	}
	return Ptr;
}

TypeIR* Semantic::recordType(TreeNode *t) {//处理记录类型的内部表示函数

	TypeIR  * Ptr = NewTy(recordTy);//新建记录类型的节点
	TreeNode *opt = t;
	opt = opt->child[0];//读取域信息

	fieldChain  *  Ptr2 = NULL;
	fieldChain  *  Ptr1 = NULL;//跟在Ptr2后面的一个工作指针

	fieldChain  *  body = NULL;//保存这个函数中生成的所有域变量的头fieldChain（相当于链表head）

	while (opt != NULL) {
		//循环处理一个节点有多个id的情况
		//此处循环是处理此种情况int a,b;
		for (int i = 0; i < opt->idnum; i++) {
			Ptr2 = NewBody();//申请新的域类型单元结构Ptr2
			if (body == NULL) {//当body为空时的特殊处理
				body = Ptr2;
				Ptr1 = Ptr2;
			}
			//填写Ptr2的各个成员内容
			Ptr2->idname = opt->name[i];
			Ptr2->unitType = TypeProcess(opt, opt->k.dec);
			Ptr2->next = NULL;

			//如果ptr1！=ptr2,将ptr1指针处理之后后移
			if (Ptr1 != Ptr2) {
				Ptr2->offset = Ptr1->offset + Ptr1->unitType->size;//计算新申请的节点的off
				//这种处理方式也使得第一个节点的offset为0
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
		}
		//处理完同类型的变量后，取语法树的兄弟节点
		opt = opt->sibling;
	}

	//取最后的Ptr2的(offset+size)为最后整个记录的size
	Ptr->size = Ptr2->offset + Ptr2->unitType->size;
	//将域链链入记录类型的body部分
	Ptr->More.body = body;

	return Ptr;
}

//**********************声明的语义分析**********************

void Semantic::TypeDecPart(TreeNode *t) {//类型声明部分处理函数
	bool present = false;

	AttributeIR *AttribP = new AttributeIR;
	AttribP->kind = type;
	
	SymbTable * Entry = NULL;
	TreeNode *opt = t;

	while (opt != NULL) {
		//调用记录属性函数，返回是否重复声明错和入口地址
		present = Enter(opt->name[0], AttribP, &Entry);
		//若没有找到，那么enter函数会完成如表操作，entry返回在符号表中的位置

		if (present == true) {
			cout << opt->name[0] << "出现重复声明" << "\n";
			//向文件中输出错误信息？？？
			//在类中保存状态？？？？   是否出错
			Entry = NULL;
		}
		else {//未出错,没有重复声明
			Entry->attrIR.idtype = TypeProcess(opt, opt->k.dec);
		}
		opt = opt->sibling;
	}
}

void Semantic::VarDecList(TreeNode *t) {//变量声明部分分析处理函数
	//处理变量声明的语义分析
	AttributeIR *AttribP = new AttributeIR;
	bool present = false;
	SymbTable *Entry = NULL;

	TreeNode *opt = t;
	while (opt != NULL) {
		AttribP->kind = var;
		for (int i = 0; i < opt->name.size(); i++) {//循环处理同一个节点的id
			AttribP->idtype = TypeProcess(opt, opt->k.dec);
			if (opt->paramt == varparamtype) {//若变量为变参
				AttribP->More.VarAttr.level = level;//记录层数
				AttribP->More.VarAttr.access = indir;//变量为间接变量
				//*****************************************************************************************
				//这个offset的变动？  有问题
				AttribP->More.VarAttr.off = OffsetScope[level];
				//如果是变参，偏移+1
				OffsetScope[level] += 1;
			}
			else {//若变量为实参
				AttribP->More.VarAttr.level = level;
				AttribP->More.VarAttr.access = dir;//变量为直接变量
				//计算值参的偏移
				if (AttribP->idtype != NULL)
				{
					AttribP->More.VarAttr.off = OffsetScope[level];
					//cout << OffsetScope[level] << endl;
					OffsetScope[level] += AttribP->idtype->size;//偏移加变量类型的size
				}
			}

			//登记该变量的属性及名字,判断是否被重复定义,并返回其类型内部指针
			present = Enter(opt->name[i], AttribP, &Entry);
			if (present == true) {//找到了，说明出现了重复定义
				cout << opt->name[0] << "出现了重复定义" << "\n";
				//向文件中输出错误信息？？？
			    //在类中保存状态？？？？   是否出错
				return;
			}
			//*********************************************************************************************
			//opt->table是个什么东西？？？？
		}
		opt = opt->sibling;
	}
	//*********************************************************************************************
	//此处与示例程序不同！！！！！！
}

void Semantic::ProcDecPart(TreeNode *t) {//过程声明部分分析处理函数**********************************************
	//处理过程声明的语义分析
	//在当前层符号表中填写过程标识符的属性；在新层符号表中填写形参标识符的属性。
	//TreeNode *p = t;
	SymbTable *Entry = HeadProcess(t);//处理过程头

	//*****************************
	if (Entry->attrIR.More.ProcAttr.param->entry == NULL) {
		cout << "null" << endl;
	}
	//*****************************
	
	TreeNode *opt = t->child[1];//暂时认为DeclarePart部分存在
	//如果过程内部存在声明部分，则处理声明部分
	//***************************************************************************
	//暂且认为这些东西都是齐全的!!!!!!
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
			//如果声明部分有函数声明，则跳出循环，先填写noff和moff等信息
			//我写的函数中并没有noff和moff，是否要改？？？？
			ProcDecPart(opt->child[0]);
			break;
		default:
			cout << "ProcDecPart出现问题  没有这种声明类型" << "\n";
			break;
		}
		if (opt->nodekind == ProcDecK)
			break;
		else
			opt = opt->sibling;
	}
	//处理体部分
	Body(t->child[2]);
	//调用函数，结束当前scope
	//结束当前符号表之前输出？
	PrintOneLayer(level);
	if (level != -1) {
		DestroyTable();
	}
}

SymbTable* Semantic::HeadProcess(TreeNode *t) {//过程声明头分析函数
	//在当前层符号表中填写函数标识符的属性；在新层符号表中填写形参标识符的属性。
	//其中过程的大小和代码都需以后回填。
	AttributeIR *AttribP = new AttributeIR;
	bool present = false;
	SymbTable *Entry = NULL;

	//填写过程标识符的属性
	AttribP->kind = proc;
	AttribP->idtype = NULL;
	AttribP->More.ProcAttr.level = level + 1;//本过程的层数=当前层数+1

	if (t != NULL) {
		//登记过程的符号表项
		present = Enter(t->name[0], AttribP, &Entry);
	}
	//调用形参处理函数
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

ParamTable* Semantic::ParaDecList(TreeNode *t) {//形参分析处理函数
	//在新的符号表中登记所有形参的表项，构造形参表项的地址表，并有param指向此地址表。
	TreeNode * p = NULL;
	ParamTable  * Ptr1 = NULL;
	ParamTable  * Ptr2 = NULL;
	ParamTable  * head = NULL;

	if (t != NULL) {
		if (t->child.size() != 0) {//当前语法树节点的第一个儿子不为null
			p = t->child[0];
		}

		//进入新的局部化区
		CreateTable();
		//子程序中的变量初始偏移设为8
		OffsetScope[level] = 7;    
		//处理变量声明部分
		VarDecList(p);

		SymbTable * Ptr0 = Scope[level].next;

		while (Ptr0 != NULL) {
			//构造形参符号表，并使其连接至符号表的param项
			Ptr2 = new ParamTable;
			//函数NewParam只是做next和entry的初始化，这已经在.h中实现了
			if (head == NULL) {
				Ptr1 = Ptr2;
				head = Ptr2;
			}

			//Ptr2->entry->idname = Ptr0->idname;
			Ptr2->entry = new SymbTable;
			Ptr2->entry->attrIR = Ptr0->attrIR;

			Ptr2->next = NULL;
			//****

			if (Ptr2 != Ptr1)//链表链接
			{
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
			Ptr0 = Ptr0->next;
		}
	}
	return head;//返回形参符号表的头指针
}

void Semantic::Body(TreeNode *t) {//执行体部分分析处理函数
	//SNL编译系统的执行体部分即为语句序列,故只需处理语句序列部分。
	if (t->nodekind == StmLK) {
		TreeNode * p = t->child[0];
		while (p != NULL) {//循环处理语句序列
			statement(p);//调用语句状态处理函数
			p = p->sibling;
		}
	}
}

void Semantic::statement(TreeNode *t) {//语句序列分析处理函数
	//根据语法树节点中的kind项判断应该转向处理哪个语句类型函数
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
		cout << "statement中语句类型出错\n";
	}
}

TypeIR* Semantic::Expr(TreeNode *t, AccessKind *Ekind) {//表达式分析处理函数
	//表达式语义分析的重点是检查运算分量的类型相容性，求表达式的类型。其中参数Ekind用来表示实参是变参还是值参。
	int present = false;
	SymbTable *Entry = NULL;
	
	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;

	if (t != NULL) {
		if (t->k.EXP == ConstK) {//若为常量
			Eptr = TypeProcess(t, IntegerK);
			Eptr->kind = intTy;
			if (Ekind != NULL) {
				*Ekind = dir;//直接变量
			}
		}
		else if (t->k.EXP == IdEK) {//若为变量
			//Var = id的情形
			if (t->vark == IdV) {
				//在符号表中查找此标识符
				present = FindEntry(t->name[0], down, &Entry);
				//在程序中,:=左侧的也被放到了child中，因此，外层if的判断变为了依靠vark，且id变为了t->child[0]->name[0]

				if (present == true) {//找到了
					if (Entry->attrIR.kind != var) {//若id不为变量
						cout << "Expr中出错，" << t->name[0] << "不是变量" << "\n";
						//向文件中输出错误信息？？？
				        //在类中保存状态？？？？   是否出错
					}
					else {
						Eptr = Entry->attrIR.idtype;
						if (Ekind != NULL) {
							*Ekind = indir;//间接变量
						}
					}
				}
				else {
					//标识符无声明
					cout << "Expr中出错，" << t->name[0] << "没有声明" << "\n";
					//向文件中输出错误信息？？？
					//在类中保存状态？？？？   是否出错
				}
			}
			//Var = Var0[E]的情形
			else if (t->vark == ArrayMembV) {
				Eptr = arrayVar(t);
			}
			//Var = Var0.id的情形
			else if (t->vark == FieldMembV) {
				Eptr = recordVar(t);
			}
			else {
				cout << "Expr中出现错误" << "\n";
			}
		}
		else if (t->k.EXP == OpK) {//若为操作符
			//递归调用儿子节点
			Eptr0 = Expr(t->child[0], NULL);
			if (Eptr0 == NULL) {
				return NULL;
			}
			Eptr1 = Expr(t->child[1], NULL);
			if (Eptr1 == NULL) {
				return NULL;
			}

			//类型判别
			if (Eptr0 != Eptr1) {
				present = false;//类型不相等
			}
			else {
				present = true;//类型相等
			}

			if (present != false) {//若类型相同
				switch (t->op)
				{
					//LT、EQ均为bool表达式
				case LT:
				case EQ:
					Eptr = boolPtr;
					break;  /*条件表达式*/
					//以下四种表达式得到的均是intptr
				case PLUS:
				case MINUS:
				case TIMES:
				case OVER:
					Eptr = intPtr;
					break;  /*算数表达式*/
				}
				if (Ekind != NULL) {
					(*Ekind) = dir;//直接变量
				}
			}
			else {
				cout << "运算符两侧不相容\n";
				//向文件中输出错误信息？？？
				//在类中保存状态？？？？   是否出错
			}
		}
	}
	return Eptr;
}

TypeIR* Semantic::arrayVar(TreeNode *t) {//数组变量的处理分析函数
	//该函数处理数组变量的下标分析
	//检查var := var0[E]中var0是不是数组类型变量，E是不是和数组的下标变量类型匹配.
	bool present = false;
	SymbTable *	Entry = NULL;

	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;

	//在符号表中查找此标识符
	present = FindEntry(t->name[0], down, &Entry);

	if (present == true) {
		if (Entry->attrIR.kind != var) {//Var0不是变量
			cout << "arrayVar中，" << t->name[0] << "不是变量\n";
			Eptr = NULL;
			//向文件中输出错误信息？？？
		    //在类中保存状态？？？？   是否出错
		}
		else {
			if (Entry->attrIR.idtype != NULL) {//保险措施，防止对null操作
				if (Entry->attrIR.idtype->kind != arrayTy) {//Var0不是数组类型变量
					cout << "arrayVar中，" << t->name[0] << "不是数组变量\n";
					Eptr = NULL;
					//向文件中输出错误信息？？？
			        //在类中保存状态？？？？   是否出错
				}
				else {
					//检查E的类型是否与下标类型相符
					Eptr0 = Entry->attrIR.idtype->More.ArrayAttr.indexTy;//得到设定的下标的类型
					if (Eptr0 == NULL) {
						cout<<"arrayVar中," << t->name[0] << "下标没有设定的类型?\n";
						return NULL;
					}
					Eptr1 = Expr(t->child[0], NULL);//得到实际的下标的类型
					if (Eptr1 == NULL) {
						cout << "arrayVar中," << t->name[0] << "实际的下标子节点没有类型?\n";
						return NULL;
					}
					//比较两种类型是否相同
					if (Eptr0 == Eptr1) {
						present == true;
					}
					else {
						present = false;
					}

					if (present != true) {
						cout << "arrayVar中," << t->name[0] << "数组的下标并不匹配\n";
						Eptr = NULL;
						//向文件中输出错误信息？？？
					    //在类中保存状态？？？？   是否出错
					}
					else {
						Eptr = Entry->attrIR.idtype->More.ArrayAttr.elemTy;//指针数组元素类型内部表示的指针
					}
				}
			}
		}
	}
	else {//符号表中没找到这个数组变量
		cout << "arrayVar中，" << t->name[0] << "没有声明\n";
		//向文件中输出错误信息？？？
		//在类中保存状态？？？？   是否出错
	}
	return Eptr;
}

TypeIR* Semantic::recordVar(TreeNode *t) {//记录变量中域变量的分析处理函数
	//检查var:=var0.id中的var0是不是记录类型变量，id是不是该记录类型中的域成员。
	bool present = false;
	SymbTable *	Entry = NULL;
	bool result = true;

	TypeIR * Eptr0 = NULL;
	TypeIR * Eptr1 = NULL;
	TypeIR * Eptr = NULL;
	fieldChain *currentP = NULL;

	//在符号表中查找此标识符
	present = FindEntry(t->name[0], down, &Entry);
	//如果找到了
	if (present == true) {
		//Var0不是变量
		if (Entry->attrIR.kind != var) {
			cout << "recordVar中，" << t->name[0] << "不是变量\n";
			Eptr = NULL;
			//向文件中输出错误信息？？？
			//在类中保存状态？？？？   是否出错
		}
		else {//到底要不要保险措施？？？
			
			//Var0不是记录类型变量
			if (Entry->attrIR.idtype->kind != recordTy) {
				cout << "recordVar中，" << t->name[0] << "不是记录类型变量\n";
				Eptr = NULL;
				//向文件中输出错误信息？？？
			    //在类中保存状态？？？？   是否出错
			}
			//检查id是否是合法域名
			else {
				Eptr0 = Entry->attrIR.idtype;//得到标识符的类型内部表示
				currentP = Eptr0->More.body;//得到域变量内部的第一个单元
				while ((currentP != NULL) && (result == true)) {
					//在域变量内部去寻找id（id被存储在t->child[0]->name[0]，这是函数体部分，不是声明，child里就是储存的id）
					//通过currentP遍历整个域变量内部
					if (t->child[0]->name[0] == currentP->idname) {//找到了
						result = false;
						Eptr = currentP->unitType;
					}
					else {//否则
						result = true;
						currentP = currentP->next;
					}
				}
				//如果没有找到
				if (currentP == NULL) {
					cout << "recordVar中，" << t->child[0]->name[0] << "不在域中\n";
					Eptr = NULL;
				}
				else {
					//如果id是一个数组变量
					if (t->child[0]->child.size() > 0) {
						Eptr = arrayVar(t->child[0]);
					}
				}
			}
		}
	}
	//否则，标识符无声明
	else {
		cout << "recordVar中" << t->name[0] << "没有声明\n";
	}
	return Eptr;//返回指向记录元素类型内部表示的指针
}

void Semantic::assignstatement(TreeNode *t) {//赋值语句分析函数
	//赋值语句的语义分析的重点是检查赋值号两端分量的类型相容性。
	SymbTable *Entry = NULL;

	int present = false;
	TypeIR *Ptr = NULL;
	TypeIR *Eptr = NULL;

	TreeNode *child1 = t->child[0];
	TreeNode *child2 = t->child[1];

	if (child1->child.size() == 0) {//若等号左边的为正常变量（而非数组或域）
		//在符号表中查找此标识符
		present = FindEntry(child1->name[0], down, &Entry);

		if (present == true) {
			//若等号左侧的并非变量
			if (Entry->attrIR.kind != var) {
				Eptr = NULL;
				cout << "assignstatement中，等号左侧的" << child1->name[0] << "并非变量" << endl;
			}
			else {
				Eptr = Entry->attrIR.idtype;
			}
		}
		//否则，标识符无生命
		else {
			cout << "assignstatement中，等号左侧的" << child1->name[0] << "没有声明" << endl;
		}
	}
	else {
		//若等号左侧的为数组，如Var0[E]
		if (child1->vark == ArrayMembV) {
			Eptr = arrayVar(child1);
		}
		//若等号左侧为域变量，如Var0.id
		else if (child1->vark == FieldMembV) {
			Eptr = recordVar(child1);
		}
		else {
			cout << "那你等号左边到底是个啥呢？" << endl;
		}
	}
	if (Eptr != NULL) {
		if ((t->nodekind == StmtK) && (t->k.stmt == AssignK)) {//保险，若这个节点是一个赋值语句
			//检查是不是赋值号两侧 类型等价
			Ptr = Expr(child2, NULL);
			if (Ptr != Eptr) {
				cout << "assignstatement中，赋值号左右两侧类型不匹配" << endl;
			}
		}
		//赋值语句中不能出现函数调用
	}
}

void Semantic::callstatement(TreeNode *t) {//过程调用语句分析处理函数
	//函数调用语句的语义分析首先检查符号表求出其属性中的Param部分（形参符号表项地址表），
	//并用它检查形参和实参之间的对应关系是否正确。
	AccessKind *Ekind = new AccessKind;
	bool present = false;
	SymbTable *Entry = NULL;
	TreeNode *p = NULL;

	//用id检查整个符号表
	present = FindEntry(t->child[0]->name[0], down, &Entry);

	//*********************************
	if (Entry->attrIR.More.ProcAttr.param->entry == NULL) {
		cout << "null" << endl;
	}
	//*********************************

	//若未找到，函数无声明
	if (present == false) {
		cout << "callstatement中，" << t->child[0]->name[0] << "函数未定义" << endl;
	}
	else {
		//若id不是函数名
		if (Entry->attrIR.kind != proc) {
			cout << "callstatement中，" << t->child[0]->name[0] << "并非函数名" << endl;
		}
		else {
			//形参实参匹配
			p = t->child[1];
			//paramP指向形参符号表的表头
			ParamTable *paramP = Entry->attrIR.More.ProcAttr.param;
			while (p != NULL && paramP != NULL) {
				SymbTable *paraEntry = paramP->entry;
				TypeIR *Etp = Expr(p, Ekind);//实参
				//参数类型不匹配
				if (paraEntry->attrIR.More.VarAttr.access == indir && *Ekind == dir) {
					cout << "callstatement中，" << "参数类型不匹配" << endl;
				}
				else {
					//参数类型不匹配
					if (paraEntry->attrIR.idtype != Etp) {
						cout << "callstatement中，" << "参数类型不匹配" << endl;
					}
				}
				p = p->sibling;
				paramP = paramP->next;
			}
			//参数个数不匹配
			if (p != NULL || paramP != NULL) {
				cout << "callstatement中，" << "参数数量不匹配" << endl;
			}
		}
	}
}

void Semantic::ifstatement(TreeNode *t) {//条件语句分析处理函数
	//分析语法树的三个儿子节点
	//child[0]是if中的判断式
	//child[1]是then中的语句
	//child[2]是else中的语句    else不一定存在
	AccessKind * Ekind = NULL;
	TypeIR *Ept = Expr(t->child[0], Ekind);
	if (Ept != NULL) {
		//处理条件表达式
		if (Ept->kind != boolTy) {
			cout << "ifstatement中，逻辑表达式错误" << endl;
		}
		else {
			TreeNode *p = t->child[1];
			while (p != NULL) {
				statement(p);
				p = p->sibling;
			}
			if (t->child.size() == 3) {//如果else节点存在
				TreeNode *p = t->child[2];
				while (p != NULL) {
					statement(p);
					p = p->sibling;
				}
			}
		}
	}
}

void Semantic::whilestatement(TreeNode *t) {//循环语句分析处理函数
	TypeIR *  Etp = Expr(t->child[0], NULL);
	if (Etp != NULL) {//处理条件表达式部分		
		//如果条件表达式不是bool类型
		if (Etp->kind != boolTy) {
			cout << "whilestatement中，WHILE语句的条件表达式错误" << endl;
		}
		else {
			TreeNode *p = t;
			//处理循环体内的语句
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

	//用id检查整个符号表
	present = FindEntry(t->name[0], down, &Entry);

	//如果没找到，说明变量没有声明
	if (present = false) {
		cout << "readstatement函数中，" << t->name[0] << "没有声明" << endl;
	}
	else {
		//如果t->name[0]不是变量标识符
		if (Entry->attrIR.kind != var) {
			cout << "readstatement函数中，" << t->name[0] << "不是变量标识符" << endl;
		}
	}
}

void Semantic::writestatement(TreeNode *t) {//写语句分析处理函数
	//分析输出语句中的表达式是否合法
	TypeIR * Etp = Expr(t->child[0], NULL);

	if (Etp != NULL) {
		//如果表达式类型为bool类型，报错
		if (Etp->kind == boolTy) {
			cout << "writestatement函数中，" << t->name[0] << "输出类型错误" << endl;
		}
	}
}

void Semantic::returnstatement(TreeNode *t) {//该函数处理函数返回语句分析
	//分析函数返回语句是否在主程序中出现
	if (level == 0)
		/*如果返回语句出现在主程序中，报错*/
		cout << "returnstatement函数中，return语句出错" << endl;
}

//*****************其余函数******************

TypeIR* Semantic::NewTy(Typekind kind) {//创建当前空类型内部表示
	//参数为类型，函数返回该类型的内部表示的地址
	TypeIR *table = new TypeIR;
	if (table == NULL) {
		cout << "out of memory" << "\n";
	}
	else {
		//类型内部表示类型指针table不是NULL,内存单元已经成功分配
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
			table->More.ArrayAttr.elemTy = NULL;//指向数组元素类型的内部表示  初始化
			table->More.ArrayAttr.indexTy = NULL;//指向数组下标类型的内部表示 初始化
			table->size = 0;//数组大小需要计算才能确定
			break;
		case recordTy:
			table->kind = kind;
			table->More.body = NULL;//记录类型中的域链 初始化
			table->size = 0;//记录类型的大小需要计算确定
			break;
		}
		
	}
	return table;
}

fieldChain* Semantic::NewBody() {//创建当前空记录类型中域的链表
	//函数返回该类型的新的链表的单元地址

	fieldChain *ptr = new fieldChain;

	//对当前新的链表单元进行初始化
	ptr->next = NULL;
	ptr->unitType = NULL;
	ptr->offset = 0;

	return ptr;
}

//*****************输出函数******************
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
	{ /*输出标识符名字*/
		cout << t->idname<<" ";
		//fprintf(listing, "%s:   ", t->idName);
		AttributeIR  *Attrib = &(t->attrIR);
		/*输出标识符的类型信息，过程标识符除外*/
		if (Attrib->idtype != NULL)  /*过程标识符*/
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
		/*输出标识符的类别，并根据不同类型输出不同其它属性*/
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
			//是否需要设立一个noff来存储sp到display表的偏移量？
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
{ /*层数从0开始*/
	int  l = 0;
	while (l<Scope.size())
	{
		PrintOneLayer(l);
		l++;
	}
}

