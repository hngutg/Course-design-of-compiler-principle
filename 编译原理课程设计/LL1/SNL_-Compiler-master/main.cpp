#include"Lex_Analysis.h"
#include<bits/stdc++.h>
#include <windows.h>
using namespace std;
#define rep(i,a,b) for(int i=a; i<b; i++)
#define per(i,a,b) for(int i=b-1; i>=a; i--)
#define pb push_back
const int N = 500;
const int M = 1e6+10;
vector<string> wenfa[N];
vector<int> First[N],Follow[N],Predict[N],next[N];
int haskong[N];
map<string, int> fu;
string fubianhao[N];
int table[N][N];
int vnvt = 0,funum=0 ;       //0~vnvt-1是非终结符,vnvt-funum-1是非终结符
vector<Token> yang;
int s[M];
int len_s;
stack<int> sta;

int tot =0;
int iszimu(char c)
{
    if((c<='z'&&c>='a')||(c<='Z'&&c>='A'))
        return 1;
    return 0;
}
void wenfazhuanhuan()
{
    ifstream in("SNL.txt");
    string filename;
    string s;
    if(!in)
    {
        cout <<"no such file" << endl;
        return;
    }
    while (getline (in, s))
    {
        int len = s.length();
        rep(i, 0, len)
        {
            if(s[i]==':'&&i+1<len)
            {
                if(s[i+1]==':'&&i+2<len)
                    i+=2;
                else
                {
                    wenfa[tot].pb(":=");
                    i++;
                }
            }
            else if(i+1<len&&s[i]=='.'&&s[i+1]=='.')
            {
                wenfa[tot].pb("..");
                i++;
            }
            else if(iszimu(s[i]))
            {
                string tmp = "";
                while(iszimu(s[i])&&i<len)
                {
                    tmp+=s[i];
                    i++;
                }
                i--;
                wenfa[tot].pb(tmp);
            }
            else if(s[i]==' '||s[i]=='\t')
                continue;
            else if(s[i]=='|')
            {
                tot++;
                wenfa[tot].pb(wenfa[tot-1][0]);
            }
            else
            {
                string tmp="";
                tmp+=s[i];
                wenfa[tot].pb(tmp);
            }
        }
        tot++;
    }
    rep(i, 0, tot)
    {
        if(fu.find(wenfa[i][0])==fu.end())
        {
            fu.insert(pair<string,int>(wenfa[i][0],funum));
            fubianhao[funum++] = wenfa[i][0];
        }
    }
    vnvt=funum;
    rep(i, 0, tot)
    {
        rep(j, 1, wenfa[i].size())
        {
            if(fu.find(wenfa[i][j])==fu.end())
            {
                fu.insert(pair<string,int>(wenfa[i][j],funum));
                fubianhao[funum++] = wenfa[i][j];
            }
        }
    }
    fu.insert(pair<string,int>("#",funum));
    fubianhao[funum++] = "#";
}
set<int> firsting;
void getonefirst(int x)
{
    if(First[x].size()!=0||firsting.find(x)!=firsting.end())
        return;
    firsting.insert(x);
    set<int> tmp;
    rep(i, 0, next[x].size())
    {
        int ii = next[x][i];
        int y = fu[wenfa[ii][1]];
        int flag=0;
        int j=1;
        while(j<wenfa[ii].size())
        {
            y = fu[wenfa[ii][j]];
            if(y>=vnvt)
            {
                tmp.insert(y);
                break;
            }
            getonefirst(y);
            if(j==wenfa[ii].size()-1)
            {
                 rep(k, 0, First[y].size())
                 {

                    tmp.insert(First[y][k]);
                 }

                break;
            }
            rep(k, 0, First[y].size())
                if(fubianhao[First[y][k]]!="KONG")
                    tmp.insert(First[y][k]);
            if(!haskong[y])
                break;
            j++;

        }
    }
    for(std::set<int>::iterator it = tmp.begin();it!=tmp.end();it++)
	{
	    First[x].pb(*it);
	    if(fubianhao[*it]=="KONG")
            haskong[x]=1;
	}
	firsting.erase(x);
}
void getfirst()
{
    ofstream output("answer/first.txt",ios::trunc);
    memset(haskong, 0, sizeof(haskong));
    rep(i, 0, tot)
    {
        int x = fu[wenfa[i][0]];
        next[x].pb(i);
    }
    for(std::map<string,int>::iterator it = fu.begin();it!=fu.end();it++)
	{
	    firsting.clear();
	    if((it->second)<vnvt)
            getonefirst(it->second);
        if((it->second)>=vnvt)
            continue;
        output<<fubianhao[it->second]<<"                 ";
        rep(i, 0, First[it->second].size())
            output<<" "<<fubianhao[First[it->second][i]];
        output<<endl;
	}
	output.close();
}


