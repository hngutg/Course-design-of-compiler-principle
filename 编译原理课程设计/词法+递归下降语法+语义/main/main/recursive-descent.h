#pragma once
#include "pch.h"
#include <vector>

#include "scanner.h"

using namespace std;

//语法树节点的类型
typedef enum {
	//**************标志节点********************
	//这种节点只表示节点的类型，没有具体内容
	ProK,     //根标志节点                     /
	PheadK,   //程序头标志节点                 /
	TypeK,    //类型声明标志节点               /
	VarK,     //变量声明标志节点               /
	ProcDecK, //函数声明标志节点               /
	StmLK,    //语句序列标志节点               /

	//**************具体节点********************
	DecK,	  //声明节点                       /
	StmtK,    //语句节点                       /
	ExpK      //表达式节点
	//具体节点的内部信息见下面
}TreeNodeType;

//定义语法树的声明节点类型
typedef enum {
	ArrayK,   //组类型
	CharK,    //字符类型
	IntegerK, //整数类型
	RecordK,  //记录类型
	Idk       //以类型标识符作为类型
}DecKind;

//定义语法树的语句节点类型
typedef enum {
	IfK,      //判断语句类型
	WhileK,   //循环语句类型
	AssignK,  //赋值语句类型
	ReadK,    //读语句类型
	WriteK,   //写语句类型
	CallK,    //函数调用语句类型
	ReturnK   //返回语句类型
}StmtKind;

//定义语法树的表达式节点的类型
typedef enum {
	OpK,      //操作符类型
	ConstK,   //常整数类型
	IdEK      //标识符类型
}ExpKind;

//定义过程（函数）的参数类型
typedef enum {
	valparamtype,//值参
	varparamtype//变参
}paramType;

//记录当前变量的类型
typedef enum {
	IdV,//变量是标志符变量
	ArrayMembV,//变量是数组成员变量
	FieldMembV//变量是域成员变量
}varKind;

union kind
{
	DecKind dec;   //当nodekind=DecK时有效
	StmtKind stmt; //当nodekind=StmtK时有效
	ExpKind EXP;   //当nodekind=ExpK时有效
};

//定义语法树节点
struct TreeNode {
	vector<TreeNode*> child;//指向子语法书节点指针
	TreeNode *sibling = NULL;//指向兄弟语法树节点指针
	int lineno;//用于记录源程序行号
	TreeNodeType nodekind;//记录语法树节点类型
	kind k;//记录语法树具体节点的具体类型
	/*
	union kind//记录语法树具体节点的具体类型
	{
		DecKind dec;   //当nodekind=DecK时有效
		StmtKind stmt; //当nodekind=StmtK时有效
		ExpKind EXP;   //当nodekind=ExpK时有效
	};*/
	int idnum = 0;//记录一个节点中标志符的个数
	vector<string> name;//用于存储节点中标志符的名字
	string type_name;//记录类型名，点前节点为声明类型，且类型是由类型标识符表示时有效

	int array_low;
	int array_high;//用来记录数组的上下界
	DecKind childType;//记录数组的成员类型
	paramType paramt;//记录过程（函数）的参数类型
	LexType op;//记录语法树节点的运算符单词，可取的值为：EQ, LT, PLUS, MINUS, TIMES, OVER

	int num;//若该语法树节点为常数节点，此节点用来记录这个常数的大小

	varKind vark;//记录语法树节点所记录的变量的类别（标志符、数组成员、域成员）
	//**************************************************************之后的那些节点是干啥的啊？？？
};

static TreeNode* temp_name;//临时变量，在Stm函数中需要使用（估计大致作用是保存等号左侧的变量名称，直到得到等号右边的变量后一并输出）

class Parse {
public:
	void getResult(token s[]);
	void PrintParseTree();//语法树输出函数
	TreeNode* getRoot();//得到语法树的根节点（用于语义分析）
private:
	token *s;
	int opposition = 0;//用于记录当前正在对哪个token进行操作
	//**************************************************************需要把tokenlist也放到这个类的private中？

	TreeNode *root = NULL;//用于保存根节点的指针

	TreeNode* parse();//调用总程序处理函数，创建语法分析树

	TreeNode* Program();//总程序处理分析程序
	//**************************************************************DOT应该如何处理？

	TreeNode* ProgramHead();//程序头部分处理分析程序

	TreeNode* DeclarePart();//程序声明部分处理分析程序

	TreeNode* TypeDec();//类型声明处理分析程序

