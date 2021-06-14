#pragma once
#include "pch.h"

#include "recursive-descent.h"

#include <string>

using namespace std;

//***************************************指针！！！！！以后看看是不是需要提前初始化为null

//定义标识符的类型
typedef enum {
	type,//类型标识符 typeKind
	var,//变量标识符  varKind
	proc//过程标识符  procKind
}IdKind;

//定义变量是直接还是间接变量
typedef enum {
	dir,//直接变量
	indir//间接变量
}AccessKind;

//定义SNL中的类型的种类
typedef enum {
	//标准类型：    其内部表示可以实现构造
	intTy,//整数类型
	charTy,//字符类型
	//**********************************
	//数组和记录类型等构造类型要在变量声明或类型声明时构造
	arrayTy,//数组类型
	recordTy,//记录类型
	//**********************************
	boolTy//布尔类型
}Typekind;

//用于函数FindEntry中，标识是否需要在更高一层或者更低一层符号表中进行标识符查找
typedef enum {
	up,       //需要在更高一层进行查找
	current,  //只需要在本层进行查找
	down      //需要在更低一层进行查找
}Flag;

struct TypeIR;
struct ParamTable;
struct fieldChain;
struct AttributeIR;
struct SymbTable;

//类型内部结构
struct TypeIR {
	int size;//类型所占空间大小
	Typekind kind;//具体类型
	union
	{
		struct {//当类型为数组类型时，对各种信息的记录
			TypeIR *indexTy;//当类型为数组类型时有效，指向数组下标类型
			TypeIR *elemTy;//当类型为数组类型时有效，指向数组元素类型
		}ArrayAttr;
		fieldChain *body;//记录类型中的域链
	}More;
};

//定义形参信息表的一项
struct ParamTable {
	ParamTable *next = NULL;//指向下一个形参
	SymbTable *entry = NULL;//指向形参标识符在符号表中的位置
};

//定义域类型单元结构
struct fieldChain {
	string idname;//记录域中的标识符
	TypeIR *unitType;//指向域中成员的内部表示        ************是这样吗?  到底是成员的内部表示还是类型
	int offset;//所在记录中的偏移
	fieldChain *next = NULL;//链表指针
};

//定义标识符信息项
struct AttributeIR {
	TypeIR *idtype = NULL;//指针，指向标识符的类型内部表示
	IdKind kind;//标识符的类型
	union //保存不同类型的标识符的属性   标识符的不同类型有不同的属性
	{
		//变量标识符的属性，当kind==varKind时有效
		struct {
			AccessKind access;//直接变量还是间接变量
			int level;//表示该变量声明所在主过程/过程的层数
			int off;//表示该变量相对它所在主过程/过程的偏移量
		}VarAttr;

		//过程标识符的属性,当kind==procKind时有效
		struct {
			int level;//表示过程层数
			ParamTable *param;//形参信息表地址
			int code;//表示过程的目标代码地址    这个部分在代码生成阶段填写
			int size;//表示过程所需的空间大小
		}ProcAttr;
	}More;
};

//定义SNL符号表
struct SymbTable {
	string idname;//记录标识符的名字
	AttributeIR attrIR;//标识符信息项  记录标识符的各项信息
	SymbTable *next = NULL;//指向符号表的下一项？
};


class Semantic {
private:
	//分层建立符号表，使各分程序的符号表项连续的排列在一起
	vector<SymbTable> Scope;//分程序表，用来记录各层分程序符号表的起始位置

	vector<int> OffsetScope;//用这个vector去保存每一层现阶段的偏移量

	int level = -1;//表示当前符号表的层数,在未创建时为-1

	TypeIR * intPtr;		//该指针一直指向整数类型的内部表示
	TypeIR * charPtr;		//该指针一直指向字符类型的内部表示
	TypeIR * boolPtr;		//该指针一直指向布尔类型的内部表示

	//***********************以下为函数***********************

	void Analyze(TreeNode *currentP);//1 语义分析主函数

	void initialize();//2 初始化基本类型内部表示函数

	TypeIR* TypeProcess(TreeNode *t, DecKind deckind);//3 类型分析处理函数

	TypeIR* nameType(TreeNode *t);//4 自定义类型内部结构分析函数

	TypeIR* arrayType(TreeNode *t);//5 数组类型内部表示处理函数

	TypeIR* recordType(TreeNode *t);//6 处理记录类型的内部表示函数

	void TypeDecPart(TreeNode *t);//7 类型声明部分分析处理函数

	void VarDecList(TreeNode *t);//8 变量声明部分分析处理函数

	void ProcDecPart(TreeNode *t);//9 过程声明部分分析处理函数

	SymbTable* HeadProcess(TreeNode *t);//10 过程声明头分析函数

	ParamTable* ParaDecList(TreeNode *t);//11 形参分析函数

	void Body(TreeNode *t);//12 执行体部分分析处理函数

	void statement(TreeNode *t);//13 语句分析处理函数

	TypeIR* Expr(TreeNode *t, AccessKind *Ekind);//14 表达式分析处理函数

	TypeIR* arrayVar(TreeNode *t);//15 数组变量的处理分析函数

	TypeIR* recordVar(TreeNode *t);//16 记录变量中域变量的分析处理函数

	void assignstatement(TreeNode *t);//17 赋值语句分析函数

	void callstatement(TreeNode *t);//18 过程调用语句分析处理函数

	void ifstatement(TreeNode *t);//19 条件语句分析处理函数

	void whilestatement(TreeNode *t);//20 循环语句分析处理函数

	void readstatement(TreeNode *t);//21 读语句分析处理函数

	void writestatement(TreeNode *t);//22 写语句分析处理函数

	void returnstatement(TreeNode *t);//23 返回语句分析处理函数   书上没有？！？

	void CreateTable();//创建一个符号表

	void DestroyTable();//撤销一个符号表

	bool SearchoneTable(string id, int currentLevel, SymbTable **Entry);//从表头开始，依次将节点中的标识符名字和id比较是否相同，直到找到标识符或到达表尾

	bool FindEntry(string id, Flag flag, SymbTable **Entry);//在符号表中查找标识符

	void PrintSymbTable();//从第一层开始，依次打印各层符号表内容

	void PrintOneLayer(int level);//打印当前层的符号表内容

	bool Enter(string id, AttributeIR *AttribP, SymbTable **Entry);//登记标识符和属性到符号表

	bool FindField(string id, fieldChain *head, fieldChain **Entry);//在域表中查找域名

	TypeIR * NewTy(Typekind  kind);//创建当前空类型内部表示

	fieldChain* NewBody();//创建当前空记录类型中域的链表
public:
	void getResult(TreeNode *root);//
};