int bing(vector<int> &a, vector<int> b, int flag)  //flag=1表示去除b的空
{
    int last = a.size();
    set<int> tmp;
    rep(i, 0, a.size())
        tmp.insert(a[i]);
    rep(i, 0, b.size())
    {
        if(flag==1&&fubianhao[b[i]]=="KONG")
            continue;
        tmp.insert(b[i]);
    }
    a.clear();
    for(std::set<int>::iterator it = tmp.begin();it!=tmp.end();it++)
	    a.pb(*it);
    if(a.size()>last)
        return 1;
    return 0;
}
void getfollow()
{
    ofstream output("answer/follow.txt",ios::trunc);
    vector<int> tmp;
    string S = wenfa[0][0];
    Follow[fu[S]].pb(fu["#"]);
    int shoulian = 1;
    while(shoulian)
    {
        shoulian = 0;
        rep(i, 0, tot)
        {
            rep(j, 1, wenfa[i].size())
            {
                int y = fu[wenfa[i][0]];
                int x= fu[wenfa[i][j]];
                if(x>=vnvt)
                    continue;
                int k = j, flag=1;
                while(k!=wenfa[i].size()&&flag==1)
                {
                    flag=0;
                    k++;
                    if(k==wenfa[i].size())
                    {
                        shoulian|= bing(Follow[x],Follow[y],0);
                        break;
                    }
                    int kk = fu[wenfa[i][k]];
                    if(kk>=vnvt)
                    {
                        tmp.clear();
                        tmp.pb(kk);
                        shoulian|= bing(Follow[x],tmp,0);
                        break;
                    }
                    if(haskong[kk])
                    {
                        shoulian|=bing(Follow[x],First[kk],1);
                        flag=1;
                    }
                    else if(!haskong[kk])
                    {
                        shoulian|=bing(Follow[x],First[kk],0);
                    }
                }
            }
        }
    }
    for(std::map<string,int>::iterator it = fu.begin();it!=fu.end();it++)
	{
	    if((it->second)>=vnvt)
            continue;
        output<<fubianhao[it->second]<<"                 ";
        rep(i, 0, Follow[it->second].size())
            output<<" "<<fubianhao[Follow[it->second][i]];
        output<<endl;
	}
    output.close();
}