	//类型声明中的其他函数
	TreeNode* TypeDeclaration();//6 根据产生式，匹配保留字TYPE，调用函数TypeDecList
	TreeNode* TypeDecList();//7 根据产生式，创建新的声明类型节点t，调用TypeId()，匹配保留字EQ，调用，调用TyoeDef(),匹配保留字SEMI，调用函数TypeDecMore()。
	TreeNode* TypeDecMore();//8

	void TypeId(TreeNode *t);//9 类型声明中新声明的类型名称处理分析程序

	void TypeDef(TreeNode *t);//10 类型处理分析程序

	void BaseType(TreeNode *t);//11 基本类型处理分析程序

	void StructureType(TreeNode *t);//12 结构类型处理分析程序

	void ArrayType(TreeNode *t);//13 数组类型的处理分析程序

	void RecType(TreeNode *t);//14 记录类型的处理分析程序

	TreeNode* FieldDecList();//15 记录类型中的域声明处理分析函数

	TreeNode* FieldDecMore();//16 记录类型中点的其他域声明处理分析函数

	void IdList(TreeNode *t);//17 记录类型域中标识符名处理分析程序

	void IdMore(TreeNode *t);//18 记录类型域中其他标识符名处理分析程序

	TreeNode* VarDec();//19 变量声明处理分程序

	//变量声明部分处理程序
	TreeNode* VarDeclaration();//20
	TreeNode* VarDecList();//21
	TreeNode* VarDecMore();//22

	//变量声明部分变量名部分处理程序
	void VarIdList(TreeNode *t);//23
	void VarIdMore(TreeNode *t);//24

	TreeNode* ProcDec();//25 过程声明部分总的处理分析程序

	TreeNode* ProcDeclaration();//26 过程声明部分具体的处理分析程序

	void ParamList(TreeNode *t);//27 过程声明中的参数声明部分的处理分析程序

	//过程声明中的参数声明其他部分的处理分析程序
	TreeNode* ParamDecList();//28 
	TreeNode* ParamMore();//29

	TreeNode* Param();//30 过程声明中的参数声明中参数部分的处理分析程序

	//过程声明中的参数声明中参数名部分的处理分析程序
	void FormList(TreeNode *t);//31 
	void FidMore(TreeNode *t);//32

	TreeNode* ProcDecPart();//33 过程中声明部分的分析处理程序

	TreeNode* ProcBody();//34 过程体部分处理分析程序

	TreeNode* ProgramBody();//35 主程序体部分处理分析程序

	TreeNode* StmList();//36 语句序列部分处理分析程序

	TreeNode* StmMore();//37 更多语句部分处理分析程序

	TreeNode* Stm();//38 语句递归处理分析程序

	TreeNode* AssCall();//39 赋值语句和函数调用语句部分的处理分析程序

	TreeNode* AssignmentRest();//40 赋值语句处理分析函数

	TreeNode* ConditionalStm();//41 条件语句处理分析程序        其中存在问题，StmL函数没写

	TreeNode* LoopStm();//42 循环语句处理分析程序

	TreeNode* InputStm();//43 输入语句的处理分析程序

	TreeNode* OutputStm();//44 输出语句处理分析程序

	TreeNode* ReturnStm();//45 过程返回语句处理分析程序

	TreeNode* CallStmRest();//46 过程调用语句处理分析程序

	TreeNode* ActParamList();//47 实参部分处理分析程序

	TreeNode* ActParamMore();//48 更多实参部分处理分析程序

	TreeNode* Exp();//49 表达式递归处理分析程序

	TreeNode* Simple_exp();//50 简单表达式递归处理分析程序

	TreeNode* term();//51 项递归处理分析程序

	TreeNode* factor();//52 因子递归处理分析程序

	TreeNode* variable();//53 变量处理程序

	void variMore(TreeNode *t);//54 变量其余部分处理程序

	TreeNode* fieldvar();//55 域变量部分处理过程

	void fieldvarMore(TreeNode *t);//56 域变量其他部分处理过程

	void match(LexType expected);//57 终极符匹配处理程序

	void DFS(TreeNode *t, int depth);//深度优先遍历函数，用于递归输出语法树

	string transform(TreeNodeType type);//将树的节点枚举类型转换为可输出的string

	string transform_deckind_to_string(DecKind kind);//将树的deckind枚举类型转换为可输出的string

	string tranform_op_to_string(LexType op);//将树的操作符节点的op枚举类型转换为可输出的string
};



