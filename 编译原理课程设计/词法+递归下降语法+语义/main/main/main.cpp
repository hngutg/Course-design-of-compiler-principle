// main.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<string>

#include "scanner.h"
#include "recursive-descent.h"
#include "semantic_analysis.h"

using namespace std;

int main()
{
	FILE* fpin;//用于打开文件
	string file_name;//文件名 

	Scanner scanner;

	fopen_s(&fpin,"c3.txt", "r");//根据文件名打开文件，打开方式是读取
	if (!fpin)//如果没有找到这个文件
	{
		cerr << "打开文件" << file_name << "失败" << endl;
		return -1;
	}

	

	token* s = scanner.getTokenList(fpin); //获取tokenlist
	scanner.PrintTokenList();   //输出tokenlist

	Parse p;//递归下降
	p.getResult(s);
	//cout << "?";

	p.PrintParseTree();

	Semantic semantic;//语义分析
	semantic.getResult(p.getRoot());
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