void getpredict()
{
    ofstream output("answer/predict.txt",ios::trunc);
    set<int> tmp;
    rep(i, 0, tot)
    {

        tmp.clear();
        int x = fu[wenfa[i][0]];
        if(wenfa[i][1]=="KONG")
            Predict[i] = Follow[x];
        else if(fu[wenfa[i][1]]>=vnvt)
            Predict[i].pb(fu[wenfa[i][1]]);
        else
        {
            int y = fu[wenfa[i][1]];
            if(!haskong[y])
                rep(j, 0, First[y].size())
                    Predict[i].pb(First[y][j]);
            else
            {
                rep(j, 0, First[y].size())
                    if(fubianhao[First[y][j]]!="KONG")
                        tmp.insert(First[y][j]);
                int k = 1;
                y =  fu[wenfa[i][k]];
                while(haskong[y])
                {
                    k++;
                    if(k==wenfa[i].size())
                    {
                        rep(j, 0, Follow[x].size())
                            tmp.insert(Follow[x][j]);
                        break;
                    }
                    y =  fu[wenfa[i][k]];
                    if(y>=vnvt)
                    {
                        tmp.insert(y);
                        break;
                    }
                    rep(j, 0, First[y].size())
                        if(fubianhao[First[y][j]]!="KONG")
                            tmp.insert(First[y][j]);
                }
                for(std::set<int>::iterator it = tmp.begin();it!=tmp.end();it++)
                    Predict[i].pb(*it);
            }
        }
        output<<i+1<<" ";
        rep(j, 0 , Predict[i].size())
            output<<" "<<fubianhao[Predict[i][j]];
        output<<endl;

    }
    output.close();
}
int LL1()
{
    ofstream output("answer/par_result.txt",ios::trunc);
    int flag=1;
    memset(table, -1, sizeof(table));
    rep(i, 0, tot)
    {
        int l = fu[wenfa[i][0]];
        rep(j, 0, Predict[i].size())
        {
            int r = Predict[i][j];
            table[l][r]=i;
        }
    }
    int i=0;
    sta.push(fu["#"]);
    sta.push(fu[wenfa[0][0]]);
    while(!sta.empty())
    {
        int now = sta.top();
        sta.pop();
        if(now==s[i])
            i++;
        else if(table[now][s[i]]!=-1)
        {
            int k = table[now][s[i]];
            output<<wenfa[k][0]<<"->";
            rep(j, 1, wenfa[k].size())
                output<<wenfa[k][j]<<" ";
            output<<endl;
            per(j, 1, wenfa[k].size())
            {
                if(wenfa[k][j]!="KONG")
                    sta.push(fu[wenfa[k][j]]);
            }
        }
        else
        {
            if(i==len_s-1)
            {
                cout<<"第"<<yang[i-1].i_line<<"行	"<<"语法错误";
                output<<"第"<<yang[i-1].i_line<<"行	"<<"语法错误";
            }
            else
            {
                cout<<"第"<<yang[i].i_line<<"行	"<<"语法错误: "<<yang[i].str_sem<<endl;
                output<<"第"<<yang[i].i_line<<"行	"<<"语法错误: "<<yang[i].str_sem<<endl;
            }

            while(table[now][s[i]]==-1&&now!=s[i]&&i<len_s)
                i++;
            flag=0;
        }
        if(i>=len_s)
            break;
    }
    output.close();
    if(i!=len_s||flag==0)
        return 0;
    return 1;

}

void ci_yu(lex_analysis * l_a)
{
    yang.clear();
    l_a->result_hjz(yang);
    rep(i, 0, yang.size())
    {
        string tmp = yang[i].str_type;
        string tmps = yang[i].str_sem;
        if(tmp=="ID"||tmp=="EOF")
            s[i]=fu[tmp];
        else if(tmp=="RESERVEWORD")
        {
            transform(tmps.begin(), tmps.end(), tmps.begin(), ::toupper);
            s[i]= fu[tmps];
        }
        else if(tmp=="INTEGER")
            s[i]= fu["INTC"];
        else
            s[i]=fu[yang[i].str_sem];
    }
    s[yang.size()]=fu["#"];
    len_s = yang.size()+1;
}

int main()
{
    lex_analysis* l_a = new lex_analysis();
	string str_FileName="test.txt";
	l_a->Scan(str_FileName);
	l_a->ResultPrint(1);
	//l_a->ResultPrint(0);
	cout<<"结束词法分析"<<endl;
	cout<<"词法分析结果成功记录在lex_result.txt中"<<endl;
    if(l_a->iswrong())
        return 0;
    wenfazhuanhuan();
    getfirst();
    getfollow();
    getpredict();
    ci_yu(l_a);
	delete l_a;
	cout<<"结束语法分析"<<endl;
	cout<<"语法分析结果记录在par_result.txt中"<<endl;
    if(LL1())
        cout<<"success! "<<endl;
    else
        cout<<"fail！ "<<endl;
    return 0;
}
